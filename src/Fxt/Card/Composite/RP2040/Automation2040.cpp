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


#define MAX_INPUT_CHANNELS          Automation2040::MAX_INPUT_CHANNELS
#define STARTING_INPUT_CHANNEL_NUM  1
#define ENDING_INPUT_CHANNEL_NUM    31

#define MAX_OUTPUT_CHANNELS         Automation2040::MAX_OUTPUT_CHANNELS
#define STARTING_OUTPUT_CHANNEL_NUM 1
#define ENDING_OUTPUT_CHANNEL_NUM   31


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
    : Fxt::Card::Common_( MAX_INPUT_CHANNELS, MAX_OUTPUT_CHANNELS, generalAllocator, cardObject )
{
    if ( m_error == Fxt::Type::Error::SUCCESS() )
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
            m_numInputs = (uint16_t) nInputs;

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
                                       STARTING_INPUT_CHANNEL_NUM,
                                       ENDING_INPUT_CHANNEL_NUM,
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
                Fxt::Point::Api* pt   = createPointForChannel( pointFactoryDb,
                                                               m_ioRegisterInputs,
                                                               Fxt::Point::Bool::GUID_STRING,
                                                               true,
                                                               channelObj,
                                                               m_error,
                                                               STARTING_INPUT_CHANNEL_NUM,
                                                               ENDING_INPUT_CHANNEL_NUM,
                                                               channelNum,
                                                               generalAllocator,
                                                               cardStatefulDataAllocator,
                                                               dbForPoints );

                // Stop processing if/when an error occurred
                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }

                // Cache the IO Register PT.  
                m_inputIoRegisterPoints[idx] = pt;

                // Build channel to driver mapping (also validates the channel number)
                if ( !updateInputMap( idx, channelNum ) )
                {
                    m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
                    m_error.logIt( "Card: %s, in ch=%u", getTypeName(), channelNum );
                    return;
                }
            }

            // Check for duplicate Channels
            for ( size_t idx=0; idx < m_numInputs; idx++ )
            {
                int driverId  = m_inputMap[idx].inputId;
                for ( size_t j=idx + 1; j < m_numInputs; j++ )
                {
                    if ( m_inputMap[idx].inputId == driverId )
                    {
                        m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
                        m_error.logIt( "Card: %s, Inputs. Duplicate. idx %u == idx %u ", getTypeName(), idx, j );
                        return;
                    }
                }
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
            m_numOutputs = (uint16_t) nOutputs;

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
                                       STARTING_OUTPUT_CHANNEL_NUM,
                                       ENDING_OUTPUT_CHANNEL_NUM,
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
                Fxt::Point::Api* pt   = createPointForChannel( pointFactoryDb,
                                                               m_ioRegisterOutputs,
                                                               Fxt::Point::Bool::GUID_STRING,
                                                               true,
                                                               channelObj,
                                                               m_error,
                                                               STARTING_OUTPUT_CHANNEL_NUM,
                                                               ENDING_OUTPUT_CHANNEL_NUM,
                                                               channelNum,
                                                               generalAllocator,
                                                               cardStatefulDataAllocator,
                                                               dbForPoints );

                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }

                // Cache the IO Register PT.  
                m_outputIoRegisterPoints[idx] = pt;

                // Build channel to driver mapping (also validates the channel number)
                if ( !updateOutputMap( idx, channelNum ) )
                {
                    m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
                    m_error.logIt( "Card: %s, out ch=%u", getTypeName(), channelNum );
                    return;
                }

            }

            // Check for duplicate Channels
            for ( size_t idx=0; idx < m_numOutputs; idx++ )
            {
                int driverId  = m_outputMap[idx].outputId;
                for ( size_t j=idx + 1; j < nOutputs; j++ )
                {
                    if ( m_outputMap[idx].outputId == driverId )
                    {
                        m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
                        m_error.logIt( "Card: %s, Outputs. Duplicate. idx %u == idx %u ", getTypeName(), idx, j );
                        return;
                    }
                }
            }
        }
    }
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
    case 31: // Intentional fall through
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
    case 31: // Intentional fall through
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
    // Call the parent's start-up actions
    if ( Common_::start( currentElapsedTimeUsec ) )
    {
        // Start the low-level driver
        if ( !Driver::Automation2040::Api::start() )
        {
            m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::DRIVER_ERROR );
            m_error.logIt( getTypeName() );
            return false;
        }

        // Initialize input IO Registers
        for ( unsigned i=0; i < m_numInputs; i++ )
        {
            m_inputIoRegisterPoints[i]->updateFromSetter();
        }

        // Initialize output IO Registers and Update the physical outputs
        for ( unsigned i=0; i < m_numOutputs; i++ )
        {
            // Set the initial IO Register values
            m_outputIoRegisterPoints[i]->updateFromSetter();

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
