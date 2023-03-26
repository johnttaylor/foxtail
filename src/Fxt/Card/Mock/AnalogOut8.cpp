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


#include "AnalogOut8.h"
#include "Cpl/System/Assert.h"
#include <stdint.h>

///
using namespace Fxt::Card::Mock;

#define MAX_OUTPUT_CHANNELS             8 
#define MAX_INPUT_CHANNELS              0

#define STARTING_OUTPUT_CHANNEL_NUM     1
#define ENDING_OUTPUT_CHANNEL_NUM       MAX_OUTPUT_CHANNELS


///////////////////////////////////////////////////////////////////////////////
AnalogOut8::AnalogOut8( Cpl::Memory::ContiguousAllocator&  generalAllocator,
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
void AnalogOut8::parseConfiguration( Cpl::Memory::ContiguousAllocator&  generalAllocator,
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
                                  0, 1 ) )
    {
        return;
    }

    // Validate the input types
    if ( !Fxt::Point::Api::validatePointTypes( m_outputIoRegisterPoints, m_numOutputs, Fxt::Point::Float::GUID_STRING ) )
    {
        return;
    }
}





///////////////////////////////////////////////////////////////////////////////
bool AnalogOut8::start( Cpl::Itc::PostApi& chassisMbox, uint64_t currentElapsedTimeUsec ) noexcept
{
    // Call the parent's start-up actions (Note: sets the VPoint/IORegPoints initial values)
    return Common_::start( currentElapsedTimeUsec );
}



void AnalogOut8::stop( Cpl::Itc::PostApi& chassisMbox ) noexcept
{
    Common_::stop();
}

bool AnalogOut8::scanInputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Do nothing since I have no inputs
    return true;
}

bool AnalogOut8::flushOutputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Need to wrap with a mutex since my output data is accessed from a different thread
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
    return Common_::flushOutputs( currentElapsedTimeUsec );
}

const char* AnalogOut8::getTypeGuid() const noexcept
{
    return GUID_STRING;
}

const char* AnalogOut8::getTypeName() const noexcept
{
    return TYPE_NAME;
}


///////////////////////////////////////////////////////////////////////////////
bool AnalogOut8::getOutputs( uint8_t channelNumber, float& dstValue, bool& dstIsValid )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    // Validate the range of channel number
    if ( channelNumber >= STARTING_OUTPUT_CHANNEL_NUM && channelNumber <= ENDING_OUTPUT_CHANNEL_NUM )
    {
        // Was the channel specified in the JSON syntax?
        if ( m_outputIoRegisterPoints[channelNumber - 1] != nullptr )
        {
            // Update the IO Register
            Fxt::Point::Float* pt = (Fxt::Point::Float*) m_outputIoRegisterPoints[channelNumber - 1];
            dstIsValid = pt->read( dstValue );
            return true;
        }
    }

    return false;
}
