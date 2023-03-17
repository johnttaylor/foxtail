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


#include "RHTemperature.h"
#include "RHTemperatureDriver.h"
#include "Cpl/System/Assert.h"

///
using namespace Fxt::Card::Sensor::I2C;

#define MAX_INPUT_CHANNELS          2
#define INPUT_POINT_OFFSET          0

#define MAX_OUTPUT_CHANNELS         1
#define OUTPUT_POINT_OFFSET         (MAX_INPUT_CHANNELS)

#define TOTAL_MAX_CHANNELS          (MAX_INPUT_CHANNELS+MAX_OUTPUT_CHANNELS)



#define INVALID_INDEX               MAX_INPUT_CHANNELS


///////////////////////////////////////////////////////////////////////////////
RHTemperature::RHTemperature( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                              Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                              Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                              Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                              Fxt::Point::DatabaseApi&           dbForPoints,
                              JsonVariant&                       cardObject,
                              Cpl::Itc::PostApi*                 cardMbox,
                              void*                              extraArgsNotUsed )
    : Fxt::Card::Common_( TOTAL_MAX_CHANNELS, generalAllocator, cardObject )
    , StartStopSync( *cardMbox )
    , m_driver( nullptr )
    , m_rhIndex( INVALID_INDEX )
    , m_tempIndex( INVALID_INDEX )
    , m_validSamples( false )
    , m_heaterEnable( false )
    , m_pendingHeaterUpdate( false )
{
    if ( m_error == Fxt::Type::Error::SUCCESS() )
    {
        // Get the driver instance
        m_driver = RHTemperatureDriver::get( m_slotNum );
        if ( m_driver == nullptr )
        {
            m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::DRIVER_ERROR );
            m_error.logIt();
        }
        else
        {
            parseConfiguration( generalAllocator, cardStatefulDataAllocator, haStatefulDataAllocator, pointFactoryDb, dbForPoints, cardObject );
        }
    }
}



///////////////////////////////////////////////////////////////////////////////
void RHTemperature::parseConfiguration( Cpl::Memory::ContiguousAllocator&  generalAllocator,
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
            size_t numInputs = inputs.size();
            if ( numInputs > MAX_INPUT_CHANNELS )
            {
                m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::TOO_MANY_INPUT_POINTS );
                m_error.logIt();
                return;
            }

            // Create Virtual Points
            for ( size_t idx=0; idx < numInputs; idx++ )
            {
                uint16_t   channelNum;
                JsonObject channelObj = inputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_virtualInputs,
                                       Fxt::Point::Float::GUID_STRING,
                                       false,
                                       channelObj,
                                       m_error,
                                       MAX_INPUT_CHANNELS,
                                       0,
                                       channelNum,
                                       generalAllocator,
                                       cardStatefulDataAllocator,
                                       dbForPoints );

                // Stop processing if/when an error occurred
                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }

                // Validate Channel number
                if ( channelNum > 2 )
                {
                    m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
                    m_error.logIt();
                    return;
                }
            }

            // Create IO Register Points
            for ( size_t idx=0; idx < numInputs; idx++ )
            {
                // Stop processing if/when an error occurred
                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }

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

                // Capture IO Point index for the RH Channel
                if ( channelNum == 1 )
                {
                    if ( m_rhIndex != INVALID_INDEX )
                    {
                        {
                            m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
                            m_error.logIt();
                            return;
                        }
                    }
                    m_rhIndex = idx;
                }

                // Capture IO Point index for the TEMP Channel
                if ( channelNum == 2 )
                {
                    if ( m_tempIndex != INVALID_INDEX )
                    {
                        {
                            m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
                            m_error.logIt();
                            return;
                        }
                    }
                    m_tempIndex = idx;
                }
            }
        }

        // OUTPUTS
        JsonArray outputs = cardObject["points"]["outputs"];
        if ( !outputs.isNull() )
        {
            // Validate supported number of signals
            size_t numOutputs = outputs.size();
            if ( numOutputs > MAX_OUTPUT_CHANNELS )
            {
                m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::TOO_MANY_OUTPUT_POINTS );
                m_error.logIt();
                return;
            }

            // Create Virtual Points
            for ( size_t idx=0; idx < numOutputs; idx++ )
            {
                uint16_t   channelNum;
                JsonObject channelObj = outputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_virtualOutputs,
                                       Fxt::Point::Bool::GUID_STRING,
                                       false,
                                       channelObj,
                                       m_error,
                                       MAX_OUTPUT_CHANNELS,
                                       OUTPUT_POINT_OFFSET,
                                       channelNum,
                                       generalAllocator,
                                       haStatefulDataAllocator,     // All Output Virtual Points are part of the HA Data set
                                       dbForPoints );

                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }

                // Validate Channel number
                if ( channelNum > 1 )
                {
                    m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
                    m_error.logIt();
                    return;
                }
            }

            // Create IO Register Points
            for ( size_t idx=0; idx < numOutputs; idx++ )
            {
                uint16_t   channelNum_notUsed;
                JsonObject channelObj = outputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_ioRegisterOutputs,
                                       Fxt::Point::Bool::GUID_STRING,
                                       true,
                                       channelObj,
                                       m_error,
                                       MAX_OUTPUT_CHANNELS,
                                       OUTPUT_POINT_OFFSET,
                                       channelNum_notUsed,
                                       generalAllocator,
                                       cardStatefulDataAllocator,
                                       dbForPoints );

                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
bool RHTemperature::start( uint64_t currentElapsedTimeUsec ) noexcept
{
    return issueStartRequest( currentElapsedTimeUsec );
}

void RHTemperature::stop() noexcept
{
    issueStopRequest();
}

// Note: This method executes in the 'driver thread'
void RHTemperature::request( StartMsg& msg )
{
    StartRequest::StartPayload& payload = msg.getPayload();
    payload.m_success                   = false;

    // Call the parent's start-up actions
    if ( Common_::start( payload.m_currentElapsedTimeUsec ) )
    {
        // Initialize inputs 
        for ( unsigned i=0; i < MAX_INPUT_CHANNELS; i++ )
        {
            if ( m_ioRegisterPoints[i + INPUT_POINT_OFFSET] != nullptr )
            {
                // Set the initial IO Register values
                m_ioRegisterPoints[i + INPUT_POINT_OFFSET]->updateFromSetter();
            }
        }

        // Initialize Outputs 
        for ( unsigned i=0; i < MAX_OUTPUT_CHANNELS; i++ )
        {
            if ( m_ioRegisterPoints[i + OUTPUT_POINT_OFFSET] != nullptr )
            {
                // Set the initial IO Register values
                m_ioRegisterPoints[i + OUTPUT_POINT_OFFSET]->updateFromSetter();
            }
        }

        // Start the underlying driver
        if ( m_driver->start() )
        {
            // Set the initial state for the heater output
            m_pendingHeaterUpdate = false;
            if ( m_ioRegisterPoints[OUTPUT_POINT_OFFSET] != nullptr )
            {
                Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_ioRegisterOutputs[OUTPUT_POINT_OFFSET];
                if ( pt->read( m_heaterEnable ) )
                {
                    m_pendingHeaterUpdate = true;
                }
            }

            // Start the background sampling (and update the Heater output)
            expired();

            // If I get here -->everything worked
            payload.m_success = true;
        }
    }

    msg.returnToSender();
}

// Note: This method executes in the 'driver thread'
void RHTemperature::request( StopMsg& msg )
{
    // Stop the underlying driver
    m_driver->stop();

    // Call my parent's stop
    Common_::stop();
}

const char* RHTemperature::getTypeGuid() const noexcept
{
    return GUID_STRING;
}

const char* RHTemperature::getTypeName() const noexcept
{
    return TYPE_NAME;
}


///////////////////////////////////////////////////////////////////////////////
// Note: This method executes in the 'Chassis thread'
bool RHTemperature::scanInputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Copy the background driver samples to the Input IO Registers
    for ( unsigned i=0, pinIdx=0; i < MAX_INPUT_CHANNELS; i++ )
    {
        if ( m_ioRegisterPoints[i + INPUT_POINT_OFFSET] != nullptr )
        {
            // Safely Read the latest sample value
            m_lock.lock();
            bool  validSample = m_validSamples;
            float value       = m_samples[i];
            m_lock.unlock();

            // Update the corresponding IO Register
            Fxt::Point::Float* pt = (Fxt::Point::Float*) m_ioRegisterPoints[i + INPUT_POINT_OFFSET];
            if ( validSample )
            {
                pt->write( value );
            }
            else
            {
                pt->setInvalid();
            }
        }
    }

    // Call parent class to manage the transfer between IO Registers and Virtual Points
    return Common_::scanInputs( currentElapsedTimeUsec );
}


bool RHTemperature::flushOutputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Call parent class to manage the transfer between IO Registers and Virtual Points
    bool result = Common_::flushOutputs( currentElapsedTimeUsec );

    // Update outputs
    if ( result )
    {
        // NOTE: At most there will be ONE output (aka the Heater enable)
        if ( m_ioRegisterPoints[OUTPUT_POINT_OFFSET] != nullptr )
        {
            Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_ioRegisterPoints[OUTPUT_POINT_OFFSET];
            bool val;
            if ( pt->read( val ) )
            {
                // Safely update my outputs flag to be physically set on the next polling cycle
                m_lock.lock();
                m_pendingHeaterUpdate = true;
                m_heaterEnable = true;
                m_lock.unlock();
            }
        }
    }

    return result;
}

