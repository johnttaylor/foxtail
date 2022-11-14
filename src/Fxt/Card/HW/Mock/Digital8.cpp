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


#include "Digital8.h"
#include "Cpl/System/Assert.h"
#include "Fxt/Point/Uint8.h"
#include <stdint.h>
#include <new>

///
using namespace Fxt::Card::HW::Mock;


///////////////////////////////////////////////////////////////////////////////
Digital8::Digital8( DatabaseApi&                       cardDb,
                    Cpl::Memory::ContiguousAllocator&  generalAllocator,
                    Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                    Fxt::Point::DatabaseApi&           dbForPoints,
                    uint16_t                           cardId,
                    JsonVariant&                       cardObject )
    : Fxt::Card::Common_( cardDb, generalAllocator, statefulDataAllocator, dbForPoints, cardId )
{
    memset( &m_virtualInDescriptors, 0, sizeof( m_virtualInDescriptors ) );
    memset( &m_virtualOutDescriptors, 0, sizeof( m_virtualOutDescriptors ) );
    memset( &m_ioRegInDescriptors, 0, sizeof( m_ioRegInDescriptors ) );
    memset( &m_ioRegOutDescriptors, 0, sizeof( m_ioRegOutDescriptors ) );

    if ( m_error == FXT_CARD_ERR_NO_ERROR )
    {
        if ( parseConfiguration( cardObject ) )
        {
            createPoints();
        }
    }
}

Digital8::~Digital8()
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

        if ( m_virtualOutDescriptors[i] != nullptr )
        {
            m_virtualOutDescriptors[i]->~Descriptor();
        }
        if ( m_ioRegOutDescriptors[i] != nullptr )
        {
            m_ioRegOutDescriptors[i]->~Descriptor();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Digital8::parseConfiguration( JsonVariant& obj ) noexcept
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
            uint16_t dummyChannellist[1];
            if ( !createDescriptors( Fxt::Point::Uint8::create,
                                     m_virtualInDescriptors,
                                     m_ioRegInDescriptors,
                                     dummyChannellist,
                                     inputs,
                                     numInputs ) )
            {
                return false;
            }
        }

        // Parse Output descriptors
        JsonArray outputs = obj["points"]["outputs"];
        if ( !outputs.isNull() )
        {
            // Validate supported number of signals
            size_t numOutputs = inputs.size();
            numOutputs = outputs.size();
            if ( numOutputs > MAX_DESCRIPTORS )
            {
                m_error = FXT_CARD_ERR_TOO_MANY_OUTPUT_POINTS;
                return false;
            }

            // Create the Output descriptors
            uint16_t dummyChannellist[1];
            if ( !createDescriptors( Fxt::Point::Uint8::create,
                                     m_virtualOutDescriptors,
                                     m_ioRegOutDescriptors,
                                     dummyChannellist,
                                     outputs,
                                     numOutputs) )
            {
                return false;
            }
        }
    }

    return true;
}



void Digital8::createPoints() noexcept
{
    // Create INPUT Points
    m_registerInputs.populate( &m_ioRegInDescriptors[0], m_generalAllocator, m_dbForPoints, m_statefulDataAllocator );
    m_virtualInputs.populate( &m_virtualInDescriptors[0], m_generalAllocator, m_dbForPoints, m_statefulDataAllocator );

    // Create OUTPUT points
    m_registerOutputs.populate( &m_ioRegOutDescriptors[0], m_generalAllocator, m_dbForPoints, m_statefulDataAllocator );
    m_virtualOutputs.populate( &m_virtualOutDescriptors[0], m_generalAllocator, m_dbForPoints, m_statefulDataAllocator );
}


///////////////////////////////////////////////////////////////////////////////
bool Digital8::start() noexcept
{
    // Fail if there was error during construction
    if ( m_error != FXT_CARD_ERR_NO_ERROR )
    {
        return false;
    }

    // Set the initial IO Register values
    setInitialValue( m_ioRegInDescriptors[0] );
    setInitialValue( m_ioRegOutDescriptors[0] );

    // Call the parent's method do complete the start-up actions
    return Common_::start();
}



bool Digital8::stop() noexcept
{
    return Common_::stop();
}

const char* Digital8::getTypeGuid() const noexcept
{
    return GUID_STRING;
}

const char* Digital8::getTypeName() const noexcept
{
    return TYPE_NAME;
}


///////////////////////////////////////////////////////////////////////////////
void Digital8::setInputs( uint8_t bitMaskToOR )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
    Fxt::Point::Uint8* ioRegPtr = (Fxt::Point::Uint8*) m_dbForPoints.lookupById( m_ioRegInDescriptors[0]->getPointId() );
    ioRegPtr->maskOr( bitMaskToOR );
}


void Digital8::clearInputs( uint8_t bitMaskToAND )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
    Fxt::Point::Uint8* ioRegPtr = (Fxt::Point::Uint8*) m_dbForPoints.lookupById( m_ioRegInDescriptors[0]->getPointId() );
    ioRegPtr->maskAnd( bitMaskToAND );
}

void Digital8::toggleInputs( uint8_t bitMaskToXOR )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
    Fxt::Point::Uint8* ioRegPtr = (Fxt::Point::Uint8*) m_dbForPoints.lookupById( m_ioRegInDescriptors[0]->getPointId() );
    ioRegPtr->maskXor( bitMaskToXOR );
}

bool Digital8::getOutputs( uint8_t& valueMask )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
    Fxt::Point::Uint8* ioRegPtr = (Fxt::Point::Uint8*) m_dbForPoints.lookupById( m_ioRegOutDescriptors[0]->getPointId() );
    if ( ioRegPtr->read( valueMask ) )
    {
        return true;
    }
    return false;
}

