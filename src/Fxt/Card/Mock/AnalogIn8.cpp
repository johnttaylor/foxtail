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
#include <new>

///
using namespace Fxt::Card::Mock;

#define MAX_CHANNELS        8 


///////////////////////////////////////////////////////////////////////////////
AnalogIn8::AnalogIn8( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                      Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                      Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                      Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                      Fxt::Point::DatabaseApi&           dbForPoints,
                      JsonVariant&                       cardObject )
    : Fxt::Card::Common_( MAX_CHANNELS, generalAllocator, cardObject )
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
    // Parse channels
    if ( !cardObject["points"].isNull() )
    {
        // INPUTS
        JsonArray inputs = cardObject["points"]["inputs"];
        if ( !inputs.isNull() )
        {
            // Validate supported number of signals
            size_t numInputs = inputs.size();
            if ( numInputs > MAX_CHANNELS )
            {
                m_error = fullErr( Err_T::TOO_MANY_INPUT_POINTS );
                m_error.logIt();
                return;
            }

            // Create Virtual Points
            for ( size_t idx=0; idx < numInputs && m_error == Fxt::Type::Error::SUCCESS(); idx++ )
            {
                uint16_t   channelNum_notUsed;
                JsonObject channelObj = inputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_virtualInputs,
                                       false,
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

            // Create IO Register Points
            for ( size_t idx=0; idx < numInputs && m_error == Fxt::Type::Error::SUCCESS(); idx++ )
            {
                uint16_t   channelNum_notUsed;
                JsonObject channelObj = inputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_ioRegisterInputs,
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
bool AnalogIn8::start( uint64_t currentElapsedTimeUsec ) noexcept
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



void AnalogIn8::stop() noexcept
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
    if ( channelNumber > 0 && channelNumber <= MAX_CHANNELS )
    {
        // Was the channel specified in the JSON syntax?
        if ( m_ioRegisterPoints[channelNumber - 1] != nullptr )
        {
            // Update the IO Register
            Fxt::Point::Float* pt = (Fxt::Point::Float*) m_ioRegisterPoints[channelNumber - 1];
            pt->write( newValue );
        }
    }
}

void AnalogIn8::setInvalid( uint8_t channelNumber )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    // Validate the range of channel number
    if ( channelNumber > 0 && channelNumber <= MAX_CHANNELS )
    {
        // Was the channel specified in the JSON syntax?
        if ( m_ioRegisterPoints[channelNumber - 1] != nullptr )
        {
            // Update the IO Register
            Fxt::Point::Api* pt =  m_ioRegisterPoints[channelNumber - 1];
            pt->setInvalid();
        }
    }
}

bool AnalogIn8::getInput( uint8_t channelNumber, float& dstValue, bool& dstIsValid )
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
