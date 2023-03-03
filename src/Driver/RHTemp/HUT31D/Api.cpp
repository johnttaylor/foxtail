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
#include "pico/time.h"

using namespace Driver::RHTemp::HUT31D;

#define SECT_ "Driver::RHTemp::HUT31D"

//////////////////////////////////////////////////////////////////////////////
Api::Api( Driver::I2C::Master& i2cDriver, uint8_t i2cDevice7BitAddress )
    : m_i2cDriver( i2cDriver )
    , m_devAddress( i2cDevice7BitAddress )
    , m_started( false )
{
}

bool Api::start() noexcept
{
    // Skip processing if already started
    if ( !m_started )
    {
        // Check if the IC is responsive/connected
        uint8_t diagVal;
        auto result = m_i2cDriver.registerRead<uint8_t>( m_devAddress, 0x08, diagVal );
        if ( result == Driver::I2C::Master::eSUCCESS )
        {
            // Check if there are any errors (The LSb is heater on/off state)
            if ( (diagVal & 0xFE) == 0x00 )
            {
                m_started = true;
                return true;
            }
            CPL_SYSTEM_TRACE_MSG( SECT_, ("HUT31D Sensor has one more errors: %02X", diagVal) );
        }
        else
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("HUT31D Sensor is not responding (i2c addr=%02X).", m_devAddress) );
        }

    }

    return false;
}

void Api::stop() noexcept
{
    m_started = false;
}

//////////////////////////////////////////////////////////////////////////////
bool Api::sampleRHAndTemperature( float& rhOut, float& tempCOut, Accuracy_T requestedAccuracy = eLOWEST ) noexcept
{
    return false;
}


/// See Driver::RHTemp::Api
bool Api::sampleRH( float& rhOut, Accuracy_T requestedAccuracy = eLOWEST ) noexcept
{
    return false;
}


/// See Driver::RHTemp::Api
bool Api::sampleTemperature( float& rhOut, Accuracy_T requestedAccuracy = eLOWEST ) noexcept
{
    return false;
}


bool Api::setHeaderState( bool enabled ) noexcept
{
    return false;
}

//////////////////////////////////////////////////////////////////////////////
//bool Api::write( size_t dstOffset, const void* srcData, size_t numBytesToWrite ) noexcept
//{
//    // Fail if not started or write is out of range
//    if ( !m_started ||
//         (dstOffset + numBytesToWrite) > (OPTION_DRIVER_NV_ONSEMI_CAT24C512_NUM_PAGES * OPTION_DRIVER_NV_ONSEMI_CAT24C512_BYTES_PER_PAGE) )
//    {
//        return false;
//    }
//
//    // The writes transactions can not cross page boundaries - so break up the write
//    // into multiple page writes
//    size_t startPage            = dstOffset / OPTION_DRIVER_NV_ONSEMI_CAT24C512_BYTES_PER_PAGE;
//    size_t bytesRemainingInPage = (startPage + 1) * OPTION_DRIVER_NV_ONSEMI_CAT24C512_BYTES_PER_PAGE - dstOffset;
//    const uint8_t* srcPtr       = (const uint8_t*) srcData;
//
//    while ( numBytesToWrite )
//    {
//        size_t bytesToWrite = numBytesToWrite > bytesRemainingInPage ? bytesRemainingInPage : numBytesToWrite;
//        if ( !writePage( dstOffset, srcPtr, bytesToWrite ) )
//        {
//            return false;
//        }
//
//        numBytesToWrite     -= bytesToWrite;
//        dstOffset           += bytesToWrite;
//        srcPtr              += bytesToWrite;
//        bytesRemainingInPage = OPTION_DRIVER_NV_ONSEMI_CAT24C512_BYTES_PER_PAGE;
//    }
//
//    // If get here - all of the provided bytes have been written
//    return true;
//}
//
//bool Api::read( size_t srcOffset, void* dstData, size_t numBytesToRead ) noexcept
//{
//    // Fail if not started or read is out of range
//    if ( !m_started ||
//         (srcOffset + numBytesToRead) > (OPTION_DRIVER_NV_ONSEMI_CAT24C512_NUM_PAGES * OPTION_DRIVER_NV_ONSEMI_CAT24C512_BYTES_PER_PAGE) )
//    {
//        return false;
//    }
//
//    // Set the read address/offset (but keep retrying if a write-cycle is in progress)
//    if ( !setReadOffsetPointer( srcOffset ) )
//    {
//        return false;
//    }
//
//    // Read the data
//    auto result = m_i2cDriver.readFromDevice( m_devAddress, numBytesToRead, dstData );
//    return result == Driver::I2C::Master::eSUCCESS;
//}
//
//bool Api::setReadOffsetPointer( size_t newOffset )
//{
//    unsigned retryCount = OPTION_DRIVER_NV_ONSEMI_CAT24C512_MAX_WAIT_RETRIES;
//    uint8_t  address[2];
//    address[0] = (uint8_t) ((newOffset >> 8) & 0xff);
//    address[1] = (uint8_t) ((newOffset & 0xFF));
//
//    // Retry the transaction if there write-cycle-in-progress
//    do
//    {
//        auto result = m_i2cDriver.writeToDevice( m_devAddress, 2, address );
//        if ( result == Driver::I2C::Master::eSUCCESS )
//        {
//            return true;
//        }
//        Cpl::System::Api::sleep( OPTION_DRIVER_NV_ONSEMI_CAT24C512_MAX_WAIT_RETRIES );
//    } while ( retryCount-- );
//
//    return false;
//}
//
//bool Api::writePage( size_t offset, const uint8_t* srcBuffer, size_t numBytesToWrite )
//{
//    // Setup temporary page buffer that contains the eeprom offset of the page being written
//    unsigned retryCount = OPTION_DRIVER_NV_ONSEMI_CAT24C512_MAX_WAIT_RETRIES;
//    uint8_t  pageBuffer[OPTION_DRIVER_NV_ONSEMI_CAT24C512_BYTES_PER_PAGE + 2];
//    pageBuffer[0] = (uint8_t) ((offset >> 8) & 0xff);
//    pageBuffer[1] = (uint8_t) ((offset & 0xFF));
//    memcpy( pageBuffer + 2, srcBuffer, numBytesToWrite );
//
//    // Retry the transaction if there write-cycle-in-progress
//    do
//    {
//        auto result = m_i2cDriver.writeToDevice( m_devAddress, 2 + numBytesToWrite, pageBuffer );
//        if ( result == Driver::I2C::Master::eSUCCESS )
//        {
//            return true;
//        }
//        Cpl::System::Api::sleep( OPTION_DRIVER_NV_ONSEMI_CAT24C512_MAX_WAIT_RETRIES );
//    } while ( retryCount-- );
//
//    return false;
//}
//
//
//size_t Api::getNumPages() const noexcept
//{
//    return OPTION_DRIVER_NV_ONSEMI_CAT24C512_NUM_PAGES;
//}
//
//size_t Api::getPageSize() const noexcept
//{
//    return OPTION_DRIVER_NV_ONSEMI_CAT24C512_BYTES_PER_PAGE;
//}
//
