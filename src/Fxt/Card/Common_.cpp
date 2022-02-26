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
Common_::Common_( const char*            cardName,
                  uint16_t               cardLocalId,
                  Fxt::Point::BankApi&   internalInputsBank,
                  Fxt::Point::BankApi&   registerInputsBank,
                  Fxt::Point::BankApi&   virtualInputsBank,
                  Fxt::Point::BankApi&   internalOutputsBank,
                  Fxt::Point::BankApi&   registerOutputsBank,
                  Fxt::Point::BankApi&   virtualOutputsBank )
    : m_internalInputsBank( internalInputsBank )
    , m_registerInputsBank( registerInputsBank )
    , m_virtualInputsBank( virtualInputsBank )
    , m_internalOutputsBank( internalOutputsBank )
    , m_registerOutputsBank( registerOutputsBank )
    , m_virtualOutputsBank( virtualOutputsBank )
    , m_cardName( cardName )
    , m_localId( cardLocalId )
    , m_started( false )
{
    CPL_SYSTEM_ASSERT( cardName );
}

Common_::~Common_()
{
    // nothing needed yet...
}

//////////////////////////////////////////////////
bool Common_::start() noexcept
{
    if ( !m_started )
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

uint16_t Common_::getLocalId() const noexcept
{
    return m_localId;
}

const char* Common_::getName() const noexcept
{
    return m_cardName;
}


//////////////////////////////////////////////////
bool Common_::scanInputs() noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_registerLock );
    return m_virtualInputsBank.copyFrom( m_registerInputsBank );
}

bool Common_::flushOutputs() noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_registerLock );
    return m_registerOutputsBank.copyFrom( m_virtualOutputsBank );
}

bool Common_::updateInputRegisters() noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection2( m_internalLock );
    Cpl::System::Mutex::ScopeBlock criticalSection( m_registerLock );
    return m_registerInputsBank.copyFrom( m_internalInputsBank );
}

bool Common_::readOutputRegisters() noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection2( m_internalLock );
    Cpl::System::Mutex::ScopeBlock criticalSection( m_registerLock );
    return m_internalOutputsBank.copyFrom( m_registerOutputsBank );
}
