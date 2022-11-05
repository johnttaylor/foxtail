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
#include <new>


#define DESTROY_BANK(b)     if ( b != nullptr ) {b->~Bank();}

static const char emptyString_[1] ={ '\0' };

///
using namespace Fxt::Card;

//////////////////////////////////////////////////
Common_::Common_( Cpl::Memory::ContiguousAllocator& allocator )
    : m_allocator( allocator )
    , m_error( 0 )
    , m_cardName( nullptr )
    , m_id( 0 )
    , m_slotNumber( 0 )
    , m_started( false )
{
    CPL_SYSTEM_ASSERT( cardName );
    m_banks.internalInputs  = createBank( allocator );
    m_banks.registerInputs  = createBank( allocator );
    m_banks.virtualInputs   = createBank( allocator );
    m_banks.internalOutputs = createBank( allocator );
    m_banks.registerOutputs = createBank( allocator );
    m_banks.virtualOutputs  = createBank( allocator );
}

Common_::~Common_()
{
    DESTROY_BANK( m_banks.internalInputs );
    DESTROY_BANK( m_banks.registerInputs );
    DESTROY_BANK( m_banks.virtualInputs );
    DESTROY_BANK( m_banks.internalOutputs );
    DESTROY_BANK( m_banks.registerOutputs );
    DESTROY_BANK( m_banks.virtualOutputs );
}

Fxt::Point::Bank* Common_::createBank( Cpl::Memory::ContiguousAllocator & allocator )
{
    void* memory = allocator.allocate( sizeof( Fxt::Point::Bank ) );
    if ( memory == nullptr )
    {
        m_error = FXT_CARD_ERR_MEMORY_POINT_BANKS;
        return nullptr;
    }

    return new(memory) Fxt::Point::Bank();
}

//////////////////////////////////////////////////
bool Common_::start() noexcept
{
    if ( !m_started && m_error == 0 )
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

const char* Common_::getName() const noexcept
{
    return m_cardName;
}

uint32_t Common_::getErrorCode() const noexcept
{
    return m_error;
}

uint16_t Common_::getSlot() const noexcept
{
    return m_slotNumber;
}

//////////////////////////////////////////////////
bool Common_::scanInputs() noexcept
{
    return m_banks.virtualInputs->copyStatefulMemoryFrom( *m_banks.registerInputs );
}

bool Common_::flushOutputs() noexcept
{
    return m_banks.registerOutputs->copyStatefulMemoryFrom( *m_banks.virtualOutputs );
}

bool Common_::updateInputRegisters() noexcept
{
    return m_banks.registerInputs->copyStatefulMemoryFrom( *m_banks.internalInputs );
}

bool Common_::readOutputRegisters() noexcept
{
    return m_banks.internalOutputs->copyStatefulMemoryFrom( *m_banks.registerOutputs );
}


bool Common_::parseCommon( JsonVariant& obj, const char* expectedGuid ) noexcept
{
    // Validate the card type
    if ( obj["type"].isNull() || strcmp( obj["type"], expectedGuid ) != 0 )
    {
        m_error = FXT_CARD_ERR_GUID_WRONG_TYPE;
        return false;
    }

    // Ensure that a Id has been assigned
    if ( obj["id"].isNull() )
    {
        m_error = FXT_CARD_ERR_CARD_MISSING_ID;
        return false;
    }
    m_id = obj["id"];

    // Ensure that a Slot ID has been assigned
    if ( obj["slot"].isNull() )
    {
        m_error = FXT_CARD_ERR_CARD_MISSING_SLOT_ID;
        return false;
    }
    m_slotNumber = obj["slot"];

    // Get the Text label
    const char* name = obj["name"];
    if ( name == nullptr )
    {
        m_error = FXT_CARD_ERR_CARD_MISSING_NAME;
        return false;
    }
    m_cardName = (char*) m_allocator.allocate( strlen( name ) + 1 );
    if ( m_cardName == nullptr )
    {
        m_cardName = (char*) emptyString_;
        m_error    = FXT_CARD_ERR_MEMORY_CARD_NAME;
        return false;
    }
    strcpy( m_cardName, name );

    return true;
}
