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


#include "Automation2040.h"
#include "Driver/Automation2040/Api.h"
#include "Cpl/System/Assert.h"

///
using namespace Fxt::Card::Composite::RP2040;


#define MAX_INPUT_CHANNELS              Automation2040::MAX_INPUT_CHANNELS
#define STARTING_INPUT_CHANNEL_NUM      1
#define ENDING_INPUT_CHANNEL_NUM        31

#define MAX_OUTPUT_CHANNELS             Automation2040::MAX_OUTPUT_CHANNELS
#define STARTING_OUTPUT_CHANNEL_NUM     1
#define ENDING_OUTPUT_CHANNEL_NUM       31

static const char* inputGuidFromChannelNum( unsigned channelNum );

static void readDriverDigitalInput( Fxt::Point::Api* dstPt, int inIdentifier );
static void readDriverAnalogInput( Fxt::Point::Api* dstPt, int inIdentifier );
static void readDriverButtonInput( Fxt::Point::Api* dstPt, int inIdentifier );
static void readDriverOnboareTempInput( Fxt::Point::Api* dstPt, int inIdentifierNotUsed );
static void writeDriverDigitalOutput( int outIdentifier, bool outValue );
static void writeDriverRelayOutput( int outIdentifier, bool outValue );
static void writeDriverButtonLEDOutput( int outIdentifier, bool outValue );
static void writeDriverConnectedLEDOutput( int outIdentifierNotUsed, bool outValue );


///////////////////////////////////////////////////////////////////////////////
Automation2040::Automation2040( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                                Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                                Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                                Fxt::Point::DatabaseApi&           dbForPoints,
                                JsonVariant&                       cardObject,
                                Cpl::Dm::MailboxServer*            cardMboxNotUsed,
                                void*                              extraArgsNotUsed )
    : Fxt::Card::Common_( MAX_INPUT_CHANNELS, MAX_OUTPUT_CHANNELS )
{
    if ( initialize( generalAllocator, cardObject ) )
    {
        memset( m_inputMap, 0, sizeof( m_inputMap ) );
        memset( m_outputMap, 0, sizeof( m_outputMap ) );
        parseConfiguration( generalAllocator, cardStatefulDataAllocator, haStatefulDataAllocator, pointFactoryDb, dbForPoints, cardObject );
    }
}



///////////////////////////////////////////////////////////////////////////////
void Automation2040::parseConfiguration( Cpl::Memory::ContiguousAllocator&  generalAllocator,
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

    // INPUTS: Build channel to driver mapping (also validates the channel number)
    JsonArray inputs = cardObject["points"]["inputs"];
    for ( unsigned idx=0; idx < m_numInputs; idx++ )
    {
        JsonObject elem       = inputs[idx];
        uint16_t   channelNum = getChannelNumber( elem, STARTING_INPUT_CHANNEL_NUM, ENDING_INPUT_CHANNEL_NUM );  // Note: '0' is invalid channel #
        if ( !updateInputMap( idx, channelNum ) )
        {
            m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
            m_error.logIt( "Card: %s, in ch=%u", getTypeName(), channelNum );
            return;
        }

        // Validate input type
        const char* guid = inputGuidFromChannelNum( channelNum );
        if ( !Fxt::Point::Api::validatePointTypes( m_inputIoRegisterPoints+idx, 1, guid ) )
        {
            m_error = fullErr( Err_T::POINT_WRONG_TYPE );
            m_error.logIt( getTypeName() );
            return;
        }
    }

    // INPUTS: Check for duplicate Channels
    for ( unsigned idx=0; idx < m_numInputs; idx++ )
    {
        int driverId  = m_inputMap[idx].inputId;
        for ( size_t j=idx + 1; j < m_numInputs; j++ )
        {
            if ( m_inputMap[j].inputId == driverId )
            {
                m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
                m_error.logIt( "Card: %s, Inputs. Duplicate. idx %u == idx %u ", getTypeName(), idx, j );
                return;
            }
        }
    }

    // OUTPUS: Build channel to driver mapping (also validates the channel number)
    JsonArray outputs = cardObject["points"]["outputs"];
    for ( unsigned idx=0; idx < m_numOutputs; idx++ )
    {
        JsonObject elem       = outputs[idx];
        uint16_t   channelNum = getChannelNumber( elem, STARTING_OUTPUT_CHANNEL_NUM, ENDING_OUTPUT_CHANNEL_NUM );  // Note: '0' is invalid channel #
        if ( !updateOutputMap( idx, channelNum ) )
        {
            m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
            m_error.logIt( "Card: %s, out ch=%u", getTypeName(), channelNum );
            return;
        }

        // Validate the output types
        if ( !Fxt::Point::Api::validatePointTypes( m_outputIoRegisterPoints+idx, 1, Fxt::Point::Bool::GUID_STRING ) )
        {
            m_error = fullErr( Err_T::POINT_WRONG_TYPE );
            m_error.logIt( getTypeName() );
            return;
        }
    }

    // OUTPUTS: Check for duplicate Channels
    for ( unsigned idx=0; idx < m_numOutputs; idx++ )
    {
        int driverId  = m_outputMap[idx].outputId;
        for ( size_t j=idx + 1; j < m_numOutputs; j++ )
        {
            if ( m_outputMap[j].outputId == driverId )
            {
                m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
                m_error.logIt( "Card: %s, Outputs. Duplicate. idx %u == idx %u ", getTypeName(), idx, j );
                return;
            }
        }
    }
}


static const char* inputGuidFromChannelNum( unsigned channelNum )
{
    if ( (channelNum >= 11 && channelNum <= 13) ||
         channelNum == 31 )
    {
        return  Fxt::Point::Float::GUID_STRING;
    }

    return Fxt::Point::Bool::GUID_STRING;
}

bool Automation2040::updateInputMap( size_t ioRegisterIdx, uint16_t channelNum )
{
    switch ( channelNum )
    {
        // Digital Inputs
    case 1: // Intentional fall through
    case 2:
    case 3:
    case 4:
        m_inputMap[ioRegisterIdx].func    = readDriverDigitalInput;
        m_inputMap[ioRegisterIdx].inputId = Driver::Automation2040::Api::eDINPUT_1 + channelNum - 1;
        break;

        // Analog Inputs
    case 11: // Intentional fall through
    case 12:
    case 13:
        m_inputMap[ioRegisterIdx].func    = readDriverAnalogInput;
        m_inputMap[ioRegisterIdx].inputId = Driver::Automation2040::Api::eAINPUT_1 + channelNum - 11;
        break;

        // User buttons
    case 21: // Intentional fall through
    case 22:
        m_inputMap[ioRegisterIdx].func    = readDriverButtonInput;
        m_inputMap[ioRegisterIdx].inputId = Driver::Automation2040::Api::eBOARD_BUTTON_A + channelNum - 21;
        break;

        // On-board temperature
    case 31: 
        m_inputMap[ioRegisterIdx].func    = readDriverOnboareTempInput;
        m_inputMap[ioRegisterIdx].inputId = 0; // Not used
        break;

    default:
        return false;
    }

    return true;
}

bool Automation2040::updateOutputMap( size_t ioRegisterIdx, uint16_t channelNum )
{
    switch ( channelNum )
    {
        // Digital Output
    case 1: // Intentional fall through
    case 2:
    case 3:
        m_outputMap[ioRegisterIdx].func     = writeDriverDigitalOutput;
        m_outputMap[ioRegisterIdx].outputId = Driver::Automation2040::Api::eDOUTPUT_1 + channelNum - 1;
        break;

        // Relay Outputs
    case 11: // Intentional fall through
    case 12:
    case 13:
        m_outputMap[ioRegisterIdx].func     = writeDriverRelayOutput;
        m_outputMap[ioRegisterIdx].outputId = Driver::Automation2040::Api::eRELAY_1 + channelNum - 11;
        break;

        // User button LEDs
    case 21: // Intentional fall through
    case 22:
        m_outputMap[ioRegisterIdx].func     = writeDriverButtonLEDOutput;
        m_outputMap[ioRegisterIdx].outputId = Driver::Automation2040::Api::eBOARD_BUTTON_A + channelNum - 21;
        break;

        // Connected LED
    case 31: 
        m_outputMap[ioRegisterIdx].func     = writeDriverConnectedLEDOutput;
        m_outputMap[ioRegisterIdx].outputId = 0; // Not used
        break;

    default:
        return false;
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////
bool Automation2040::start( Cpl::Itc::PostApi & chassisMbox, uint64_t currentElapsedTimeUsec ) noexcept
{
    // Call the parent's start-up actions (Note: sets the VPoint/IORegPoints initial values)
    if ( Common_::start( currentElapsedTimeUsec ) )
    {
        // Start the low-level driver
        if ( !Driver::Automation2040::Api::start() )
        {
            m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::DRIVER_ERROR );
            m_error.logIt( getTypeName() );
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
                (m_outputMap[i].func)(m_outputMap[i].outputId, bitValue);
            }
        }

        // If I get here -->everything worked            
        return true;
    }

    // Start FAILED
    return false;
}



void Automation2040::stop( Cpl::Itc::PostApi & chassisMbox ) noexcept
{
    // Stop the driver
    Driver::Automation2040::Api::stop();

    // Call my parent's stop
    Common_::stop();
}

const char* Automation2040::getTypeGuid() const noexcept
{
    return GUID_STRING;
}

const char* Automation2040::getTypeName() const noexcept
{
    return TYPE_NAME;
}



///////////////////////////////////////////////////////////////////////////////
bool Automation2040::scanInputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Sample Inputs
    for ( unsigned i=0; i < m_numInputs; i++ )
    {
        if ( m_inputIoRegisterPoints[i] != nullptr )
        {
            (m_inputMap[i].func)(m_inputIoRegisterPoints[i], m_inputMap[i].inputId);
        }
    }

    // Call parent class to manage the transfer between IO Registers and Virtual Points
    return Common_::scanInputs( currentElapsedTimeUsec );
}


bool Automation2040::flushOutputs( uint64_t currentElapsedTimeUsec ) noexcept
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
                (m_outputMap[i].func)(m_outputMap[i].outputId, val);
            }
        }
    }

    return result;
}

void readDriverDigitalInput( Fxt::Point::Api * dstPt, int inIdentifier )
{
    Fxt::Point::Bool* pt = (Fxt::Point::Bool*) dstPt;
    pt->write( Driver::Automation2040::Api::getInputState( (Driver::Automation2040::Api::DInputId_T) inIdentifier ) );
}

void readDriverAnalogInput( Fxt::Point::Api * dstPt, int inIdentifier )
{
    Fxt::Point::Float* pt = (Fxt::Point::Float*) dstPt;
    pt->write( Driver::Automation2040::Api::getAnalogValue( (Driver::Automation2040::Api::AInputId_T) inIdentifier ) );
}

void readDriverButtonInput( Fxt::Point::Api * dstPt, int inIdentifier )
{
    Fxt::Point::Bool* pt = (Fxt::Point::Bool*) dstPt;
    pt->write( Driver::Automation2040::Api::userButtonPressed( (Driver::Automation2040::Api::ButtonId_T) inIdentifier ) );
}

void readDriverOnboareTempInput( Fxt::Point::Api* dstPt, int inIdentifierNotUsed )
{
    Fxt::Point::Float* pt = (Fxt::Point::Float*) dstPt;
    pt->write( Driver::Automation2040::Api::getBoardTemperature() );
}

void writeDriverDigitalOutput( int outIdentifier, bool outValue )
{
    Driver::Automation2040::Api::setOutputState( (Driver::Automation2040::Api::DOutputId_T) outIdentifier, outValue );
}

void writeDriverRelayOutput( int outIdentifier, bool outValue )
{
    Driver::Automation2040::Api::setRelayState( (Driver::Automation2040::Api::RelayId_T) outIdentifier, outValue );
}

void writeDriverButtonLEDOutput( int outIdentifier, bool outValue )
{
    Driver::Automation2040::Api::setButtonLED( (Driver::Automation2040::Api::ButtonId_T) outIdentifier, outValue );
}

void writeDriverConnectedLEDOutput( int outIdentifierNotUsed, bool outValue )
{
    Driver::Automation2040::Api::setConnectedLED( outValue );
}
