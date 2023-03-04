/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Api.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/ElapsedTime.h"
#include "pico/time.h"

using namespace Driver::RHTemp::HUT31D;

#define SECT_ "Driver::RHTemp::HUT31D"

#define MAX_CONVERSION_TIME 22

static uint8_t htu31dCrc( uint16_t value );
static float convertRH( uint16_t rawRH );
static float convertTemp( uint16_t rawTemp );

//////////////////////////////////////////////////////////////////////////////
Api::Api( Driver::I2C::Master& i2cDriver, uint8_t i2cDevice7BitAddress )
    : m_i2cDriver( i2cDriver )
    , m_devAddress( i2cDevice7BitAddress )
    , m_started( false )
{
}

void Api::initialize() noexcept
{
    // Nothing needed
}

bool Api::start() noexcept
{
    // Skip processing if already started
    if ( !m_started )
    {
        // Check if the IC is responsive/connected
        uint8_t data[2] ={ 0, };
        auto result = m_i2cDriver.registerRead<uint8_t[2]>( m_devAddress, 0x08, data );
        if ( result == Driver::I2C::Master::eSUCCESS )
        {
            // Check if there are any errors (The LSb is heater on/off state)
            if ( htu31dCrc( data[0] ) == data[1] && (data[0] & 0xFE) == 0x00 )
            {
                m_sampleState = eNOT_STARTED;
                m_started     = true;
                return true;
            }
            CPL_SYSTEM_TRACE_MSG( SECT_, ("HUT31D Sensor has one more errors: %02X %02X", data[0], data[1]) );
        }
        else
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("HUT31D Sensor is not responding (i2c addr=%02X). result=%d", m_devAddress, result) );
        }

    }

    return false;
}

void Api::stop() noexcept
{
    m_started = false;
}

//////////////////////////////////////////////////////////////////////////////
bool Api::sample( float& rhOut, float& tempCOut ) noexcept
{
    // Fail if sampling is in progress
    if ( m_sampleState == eSAMPLING )
    {
        return false;
    }

    // Start the conversion
    if ( startConversion() == Driver::I2C::Master::eSUCCESS )
    {
        Cpl::System::Api::sleep( MAX_CONVERSION_TIME );

        if ( readConversionResult( rhOut, tempCOut ) == Driver::I2C::Master::eSUCCESS )
        {
            return true;
        }
    }

    return false;
}

Driver::I2C::Master::Result_T Api::startConversion()
{
    uint8_t conversion = 0x40 | (0x0F << 1); // Highest accuracy
    return m_i2cDriver.writeToDevice( m_devAddress, sizeof( conversion ), &conversion );
}

Driver::I2C::Master::Result_T Api::readConversionResult( float& rhOut, float& tempCOut )
{
    uint8_t readCmd = 0;
    Driver::I2C::Master::Result_T result = m_i2cDriver.writeToDevice( m_devAddress, sizeof( readCmd ), &readCmd, true );
    if ( result == Driver::I2C::Master::eSUCCESS )
    {
        uint8_t data[6];
        result = m_i2cDriver.readFromDevice( m_devAddress, sizeof( data ), &data );
        if ( result == Driver::I2C::Master::eSUCCESS )
        {
            uint16_t rawTemp = ((uint16_t) (data[0]) << 8) | data[1];
            uint8_t  crcTemp = htu31dCrc( rawTemp );
            if ( crcTemp == data[2] )
            {
                uint16_t rawRh = ((uint16_t) (data[3]) << 8) | data[4];
                uint8_t  crcRh = htu31dCrc( rawRh );
                if ( crcRh == data[5] )
                {
                    rhOut    = convertRH( rawRh );
                    tempCOut = convertTemp( rawTemp );
                    return Driver::I2C::Master::eSUCCESS;
                }
                else
                {
                    CPL_SYSTEM_TRACE_MSG( SECT_, ("RH CRC failed (calc=%02X, expected%02X", crcRh, data[5]) );
                }
            }
            else
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Temperature CRC failed (calc=%02X, expected%02X", crcTemp, data[2]) );
            }
        }
        else
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Failed to Read Conversion result. result=%d", result) );
        }
    }
    else
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Failed to write READ-RH-TEMP CMD (%02X). result=%d", readCmd, result) );
    }

    return result;
}


//////////////////////////////////////////////////////////////////////////////
Driver::RHTemp::Api::SamplingState_T Api::startSample() noexcept
{
    // Fail if sampling is in progress
    if ( m_sampleState == eSAMPLING )
    {
        return eERROR;
    }

    // Start the conversion
    if ( startConversion() == Driver::I2C::Master::eSUCCESS )
    {
        m_timeMarker  = Cpl::System::ElapsedTime::milliseconds();
        m_sampleState = eSAMPLING;
    }
    else
    {
        m_sampleState = eERROR;
    }

    return m_sampleState;
}

Driver::RHTemp::Api::SamplingState_T Api::checkSamplingTime()
{
    if ( m_sampleState == eSAMPLING && Cpl::System::ElapsedTime::expiredMilliseconds( m_timeMarker, MAX_CONVERSION_TIME ) )
    {
        m_sampleState = eSAMPLE_READY;
    }

    return m_sampleState;
}

Driver::RHTemp::Api::SamplingState_T Api::getSamplingState() noexcept
{
    return checkSamplingTime();
}

Driver::RHTemp::Api::SamplingState_T Api::getSample( float& rhOut, float& tempCOut ) noexcept
{
    if ( checkSamplingTime() == eSAMPLE_READY )
    {
        if ( readConversionResult( rhOut, tempCOut ) != Driver::I2C::Master::eSUCCESS )
        {
            m_sampleState = eERROR;
        }
    }

    return m_sampleState;
}

//////////////////////////////////////////////////////////////////////////////
bool Api::setHeaderState( bool enabled ) noexcept
{
    uint8_t heaterCmd = enabled ? 0x04 : 0x02;
    Driver::I2C::Master::Result_T result = m_i2cDriver.writeToDevice( m_devAddress, sizeof( heaterCmd ), &heaterCmd );
    if ( result == Driver::I2C::Master::eSUCCESS )
    {
        return true;
    }

    CPL_SYSTEM_TRACE_MSG( SECT_, ("Failed to set the heater state (%d). result=%d", enabled, result) );
    return false;
}

//////////////////////////////////////////////////////////////////////////////
float convertRH( uint16_t rawRH )
{
    // Convert to % (0-100)
    float rh =  (rawRH / 65535.0) * 100;
    return rh;
}

float convertTemp( uint16_t rawTemp )
{
    // Convert to degrees Centigrade
    float temp = (rawTemp / 65535.0) * 165 - 40;
    return temp;
}

uint8_t htu31dCrc( uint16_t value )
{
    uint32_t polynom = 0x988000; // x^8 + x^5 + x^4 + 1
    uint32_t msb     = 0x800000;
    uint32_t mask    = 0xFF8000;
    uint32_t result  = (uint32_t) value << 8; // Pad with zeros as specified in spec

    while ( msb != 0x80 )
    {
        // Check if msb of current value is 1 and apply XOR mask
        if ( result & msb )
        {
            result = ((result ^ polynom) & mask) | (result & ~mask);
        }

        // Shift by one
        msb >>= 1;
        mask >>= 1;
        polynom >>= 1;
    }

    return result;
}

