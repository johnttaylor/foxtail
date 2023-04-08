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


#define INVALID_INDEX               MAX_INPUT_CHANNELS

#define MAX_INPUT_CHANNELS          2
#define STARTING_INPUT_CHANNEL_NUM  1
#define ENDING_INPUT_CHANNEL_NUM    MAX_INPUT_CHANNELS

#define MAX_OUTPUT_CHANNELS         1
#define STARTING_OUTPUT_CHANNEL_NUM 1
#define ENDING_OUTPUT_CHANNEL_NUM   MAX_OUTPUT_CHANNELS


///////////////////////////////////////////////////////////////////////////////
RHTemperature::RHTemperature( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                              Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                              Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                              Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                              Fxt::Point::DatabaseApi&           dbForPoints,
                              JsonVariant&                       cardObject,
                              Cpl::Dm::MailboxServer*            cardMbox,
                              void*                              extraArgsNotUsed )
    : Fxt::Card::Common_( MAX_INPUT_CHANNELS, MAX_OUTPUT_CHANNELS )
    , StartStopAsync( *cardMbox )
    , IoFlushAsync( *cardMbox )
    , IoScanAsync( *cardMbox )
    , Timer( *cardMbox )
    , m_driver( nullptr )
    , m_rhIndex( INVALID_INDEX )
    , m_tempIndex( INVALID_INDEX )
    , m_drvForceHeaterUpdate( false )
    , m_drvLastHeaterEnable( false )
{
    if ( initialize( generalAllocator, cardObject ) )
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
    // Parse/Create Virtual & IO Register points
    if ( !parseInputOutputPoints( generalAllocator,
                                  cardStatefulDataAllocator,
                                  haStatefulDataAllocator,
                                  pointFactoryDb,
                                  dbForPoints,
                                  cardObject,
                                  false,
                                  1, 0 ) )
    {
        return;
    }

    // Validate the input/output types
    if ( !Fxt::Point::Api::validatePointTypes( m_inputIoRegisterPoints, m_numInputs, Fxt::Point::Float::GUID_STRING ) ||
         !Fxt::Point::Api::validatePointTypes( m_outputIoRegisterPoints, m_numOutputs, Fxt::Point::Bool::GUID_STRING ) )
    {
        m_error = fullErr( Err_T::POINT_WRONG_TYPE );
        m_error.logIt( getTypeGuid() );
        return;
    }

    // Parse background sampling time
    m_delayMs = cardObject["driverInterval"] | 0;
    if ( m_delayMs < OPTION_FXT_CARD_SENSOR_I2C_MIN_DRIVER_SAMPLE_TIME_MS )
    {
        m_error = Fxt::Card::fullErr( m_delayMs == 0 ? Fxt::Card::Err_T::MISSING_REQUIRE_FIELD : Fxt::Card::Err_T::INVALID_FIELD );
        m_error.logIt( getTypeName() );
        return;
    }

    // INPUTS: Map channel number to RH/Temp
    JsonArray inputs = cardObject["points"]["inputs"];
    for ( size_t idx=0; idx < m_numInputs; idx++ )
    {
        JsonObject elem       = inputs[idx];
        uint16_t   channelNum = getChannelNumber( elem, STARTING_INPUT_CHANNEL_NUM, ENDING_INPUT_CHANNEL_NUM );

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
        else if ( channelNum == 2 )
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

        // Bad channel Number
        else
        {
            m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
            m_error.logIt( getTypeName() );
            return;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// Executes in a Client thread (which will NEVER be the chassis thread)
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

    // Housekeeping
    IoFlushAsync::start( chassisMbox );
    IoScanAsync::start( chassisMbox );

    // Update the physical outputs (aka the heater)
    Fxt::Point::Bool* pt           = (Fxt::Point::Bool*) m_outputIoRegisterPoints[0];
    m_drvCommandedValues.values[0] = false;   // Note: It is okay access the 'driver' data since the driver is NOT currently running
    pt->read( m_drvCommandedValues.values[0] );

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
        m_drvForceHeaterUpdate = true;
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
    if ( msg.getPayload().m_success == false )
    {
        m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::DRIVER_ERROR );
        m_error.logIt( getTypeName() );
    }

    startTransactionCompleted();
}

///////////////////////////////////////////////////////////////////////////////
// Executes in a Client thread (which will NEVER be the chassis thread)
void RHTemperature::stop( Cpl::Itc::PostApi& chassisMbox ) noexcept
{
    if ( m_started )
    {
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

// Note: This method executes in the chassis thread
void RHTemperature::response( StopRspMsg& msg )
{
    // Call my parent's stop
    Common_::stop();

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
    // Update output
    bool heaterEnabled = m_drvCommandedValues.values[0];
    if ( m_drvForceHeaterUpdate || heaterEnabled != m_drvLastHeaterEnable )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Updating heater enabled: %d", heaterEnabled) );
        m_drvLastHeaterEnable  = heaterEnabled;
        m_drvForceHeaterUpdate = false;
        m_driver->setHeaterState( heaterEnabled );
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
        m_driver->startSample();
    }

    // Read sample and restart the sampling
    else if ( state == Driver::RHTemp::Api::eSAMPLE_READY )
    {
        float rh, temp;
        if ( m_driver->getSample( rh, temp ) == Driver::RHTemp::Api::eSAMPLE_READY )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("RHTemperature: driver input: rh=%g, temp=%g", rh, temp) );
            m_drvReceivedValues.values[m_rhIndex]     = rh;
            m_drvReceivedValues.values[m_tempIndex]   = temp;
            m_drvReceivedValues.isValid[m_rhIndex]    = true;
            m_drvReceivedValues.isValid[m_tempIndex]  = true;
        }
        else
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("RHTemperature: driver input: INVALID") );
            m_drvReceivedValues.isValid[m_rhIndex]    = false;
            m_drvReceivedValues.isValid[m_tempIndex]  = false;
        }

        // Transfer the data to the card/chassis thread
        issueAsyncIoScanRequest();

        // Start the next sample
        m_driver->startSample();
    }

    // Restart my sampling timer
    Timer::start( m_delayMs );
}

// NOTE: executes in the driver thread
void RHTemperature::extractOutputsTransferBuffer() noexcept
{
    memcpy( &m_drvCommandedValues, &m_transferOutputsBuffer, sizeof( m_drvCommandedValues ) );
}

// NOTE: executes in the driver thread
void RHTemperature::populateInputsTransferBuffer() noexcept
{
    memcpy( &m_transferInputsBuffer, &m_drvReceivedValues, sizeof( m_transferInputsBuffer) );
}

///////////////////////////////////////////////////////////////////////////////

// Note: This method executes in the 'Chassis thread'
bool RHTemperature::scanInputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Call parent class to manage the transfer between IO Registers and Virtual Points
    bool result = Common_::scanInputs( currentElapsedTimeUsec );

    // Check for failures (including the async failures in starting the driver)
    if ( !result || m_error != Fxt::Type::Error::SUCCESS() )
    {
        return false;
    }
    return true;
}

/// See Fxt::Card::IoFlushAsync
void RHTemperature::extractInputsTransferBuffer() noexcept
{
    // Copy the transferred data (from the data) to the Input IO Registers
    for ( unsigned i=0; i < MAX_INPUT_CHANNELS; i++ )
    {
        if ( m_inputIoRegisterPoints[i] != nullptr )
        {
            // Update the corresponding IO Register
            Fxt::Point::Float* pt         = (Fxt::Point::Float*) m_inputIoRegisterPoints[i];
            bool               validInput = m_transferInputsBuffer.isValid[i];
            if ( validInput )
            {
                pt->write( m_transferInputsBuffer.values[i] );
            }
            else
            {
                pt->setInvalid();
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// NOTE: executes in the Chassis thread
bool RHTemperature::flushOutputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Call parent class to manage the transfer between IO Registers and Virtual Points
    bool result = Common_::flushOutputs( currentElapsedTimeUsec );

    // Update outputs
    if ( result )
    {
        // NOTE: At most there will be ONE output (aka the Heater enable)
        if ( m_outputIoRegisterPoints[0] != nullptr )
        {
            // Initiate the transfer of the data to the driver thread
            issueAsyncIoFlushRequest();
        }
    }

    // Check for failures (including the async failures in starting the driver)
    if ( !result || m_error != Fxt::Type::Error::SUCCESS() )
    {
        return false;
    }
    return true;
}

// NOTE: executes in the Chassis thread
void RHTemperature::popuplateOutputsTransferBuffer() noexcept
{
    // Get the current value (and force the 'off state' if the Point is invalid)
    Fxt::Point::Bool* pt              = (Fxt::Point::Bool*) m_outputIoRegisterPoints[0];
    m_transferOutputsBuffer.values[0] = false;
    pt->read( m_transferOutputsBuffer.values[0] );
}

