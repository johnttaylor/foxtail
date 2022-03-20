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

#define CREATE_BANK(b)      memBank = allocator.allocate( sizeof( Fxt::Point::Bank ) ); \
                            if ( memBank == nullptr ) { m_error = ERR_MEMORY_POINT_BANKS; } \
                            else { b = new(memBank) Fxt::Point::Bank(); }

#define DESTROY_BANK(b)     if ( b != nullptr ) {b->~Bank();}

static const char emptyString_[1] ={ '\0' };

///
using namespace Fxt::Card;

//////////////////////////////////////////////////
Common_::Common_( Cpl::Memory::ContiguousAllocator& allocator )
    : m_allocator( allocator )
    , m_error( 0 )
    , m_cardName( nullptr )
    , m_localId( 0 )
    , m_slotNumber( 0 )
    , m_started( false )
{
    CPL_SYSTEM_ASSERT( cardName );

    void* memBank;
    CREATE_BANK( m_banks.internalInputs );
    CREATE_BANK( m_banks.registerInputs );
    CREATE_BANK( m_banks.virtualInputs );
    CREATE_BANK( m_banks.internalOutputs );
    CREATE_BANK( m_banks.registerOutputs );
    CREATE_BANK( m_banks.virtualOutputs );
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

uint32_t Common_::getLocalId() const noexcept
{
    return m_localId;
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
    Cpl::System::Mutex::ScopeBlock criticalSection( m_registerLock );
    return m_banks.virtualInputs->copyFrom( *m_banks.registerInputs );
}

bool Common_::flushOutputs() noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_registerLock );
    return m_banks.registerOutputs->copyFrom( *m_banks.virtualOutputs );
}

bool Common_::updateInputRegisters() noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_registerLock );
    return m_banks.registerInputs->copyFrom( *m_banks.internalInputs );
}

bool Common_::readOutputRegisters() noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_registerLock );
    return m_banks.internalOutputs->copyFrom( *m_banks.registerOutputs );
}


bool Common_::parseCommon( JsonVariant& obj, const char* expectedGuid ) noexcept
{
    // Validate the card type
    if ( obj["guid"].isNull() || strcmp( obj["guid"], expectedGuid ) != 0 )
    {
        m_error = ERR_GUID_WRONG_TYPE;
        return false;
    }

    // Ensure that a LocalId has been assigned
    if ( obj["localId"].isNull() )
    {
        m_error = ERR_CARD_MISSING_LOCAL_ID;
        return false;
    }
    m_localId = obj["localId"];

    // Ensure that a Slot ID has been assigned
    if ( obj["slot"].isNull() )
    {
        m_error = ERR_CARD_MISSING_SLOT_ID;
        return false;
    }
    m_slotNumber = obj["slot"];

    // Get the Text label
    const char* name = obj["name"];
    if ( name == nullptr )
    {
        m_error = ERR_CARD_MISSING_NAME;
        return false;
    }
    m_cardName = (char*) m_allocator.allocate( strlen( name ) + 1 );
    if ( m_cardName == nullptr )
    {
        m_cardName = (char*) emptyString_;
        m_error    = ERR_MEMORY_CARD_NAME;
        return false;
    }
    strcpy( m_cardName, name );

    return true;
}
