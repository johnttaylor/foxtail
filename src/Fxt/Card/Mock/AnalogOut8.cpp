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

#define MAX_CHANNELS        8 


///////////////////////////////////////////////////////////////////////////////
AnalogOut8::AnalogOut8( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                        Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                        Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                        Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                        Fxt::Point::DatabaseApi&           dbForPoints,
                        JsonVariant&                       cardObject,
                        Cpl::Itc::PostApi*                 cardMboxNotUsed,
                        void*                              extraArgsNotUsed )
    : Fxt::Card::Common_( MAX_CHANNELS, generalAllocator, cardObject )
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
    // Parse channels
    if ( !cardObject["points"].isNull() )
    {
        // OUTPUTS
        JsonArray outputs = cardObject["points"]["outputs"];
        if ( !outputs.isNull() )
        {
            // Validate supported number of signals
            size_t numOutputs = outputs.size();
            if ( numOutputs > MAX_CHANNELS )
            {
                m_error = fullErr( Err_T::TOO_MANY_OUTPUT_POINTS );
                m_error.logIt( getTypeName() );
                return;
            }

            // Create Virtual Points
            for ( size_t idx=0; idx < numOutputs && m_error == Fxt::Type::Error::SUCCESS(); idx++ )
            {
                uint16_t   channelNum_notUsed;
                JsonObject channelObj = outputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_virtualInputs,
                                       Fxt::Point::Float::GUID_STRING,
                                       false,
                                       channelObj,
                                       m_error,
                                       MAX_CHANNELS,
                                       0,
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
            for ( size_t idx=0; idx < numOutputs && m_error == Fxt::Type::Error::SUCCESS(); idx++ )
            {
                uint16_t   channelNum_notUsed;
                JsonObject channelObj = outputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_ioRegisterInputs,
                                       Fxt::Point::Float::GUID_STRING,
                                       true,
                                       channelObj,
                                       m_error,
                                       MAX_CHANNELS,
                                       0,
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
bool AnalogOut8::start( Cpl::Itc::PostApi& chassisMbox, uint64_t currentElapsedTimeUsec ) noexcept
{
    // Call the parent's start-up actions
    if ( Common_::start( currentElapsedTimeUsec ) )
    {
        // Set the initial IO Register values
        for ( unsigned i=0; i < MAX_CHANNELS; i++ )
        {
            if ( m_ioRegisterPoints[i] != nullptr )
            {
                m_ioRegisterPoints[i]->updateFromSetter();
            }
        }

        // If I get here -->everything worked            
        return true;
    }

    // Start FAILED
    return false;
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
    if ( channelNumber > 0 && channelNumber <= MAX_CHANNELS )
    {
        // Was the channel specified in the JSON syntax?
        if ( m_ioRegisterPoints[channelNumber - 1] != nullptr )
        {
            // Update the IO Register
            Fxt::Point::Float* pt = (Fxt::Point::Float*) m_ioRegisterPoints[channelNumber - 1];
            dstIsValid = pt->read( dstValue );
            return true;
        }
    }

    return false;
}
