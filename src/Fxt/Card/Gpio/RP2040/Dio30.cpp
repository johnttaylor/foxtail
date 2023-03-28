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


///////////////////////////////////////////////////////////////////////////////
Dio30::Dio30( Cpl::Memory::ContiguousAllocator&  generalAllocator,
              Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
              Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
              Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
              Fxt::Point::DatabaseApi&           dbForPoints,
              JsonVariant&                       cardObject,
              Cpl::Dm::MailboxServer*            cardMboxNotUsed,
              void*                              extraArgsNotUsed )
    : Fxt::Card::Common_( MAX_INPUT_CHANNELS, MAX_OUTPUT_CHANNELS )
{
    memset( m_driverInCfg, 0, sizeof( m_driverInCfg ) );
    memset( m_driverOutCfg, 0, sizeof( m_driverOutCfg ) );

    if ( initialize( generalAllocator, cardObject ) )
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
    // Parse/Create Virtual & IO Register points
    if ( !parseInputOutputPoints( generalAllocator,
                                  cardStatefulDataAllocator,
                                  haStatefulDataAllocator,
                                  pointFactoryDb,
                                  dbForPoints,
                                  cardObject,
                                  false,
                                  0, 0 ) )
    {
        return;
    }

    // Validate the input/output types
    if ( !Fxt::Point::Api::validatePointTypes( m_inputIoRegisterPoints, m_numInputs, Fxt::Point::Bool::GUID_STRING ) ||
         !Fxt::Point::Api::validatePointTypes( m_outputIoRegisterPoints, m_numOutputs, Fxt::Point::Bool::GUID_STRING ) )
    {
        m_error = fullErr( Err_T::POINT_WRONG_TYPE );
        m_error.logIt( getTypeName() );
        return;
    }

    // Parse INPUT Pull resistor option
    JsonArray inputs = cardObject["points"]["inputs"];
    for ( unsigned idx=0; idx < m_numInputs; idx++ )
    {
        // Get the channel number
        JsonObject elem       = inputs[idx];
        uint16_t   channelNum = getChannelNumber( elem, 1, MAX_INPUT_CHANNELS );  // Note: '0' is invalid channel #
        if ( channelNum == 0 )
        {
            return;
        }

        // Parse Pull resistor option
        parseDriverConfig( elem, m_driverInCfg, idx, channelNum );
    }

    // Parse OUTPUT Pull resistor option
    JsonArray outputs = cardObject["points"]["outputs"];
    for ( unsigned idx=0; idx < m_numOutputs; idx++ )
    {
        // Get the channel number
        JsonObject elem       = outputs[idx];
        uint16_t   channelNum = getChannelNumber( elem, 1, MAX_OUTPUT_CHANNELS );  // Note: '0' is invalid channel #
        if ( channelNum == 0 )
        {
            return;
        }

        // Parse Pull resistor option
        parseDriverConfig( elem, m_driverOutCfg, idx, channelNum );
    }

    // Make sure that no IO pin has been 'doubled-up'
    for ( unsigned idx = 0; idx < m_numInputs; idx++ )
    {
        size_t pinId = m_driverInCfg[idx].pin;
        for ( unsigned idx=0; idx < m_numOutputs; idx++ )
        {
            if ( m_driverOutCfg[idx].pin == pinId )
            {
                m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
                m_error.logIt( "%s. duplicate pin=%u", getTypeName(), pinId );
                return;
            }
        }
    }
}

void Dio30::parseDriverConfig( JsonObject & channelObj, Driver::DIO::InOut::Config_T cfg[], size_t arrayIdx, uint16_t channelNum )
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
bool Dio30::start( Cpl::Itc::PostApi & chassisMbox, uint64_t currentElapsedTimeUsec ) noexcept
{
    // Call the parent's start-up actions (Note: sets the VPoint/IORegPoints initial values)
    if ( Common_::start( currentElapsedTimeUsec ) )
    {
        // Start the low-level driver
        if ( !Driver::DIO::InOut::start( (uint8_t) m_numInputs, m_driverInCfg, (uint8_t) m_numOutputs, m_driverOutCfg ) )
        {
            m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::DRIVER_ERROR );
            m_error.logIt( "%s. Start failure", getTypeName() );
            return false;
        }

        // Initialize the physical outputs
        for ( unsigned i=0; i < m_numOutputs; i++ )
        {
            // Set the initial output value (if there is one)
            Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_outputIoRegisterPoints[i];
            bool bitValue;
            if ( pt->read( bitValue ) )
            {
                Driver::DIO::InOut::setOutput( i, bitValue );
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
    for ( unsigned i=0; i < m_numInputs; i++ )
    {
        Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_inputIoRegisterPoints[i];
        bool bitValue;
        if ( Driver::DIO::InOut::getInput( i, bitValue ) )
        {
            pt->write( bitValue );
        }
        else
        {
            m_error = fullErr( Err_T::DRIVER_ERROR );
            m_error.logIt( "%s. ScanInputs", getTypeName() );
            return false;
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
        for ( unsigned i=0; i < m_numOutputs; i++ )
        {
            Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_outputIoRegisterPoints[i];
            bool val;
            if ( pt->read( val ) )
            {
                if ( !Driver::DIO::InOut::setOutput( i, val ) )
                {
                    m_error = fullErr( Err_T::DRIVER_ERROR );
                    m_error.logIt( "%s. FlushOutputs", getTypeName() );
                    return false;
                }
            }
        }
    }

    return result;
}

