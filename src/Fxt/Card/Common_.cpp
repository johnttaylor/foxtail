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

///
using namespace Fxt::Card;

//////////////////////////////////////////////////
Common_::Common_( Banks_T&  banks )
    : m_banks( banks )
    , m_error( 0 )
    , m_cardName( nullptr )
    , m_localId( 0 )
    , m_slotNumber( 0 )
    , m_started( false )
{
    CPL_SYSTEM_ASSERT( banks.internalInputs );
    CPL_SYSTEM_ASSERT( banks.registerInputs );
    CPL_SYSTEM_ASSERT( banks.virtualInputs );
    CPL_SYSTEM_ASSERT( banks.internalOutputs );
    CPL_SYSTEM_ASSERT( banks.registerOutputs );
    CPL_SYSTEM_ASSERT( banks.virtualOutputs );
    CPL_SYSTEM_ASSERT( cardName );
}

Common_::~Common_()
{
    // nothing needed yet...
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
