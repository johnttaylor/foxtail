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
#include "Cpl/System/Trace.h"

#define SECT_ "Fxt::Card::Sensor::I2C::RHTemperature"

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
                              Cpl::Dm::MailboxServer*            cardMbox,
                              void*                              extraArgsNotUsed )
    : Fxt::Card::Common_( TOTAL_MAX_CHANNELS, generalAllocator, cardObject )
    , StartStopAsync( *cardMbox )
    , Timer( *cardMbox )
    , m_driver( nullptr )
    , m_rhIndex( INVALID_INDEX )
    , m_tempIndex( INVALID_INDEX )
    , m_validSamples( false )
    , m_heaterEnable( false )
    , m_driverStarted( false )
    , m_forceHeaterUpdate( false )
    , m_lastHeaterEnable( false )
{
    if ( m_error == Fxt::Type::Error::SUCCESS() )
    {
        // Get the driver instance
        m_driver = RHTemperatureDriver::get( m_slotNum );
        if ( m_driver == nullptr )
        {
            m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::DRIVER_ERROR );
            m_error.logIt( getTypeName() );
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
    // Parse background sampling time
    m_delayMs = cardObject["driverInterval"] | 0;
    if ( m_delayMs < OPTION_FXT_CARD_SENSOR_I2C_MIN_DRIVER_SAMPLE_TIME_MS )
    {
        m_error = Fxt::Card::fullErr( m_delayMs == 0 ? Fxt::Card::Err_T::MISSING_REQUIRE_FIELD : Fxt::Card::Err_T::INVALID_FIELD );
        m_error.logIt( getTypeName() );
        return;
    }

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
                m_error.logIt( getTypeName() );
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
                    m_error.logIt( getTypeName() );
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
                                       Fxt::Point::Float::GUID_STRING,
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
                            m_error.logIt( getTypeName() );
                            return;
                        }
                    }
                    m_rhIndex = (uint16_t) idx;
                }

                // Capture IO Point index for the TEMP Channel
                if ( channelNum == 2 )
                {
                    if ( m_tempIndex != INVALID_INDEX )
                    {
                        {
                            m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
                            m_error.logIt( getTypeName() );
                            return;
                        }
                    }
                    m_tempIndex =  (uint16_t) idx;
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
                    m_error.logIt( getTypeName() );
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
// NO Guarantee on which thread this method executes in, but it is NOT the chassis thread
bool RHTemperature::start( Cpl::Itc::PostApi& chassisMbox, uint64_t currentElapsedTimeUsec ) noexcept
{
    // Because the chassis server has NOT been started at this point - I
    // can safely access all class members since there we are effectively 
    // 'single threaded' at this point

    // Call the parent's start-up actions (note: returns false if ALREADY started OR if there is card error)
    if ( !Common_::start( currentElapsedTimeUsec ) )
    {
        return false;
    }

    // Initialize inputs 
    m_validSamples = false;
    for ( unsigned i=0; i < MAX_INPUT_CHANNELS; i++ )
    {
        if ( m_ioRegisterPoints[i + INPUT_POINT_OFFSET] != nullptr )
        {
            // Set the initial IO Register values
            m_ioRegisterPoints[i + INPUT_POINT_OFFSET]->updateFromSetter();
        }
    }

    // Initialize Outputs (at most there is ONE)
    Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_ioRegisterPoints[OUTPUT_POINT_OFFSET];
    if ( pt != nullptr )
    {
        // Set the initial IO Register values
        pt->updateFromSetter();
    }

    // Force an initial update of the Heater. Note: if the IO Point is invalid -->the heater will be forced off
    m_forceHeaterUpdate = true;
    m_heaterEnable      = false;
    pt->read( m_heaterEnable );

    // Request to start the driver (which executes in the driver thread)
    // NOTE: The method only fails if there is pending ITC start transaction.
    //       If the driver fails to start, it is reported at run time via
    //       scan()/flush() methods.
    return issueAsyncStartRequest( chassisMbox, currentElapsedTimeUsec );
}

// Note: This method executes in the 'driver thread'
void RHTemperature::request( StartReqMsg& msg )
{
    StartRequest::StartPayload& payload = msg.getPayload();
    payload.m_success                   = false;

    // Start the underlying driver
    if ( m_driver->start() )
    {
        // Start the background sampling (and update the Heater output)
        expired();

        // If I get here -->everything worked
        payload.m_success = true;
    }

    // Complete the ITC transaction
    msg.returnToSender();
}

// Note: This method executes in the 'chassis thread'
void RHTemperature::response( StartRspMsg& msg )
{
    m_driverStarted = msg.getPayload().m_success;
    if ( !m_driverStarted )
    {
        m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::DRIVER_ERROR );
        m_error.logIt( getTypeName() );
    }

    startTransactionCompleted();
}

// NO Guarantee on which thread this method executes in, but it is NOT the chassis thread
void RHTemperature::stop( Cpl::Itc::PostApi& chassisMbox ) noexcept
{
    if ( m_started )
    {
        // Prevents the scan()/flush() methods from accessing the driver
        m_driverStarted = false;

        // Call my parent's stop
        Common_::stop();
        m_started = true;   // Need to ensure the card's state stays in the 'started' state until the driver actually is stopped.

        // Request the driver to stop (which executes in the driver thread)
        issueAsyncStopRequest( chassisMbox );
    }
}

// Note: This method executes in the 'driver thread'
void RHTemperature::request( StopReqMsg& msg )
{
    // Stop my timer
    Timer::stop();

    // Stop the underlying driver
    m_driver->stop();

    // Complete the ITC transaction
    msg.returnToSender();
}

// Note: This method executes in the 'chassis thread'
void RHTemperature::response( StopRspMsg& msg )
{
    m_started = false;

    stopTransactionCompleted();
}

/////////////////////////////////////////////
const char* RHTemperature::getTypeGuid() const noexcept
{
    return GUID_STRING;
}

const char* RHTemperature::getTypeName() const noexcept
{
    return TYPE_NAME;
}


///////////////////////////////////////////////////////////////////////////////
// NOTE: This method executes in the Driver Thread
void RHTemperature::expired() noexcept
{
    // Get output state
    m_lock.lock();
    bool heaterVal = m_heaterEnable;
    m_lock.unlock();

    // Update output
    if ( m_forceHeaterUpdate || heaterVal != m_lastHeaterEnable )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Updating heater enabled: %d", heaterVal) );
        m_lastHeaterEnable  = heaterVal;
        m_forceHeaterUpdate = false;
        m_driver->setHeaterState( heaterVal );
        
    }

    // Start the first sample
    Driver::RHTemp::Api::SamplingState_T state = m_driver->getSamplingState();
    if ( state == Driver::RHTemp::Api::eNOT_STARTED )
    {
        m_driver->startSample();
    }

    // Trap sampling errors
    else if ( state == Driver::RHTemp::Api::eERROR )
    {
        m_lock.lock();
        m_validSamples = false;
        m_lock.unlock();

        m_driver->startSample();
    }

    // Read sample and restart the sampling
    else if ( state == Driver::RHTemp::Api::eSAMPLE_READY )
    {
        float rh, temp;
        m_driver->getSample( rh, temp );

        m_lock.lock();
        m_samples[m_rhIndex]   = rh;
        m_samples[m_tempIndex] = temp;
        m_validSamples         = true;
        m_lock.unlock();

        m_driver->startSample();
    }

    // Restart my sampling timer
    Timer::start( m_delayMs );
}

// Note: This method executes in the 'Chassis thread'
bool RHTemperature::scanInputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Skip/wait till the driver has been started
    if ( m_driverStarted )
    {
        // Get local cache of the driver data (so I lock/unlock the mutex ONLY once per scan cycle)
        m_lock.lock();
        bool  validSample = m_validSamples;
        float samples[2];
        memcpy( samples, m_samples, sizeof( samples ) );
        m_lock.unlock();

        // Copy the background driver samples to the Input IO Registers
        for ( unsigned i=0; i < MAX_INPUT_CHANNELS; i++ )
        {
            if ( m_ioRegisterPoints[i + INPUT_POINT_OFFSET] != nullptr )
            {
                // Update the corresponding IO Register
                Fxt::Point::Float* pt = (Fxt::Point::Float*) m_ioRegisterPoints[i + INPUT_POINT_OFFSET];
                if ( validSample )
                {
                    pt->write( samples[i] );
                }
                else
                {
                    pt->setInvalid();
                }
            }
        }
    }

    // Call parent class to manage the transfer between IO Registers and Virtual Points
    bool result = Common_::scanInputs( currentElapsedTimeUsec );

    // Check for failures (include the async failures in starting the driver)
    if ( !result || m_error != Fxt::Type::Error::SUCCESS() )
    {
        return false;
    }
    return true;
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
            // Get current value (and force the 'off state' if the Point is invalid)
            Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_ioRegisterPoints[OUTPUT_POINT_OFFSET];
            bool val = false;
            pt->read( val );

            // Safely update my output flags to be physically set on the next polling cycle
            m_lock.lock();
            m_heaterEnable = val;
            m_lock.unlock();
        }
    }

    // Check for failures (include the async failures in starting the driver)
    if ( !result || m_error != Fxt::Type::Error::SUCCESS() )
    {
        return false;
    }
    return true;
}

