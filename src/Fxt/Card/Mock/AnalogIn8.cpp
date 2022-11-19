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
#include "Fxt/Point/Float.h"
#include <stdint.h>
#include <new>

///
using namespace Fxt::Card::Mock;

///////////////////////////////////////////////////////////////////////////////
AnalogIn8::AnalogIn8( DatabaseApi&                       cardDb,
                      Cpl::Memory::ContiguousAllocator&  generalAllocator,
                      Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                      Fxt::Point::DatabaseApi&           dbForPoints,
                      uint16_t                           cardId,
                      JsonVariant&                       cardObject )
    : Fxt::Card::Common_( cardDb, generalAllocator, statefulDataAllocator, dbForPoints, cardId )
{
    memset( &m_virtualInDescriptors, 0, sizeof( m_virtualInDescriptors ) );
    memset( &m_ioRegInDescriptors, 0, sizeof( m_ioRegInDescriptors ) );

    if ( parseConfiguration( cardObject ) )
    {
        createPoints();
    }
}

AnalogIn8::~AnalogIn8()
{
    // NOTE: Call the destructors on ALL dynamically allocated objects - but 
    //       the Application is responsible for freeing the actual memory

    // Destroy any created Descriptors
    for ( unsigned i=0; i < MAX_DESCRIPTORS; i++ )
    {
        if ( m_virtualInDescriptors[i] != nullptr )
        {
            m_virtualInDescriptors[i]->~Descriptor();
        }
        if ( m_ioRegInDescriptors[i] != nullptr )
        {
            m_ioRegInDescriptors[i]->~Descriptor();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool AnalogIn8::parseConfiguration( JsonVariant & obj ) noexcept
{
    // Parse point Descriptors
    if ( !obj["points"].isNull() )
    {
        JsonArray inputs = obj["points"]["inputs"];
        if ( !inputs.isNull() )
        {
            // Validate supported number of signals
            size_t numInputs = inputs.size();
            if ( numInputs > MAX_DESCRIPTORS )
            {
                m_error = FXT_CARD_ERR_TOO_MANY_INPUT_POINTS;
                return false;
            }

            // Create the Input descriptors
            if ( !createDescriptors( Fxt::Point::Float::create,
                                     m_virtualInDescriptors,
                                     m_ioRegInDescriptors,
                                     m_channelNumbers,
                                     inputs,
                                     numInputs ) )
            {
                return false;
            }
        }
    }

    return true;
}



void AnalogIn8::createPoints() noexcept
{
    // Create INPUT Points
    m_registerInputs.populate( &m_ioRegInDescriptors[0], m_generalAllocator, m_dbForPoints, m_statefulDataAllocator );
    m_virtualInputs.populate( &m_virtualInDescriptors[0], m_generalAllocator, m_dbForPoints, m_statefulDataAllocator );
}


///////////////////////////////////////////////////////////////////////////////
bool AnalogIn8::start() noexcept
{
    // Fail if there was error during construction
    if ( m_error != FXT_CARD_ERR_NO_ERROR )
    {
        return false;
    }

    // Set the initial IO Register values
    for ( unsigned i=0; i < MAX_DESCRIPTORS; i++ )
    {
        setInitialValue( m_ioRegInDescriptors[i] );
    }

    // Call the parent's method do complete the start-up actions
    return Common_::start();
}



bool AnalogIn8::stop() noexcept
{
    return Common_::stop();
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
void AnalogIn8::setInputs( uint8_t channelNumber, float newValue )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    // Convert channelNumber to a descriptor index
    for ( unsigned i=0; i < MAX_DESCRIPTORS; i++ )
    {
        if ( m_channelNumbers[i] == channelNumber )
        {
            // Found the corresponding descriptor index -->get the IO Register Point
            Fxt::Point::Float* ioRegPtr = (Fxt::Point::Float*) m_dbForPoints.lookupById( m_ioRegInDescriptors[i]->getPointId() );
            if ( ioRegPtr )
            {
                // Update the IO Register
                ioRegPtr->write( newValue );
                break;
            }
        }
    }
}

