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


#include "Common_.h"
#include "Cpl/System/Assert.h"
#include "Fxt/Point/Bank.h"
#include "Fxt/Point/Api.h"
#include <new>


///
using namespace Fxt::Card;

//////////////////////////////////////////////////
Common_::Common_( DatabaseApi &                     cardDb,
                  Cpl::Memory::ContiguousAllocator& m_generalAllocator,
                  Cpl::Memory::ContiguousAllocator& statefulDataAllocator,
                  Fxt::Point::DatabaseApi&          dbForPoints,
                  uint16_t                          cardId)
    : m_dbForPoints( dbForPoints )
    , m_generalAllocator( m_generalAllocator )
    , m_statefulDataAllocator( statefulDataAllocator )
    , m_error( fullErr(Err_T::SUCCESS) )
    , m_id( cardId )
    , m_started( false )
{
    CPL_SYSTEM_ASSERT( cardName );
    if ( !cardDb.add( *this ) )
    {
        m_error = fullErr(Err_T::CARD_INVALID_ID);
    }
}

Common_::~Common_()
{
}

//////////////////////////////////////////////////
bool Common_::start() noexcept
{
    if ( !m_started && m_error == fullErr( Err_T::SUCCESS ) )
    {
        m_started = true;
        return true;
    }
    return false;
}

bool Common_::stop() noexcept
{
    if ( m_started )
    {
        m_started = false;
        return true;
    }
    return false;
}

bool Common_::isStarted() const noexcept
{
    return m_started;
}

uint16_t Common_::getId() const noexcept
{
    return m_id;
}


Fxt::Type::Error Common_::getErrorCode() const noexcept
{
    return m_error;
}


//////////////////////////////////////////////////
bool Common_::scanInputs() noexcept
{
    return m_virtualInputs.copyStatefulMemoryFrom( m_registerInputs );
}

bool Common_::flushOutputs() noexcept
{
    return m_registerOutputs.copyStatefulMemoryFrom( m_virtualOutputs );
}


bool Common_::createDescriptors( Fxt::Point::Descriptor::CreateFunc_T createFunc,
                                 Fxt::Point::Descriptor*              vpointDesc[],
                                 Fxt::Point::Descriptor*              ioRegDesc[],
                                 uint16_t                             channelIds[],
                                 JsonArray&                           json,
                                 size_t                               numDescriptors ) noexcept
{
    // Initialize the descriptor elements
    for ( size_t i=0; i < numDescriptors; i++ )
    {
        // Parse IDs, and Name
        uint32_t id         = json[i]["id"] | Point::Api::INVALID_ID;
        uint32_t ioRegId    = json[i]["ioRegId"] | Point::Api::INVALID_ID;
        if ( id == Point::Api::INVALID_ID || ioRegId == Point::Api::INVALID_ID  )
        {
            m_error = fullErr(Err_T::POINT_MISSING_ID);
            return false;
        }

        // Parse channel Number
        channelIds[i] = json[i]["channel"] | 0;   // Zero is NOT a valid channel number
        if ( channelIds[i] == 0 )
        {
            m_error = fullErr(Err_T::BAD_CHANNEL_ASSIGNMENTS);
            return false;
        }
        
        // Ensure Channel Number is unique
        for ( size_t j=0; j < numDescriptors; j++ )
        {
            if ( j != i && channelIds[j] == channelIds[i] )
            {
                m_error = fullErr(Err_T::BAD_CHANNEL_ASSIGNMENTS);
                return false;
            }
        }

        // Parse and Allocate memory for the point name
        const char* name          = json[i]["name"];
        char*       nameMemoryPtr = (char*) m_generalAllocator.allocate( strlen( name ) + 1 );
        if ( nameMemoryPtr == nullptr )
        {
            m_error = fullErr(Err_T::MEMORY_DESCRIPTOR_NAME);
            return false;
        }
        strcpy( nameMemoryPtr, name );

        // Create Virtual Point descriptor
        Fxt::Point::Descriptor* memDescriptor = (Fxt::Point::Descriptor*) m_generalAllocator.allocate( sizeof( Fxt::Point::Descriptor ) );
        if ( memDescriptor == nullptr )
        {
            m_error = fullErr(Err_T::MEMORY_DESCRIPTORS);
            return false;
        }
        vpointDesc[i] = new(memDescriptor) Fxt::Point::Descriptor( id, nameMemoryPtr, createFunc );

        // Parse the optional Initial value (aka a 'setter point')
        Fxt::Point::Setter* setter    = nullptr;
        JsonVariant         setterObj = json[i]["initial"];
        if ( !setterObj.isNull() )
        {
            uint32_t setterId = setterObj["id"] | Point::Api::INVALID_ID;
            if ( setterId == Point::Api::INVALID_ID )
            {
                m_error = fullErr(Err_T::POINT_MISSING_ID);
                return false;
            }

            setter = Fxt::Point::Setter::create( m_dbForPoints,
                                                 createFunc,
                                                 setterId,
                                                 name,
                                                 m_generalAllocator,
                                                 m_generalAllocator,
                                                 setterObj );
            if ( setter == nullptr )
            {
                m_error = fullErr(Err_T::CARD_SETTER_ERROR);
                return false;
            }
        }


        // Create IO Register descriptor
        memDescriptor = (Fxt::Point::Descriptor*) m_generalAllocator.allocate( sizeof( Fxt::Point::Descriptor ) );
        if ( memDescriptor == nullptr )
        {
            m_error = fullErr(Err_T::MEMORY_DESCRIPTORS);
            return false;
        }
        ioRegDesc[i] = new(memDescriptor) Fxt::Point::Descriptor( ioRegId, nameMemoryPtr, createFunc, setter );
    }
    return true;
}

void Common_::setInitialValue( Fxt::Point::Descriptor* descriptor ) noexcept
{
    if ( descriptor != nullptr )
    {
        // Set the initial INPUT value (if one was provided)
        Fxt::Point::Setter* setterPtr = descriptor->getSetter();
        if ( setterPtr )
        {
            Fxt::Point::Api* ioRegPtr = m_dbForPoints.lookupById( descriptor->getPointId() );
            if ( ioRegPtr )
            {
                // Set the IO Register state
                setterPtr->setValue( *ioRegPtr );
            }
        }
    }
}
