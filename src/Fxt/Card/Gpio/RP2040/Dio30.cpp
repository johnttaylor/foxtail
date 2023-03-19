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


#include "Dio30.h"
#include "Driver/DIO/RP2040/InOut30.h"
#include "Cpl/System/Assert.h"

///
using namespace Fxt::Card::Gpio::RP2040;

#define MAX_INPUT_CHANNELS      30
#define INPUT_POINT_OFFSET      0

#define MAX_OUTPUT_CHANNELS     30
#define OUTPUT_POINT_OFFSET     (MAX_INPUT_CHANNELS)

#define TOTAL_MAX_CHANNELS      (MAX_INPUT_CHANNELS + MAX_OUTPUT_CHANNELS)


///////////////////////////////////////////////////////////////////////////////
Dio30::Dio30( Cpl::Memory::ContiguousAllocator&  generalAllocator,
              Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
              Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
              Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
              Fxt::Point::DatabaseApi&           dbForPoints,
              JsonVariant&                       cardObject,
              Cpl::Dm::MailboxServer*            cardMboxNotUsed,
              void*                              extraArgsNotUsed )
    : Fxt::Card::Common_( TOTAL_MAX_CHANNELS, generalAllocator, cardObject )
    , m_numInputs( 0 )
    , m_numOutputs( 0 )
{
    memset( m_driverInCfg, 0, sizeof( m_driverInCfg ) );
    memset( m_driverOutCfg, 0, sizeof( m_driverOutCfg ) );

    if ( m_error == Fxt::Type::Error::SUCCESS() )
    {
        parseConfiguration( generalAllocator, cardStatefulDataAllocator, haStatefulDataAllocator, pointFactoryDb, dbForPoints, cardObject );
    }
}



///////////////////////////////////////////////////////////////////////////////
void Dio30::parseConfiguration( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                                Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                                Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                                Fxt::Point::DatabaseApi&           dbForPoints,
                                JsonVariant&                       cardObject ) noexcept
{
    // Parse channels
    if ( !cardObject["points"].isNull() )
    {
        // INPUTS
        JsonArray inputs = cardObject["points"]["inputs"];
        if ( !inputs.isNull() )
        {
            // Validate supported number of signals
            size_t nInputs = inputs.size();
            if ( nInputs > MAX_INPUT_CHANNELS )
            {
                m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::TOO_MANY_INPUT_POINTS );
                m_error.logIt( getTypeName() );
                return;
            }
            m_numInputs = (uint8_t) nInputs;

            // Create Virtual Points
            for ( size_t idx=0; idx < m_numInputs; idx++ )
            {
                uint16_t   channelNum_notUsed;
                JsonObject channelObj = inputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_virtualInputs,
                                       Fxt::Point::Bool::GUID_STRING,
                                       false,
                                       channelObj,
                                       m_error,
                                       MAX_INPUT_CHANNELS,
                                       INPUT_POINT_OFFSET,
                                       channelNum_notUsed,
                                       generalAllocator,
                                       cardStatefulDataAllocator,
                                       dbForPoints );

                // Stop processing if/when an error occurred
                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }

            }

            // Create IO Register Points and Driver Configuration:Inputs
            for ( size_t idx=0; idx < m_numInputs; idx++ )
            {
                uint16_t   channelNum;
                JsonObject channelObj = inputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_ioRegisterInputs,
                                       Fxt::Point::Bool::GUID_STRING,
                                       true,
                                       channelObj,
                                       m_error,
                                       MAX_INPUT_CHANNELS,
                                       INPUT_POINT_OFFSET,
                                       channelNum,
                                       generalAllocator,
                                       cardStatefulDataAllocator,
                                       dbForPoints );

                // Stop processing if/when an error occurred
                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }

                // Parse Pull resistor option
                parseDriverConfig( channelObj, m_driverInCfg, idx, channelNum );
            }
        }

        // OUTPUTS
        JsonArray outputs = cardObject["points"]["outputs"];
        if ( !outputs.isNull() )
        {
            // Validate supported number of signals
            size_t nOutputs = outputs.size();
            if ( nOutputs > MAX_OUTPUT_CHANNELS )
            {
                m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::TOO_MANY_OUTPUT_POINTS );
                m_error.logIt( getTypeName() );
                return;
            }
            m_numOutputs = (uint8_t) nOutputs;

            // Create Virtual Points
            for ( size_t idx=0; idx < m_numOutputs; idx++ )
            {
                uint16_t   channelNum_notUsed;
                JsonObject channelObj = outputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_virtualOutputs,
                                       Fxt::Point::Bool::GUID_STRING,
                                       false,
                                       channelObj,
                                       m_error,
                                       MAX_OUTPUT_CHANNELS,
                                       OUTPUT_POINT_OFFSET,
                                       channelNum_notUsed,
                                       generalAllocator,
                                       haStatefulDataAllocator,     // All Output Virtual Points are part of the HA Data set
                                       dbForPoints );

                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }
            }

            // Create IO Register Points
            for ( size_t idx=0; idx < m_numOutputs; idx++ )
            {
                uint16_t   channelNum;
                JsonObject channelObj = outputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_ioRegisterOutputs,
                                       Fxt::Point::Bool::GUID_STRING,
                                       true,
                                       channelObj,
                                       m_error,
                                       MAX_OUTPUT_CHANNELS,
                                       OUTPUT_POINT_OFFSET,
                                       channelNum,
                                       generalAllocator,
                                       cardStatefulDataAllocator,
                                       dbForPoints );

                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }

                // Parse Pull resistor option
                parseDriverConfig( channelObj, m_driverOutCfg, idx, channelNum );
            }
        }

        // Make sure that no IO pin has been 'doubled-up'
        for ( int idx = 0; idx < MAX_INPUT_CHANNELS; idx++ )
        {
            if ( m_ioRegisterPoints[idx + INPUT_POINT_OFFSET] != nullptr &&
                 m_ioRegisterPoints[idx + OUTPUT_POINT_OFFSET] != nullptr )
            {
                m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
                m_error.logIt( getTypeName() );
                return;
            }
        }
    }
}

void Dio30::parseDriverConfig( JsonObject& channelObj, Driver::DIO::InOut::Config_T cfg[], size_t arrayIdx, uint16_t channelNum )
{
    cfg[arrayIdx].pin  = channelNum - 1;  // Note: ChannelNum is one-based and the Driver index is zero-based
    cfg[arrayIdx].blob = Driver::DIO::RP2040::INOUT_CFG_NO_PULL_UPDOWN;
    const char* pulls  = channelObj["pulls"];
    if ( pulls )
    {
        if ( *pulls == 'u' || *pulls == 'U' )
        {
            cfg[arrayIdx].blob = Driver::DIO::RP2040::INOUT_CFG_PULL_UP;
        }
        else if ( *pulls == 'd' || *pulls == 'D' )
        {
            cfg[arrayIdx].blob = Driver::DIO::RP2040::INOUT_CFG_PULL_DOWN;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Dio30::start( Cpl::Itc::PostApi& chassisMbox, uint64_t currentElapsedTimeUsec ) noexcept
{
    // Call the parent's start-up actions
    if ( Common_::start( currentElapsedTimeUsec ) )
    {
        // Start the low-level driver
        if ( !Driver::DIO::InOut::start( m_numInputs, m_driverInCfg, m_numOutputs, m_driverOutCfg ) )
        {
            m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::DRIVER_ERROR );
            m_error.logIt( getTypeName() );
            return false;
        }

        // Initialize input IO Registers
        for ( unsigned i=0; i < MAX_INPUT_CHANNELS; i++ )
        {
            // Set the initial IO Register values
            if ( m_ioRegisterPoints[i + INPUT_POINT_OFFSET] != nullptr )
            {
                m_ioRegisterPoints[i + INPUT_POINT_OFFSET]->updateFromSetter();
            }
        }

        // Initialize output IO Registers and Update the physical outputs
        for ( unsigned i=0, pinIdx=0; i < MAX_OUTPUT_CHANNELS; i++ )
        {
            // Set the initial IO Register values
            if ( m_ioRegisterPoints[i + OUTPUT_POINT_OFFSET] != nullptr )
            {
                m_ioRegisterPoints[i + OUTPUT_POINT_OFFSET]->updateFromSetter();
                
                // Set the initial output value (if there is one)
                Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_ioRegisterPoints[i + OUTPUT_POINT_OFFSET];
                bool bitValue;
                if ( pt->read( bitValue ) )
                {
                    Driver::DIO::InOut::setOutput( pinIdx, bitValue );
                }

                pinIdx++;
            }
        }

        // If I get here -->everything worked            
        return true;
    }

    // Start FAILED
    return false;
}



void Dio30::stop( Cpl::Itc::PostApi& chassisMbox ) noexcept
{
    // Stop the driver
    Driver::DIO::InOut::stop();

    // Call my parent's stop
    Common_::stop();
}

const char* Dio30::getTypeGuid() const noexcept
{
    return GUID_STRING;
}

const char* Dio30::getTypeName() const noexcept
{
    return TYPE_NAME;
}


///////////////////////////////////////////////////////////////////////////////
bool Dio30::scanInputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Sample Inputs
    for ( unsigned i=0, pinIdx=0; i < MAX_INPUT_CHANNELS; i++ )
    {
        if ( m_ioRegisterPoints[i + INPUT_POINT_OFFSET] != nullptr )
        {
            Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_ioRegisterPoints[i + INPUT_POINT_OFFSET];
            bool bitValue;
            if ( Driver::DIO::InOut::getInput( pinIdx++, bitValue ) )
            {
                pt->write( bitValue );
            }
            else
            {
                m_error = fullErr( Err_T::DRIVER_ERROR );
                m_error.logIt( getTypeName() );
                return false;
            }
        }
    }

    // Call parent class to manage the transfer between IO Registers and Virtual Points
    return Common_::scanInputs( currentElapsedTimeUsec );
}


bool Dio30::flushOutputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Call parent class to manage the transfer between IO Registers and Virtual Points
    bool result = Common_::flushOutputs( currentElapsedTimeUsec );

    // Update outputs
    if ( result )
    {
        for ( unsigned i=0, pinIdx=0; i < MAX_OUTPUT_CHANNELS; i++ )
        {
            if ( m_ioRegisterPoints[i + OUTPUT_POINT_OFFSET] != nullptr )
            {
                Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_ioRegisterPoints[i + OUTPUT_POINT_OFFSET];
                bool val;
                if ( pt->read( val ) )
                {
                    if ( !Driver::DIO::InOut::setOutput( pinIdx++, val ) )
                    {
                        m_error = fullErr( Err_T::DRIVER_ERROR );
                        m_error.logIt( getTypeName() );
                        return false;
                    }
                }
            }
        }
    }

    return result;
}

