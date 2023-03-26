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


#include "AnalogIn8.h"
#include "Cpl/System/Assert.h"
#include <stdint.h>

///
using namespace Fxt::Card::Mock;

#define MAX_INPUT_CHANNELS              8 
#define MAX_OUTPUT_CHANNELS             0

#define STARTING_INPUT_CHANNEL_NUM      1
#define ENDING_INPUT_CHANNEL_NUM        MAX_INPUT_CHANNELS


///////////////////////////////////////////////////////////////////////////////
AnalogIn8::AnalogIn8( Cpl::Memory::ContiguousAllocator&  generalAllocator,
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
        parseConfiguration( generalAllocator, cardStatefulDataAllocator, haStatefulDataAllocator, pointFactoryDb, dbForPoints, cardObject );
    }
}


///////////////////////////////////////////////////////////////////////////////
void AnalogIn8::parseConfiguration( Cpl::Memory::ContiguousAllocator&  generalAllocator,
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
                                  true,
                                  1, 0 ) )
    {
        return;
    }

    // Validate the input types
    if ( !Fxt::Point::Api::validatePointTypes( m_inputIoRegisterPoints, m_numInputs, Fxt::Point::Float::GUID_STRING )  )
    {
        return;
    }
}





///////////////////////////////////////////////////////////////////////////////
bool AnalogIn8::start( Cpl::Itc::PostApi& chassisMbox, uint64_t currentElapsedTimeUsec ) noexcept
{
    // Call the parent's start-up actions (Note: sets the VPoint/IORegPoints initial values)
    return Common_::start( currentElapsedTimeUsec );
}



void AnalogIn8::stop( Cpl::Itc::PostApi& chassisMbox ) noexcept
{
    Common_::stop();
}

bool AnalogIn8::scanInputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Need to wrap with a mutex since my input data is coming from a different thread
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
    return Common_::scanInputs( currentElapsedTimeUsec );
}

bool AnalogIn8::flushOutputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Do nothing since I have no outputs
    return true;
}

const char* AnalogIn8::getTypeGuid() const noexcept
{
    return GUID_STRING;
}

const char* AnalogIn8::getTypeName() const noexcept
{
    return TYPE_NAME;
}


///////////////////////////////////////////////////////////////////////////////
void AnalogIn8::setInput( uint8_t channelNumber, float newValue )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    // Validate the range of channel number
    if ( channelNumber >= STARTING_INPUT_CHANNEL_NUM && channelNumber <= ENDING_INPUT_CHANNEL_NUM )
    {
        // Was the channel specified in the JSON syntax?
        if ( m_inputIoRegisterPoints[channelNumber - 1] != nullptr )
        {
            // Update the IO Register
            Fxt::Point::Float* pt = (Fxt::Point::Float*) m_inputIoRegisterPoints[channelNumber - 1];
            pt->write( newValue );
        }
    }
}

void AnalogIn8::setInvalid( uint8_t channelNumber )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    // Validate the range of channel number
    if ( channelNumber >= STARTING_INPUT_CHANNEL_NUM && channelNumber <= ENDING_INPUT_CHANNEL_NUM )
    {
        // Was the channel specified in the JSON syntax?
        if ( m_inputIoRegisterPoints[channelNumber - 1] != nullptr )
        {
            // Update the IO Register
            Fxt::Point::Api* pt =  m_inputIoRegisterPoints[channelNumber - 1];
            pt->setInvalid();
        }
    }
}

bool AnalogIn8::getInput( uint8_t channelNumber, float& dstValue, bool& dstIsValid )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    // Validate the range of channel number
    if ( channelNumber >= STARTING_INPUT_CHANNEL_NUM && channelNumber <= ENDING_INPUT_CHANNEL_NUM )
    {
        // Was the channel specified in the JSON syntax?
        if ( m_inputIoRegisterPoints[channelNumber - 1] != nullptr )
        {
            // Update the IO Register
            Fxt::Point::Float* pt = (Fxt::Point::Float*) m_inputIoRegisterPoints[channelNumber - 1];
            dstIsValid = pt->read( dstValue );
            return true;
        }
    }

    return false;
}
