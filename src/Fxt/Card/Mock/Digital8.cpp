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
#include <stdint.h>

///
using namespace Fxt::Card::Mock;

#define MAX_INPUT_CHANNELS          1
#define MAX_OUTPUT_CHANNELS         1

///////////////////////////////////////////////////////////////////////////////
Digital8::Digital8( Cpl::Memory::ContiguousAllocator&  generalAllocator,
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
void Digital8::parseConfiguration( Cpl::Memory::ContiguousAllocator&  generalAllocator,
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
                                  0, 0 ) )
    {
        return;
    }

    // Validate the input/output types
    if ( !Fxt::Point::Api::validatePointTypes( m_inputIoRegisterPoints, m_numInputs, Fxt::Point::Uint8::GUID_STRING ) ||
         !Fxt::Point::Api::validatePointTypes( m_outputIoRegisterPoints, m_numOutputs, Fxt::Point::Uint8::GUID_STRING) )
    {
        return;
    }
    
}


///////////////////////////////////////////////////////////////////////////////
bool Digital8::start( Cpl::Itc::PostApi& chassisMbox, uint64_t currentElapsedTimeUsec ) noexcept
{
    // Call the parent's start-up actions (Note: sets the VPoint/IORegPoints initial values)
    return Common_::start( currentElapsedTimeUsec );
}



void Digital8::stop( Cpl::Itc::PostApi& chassisMbox ) noexcept
{
    Common_::stop();
}

bool Digital8::scanInputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Need to wrap with a mutex since my input data is coming from a different thread
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
    return Common_::scanInputs( currentElapsedTimeUsec );
}

bool Digital8::flushOutputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Need to wrap with a mutex since my input data is coming from a different thread
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
    return Common_::flushOutputs( currentElapsedTimeUsec );
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
void Digital8::writeInputs( uint8_t byteTowrite )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numInputs > 0 )
    {
        // Update the IO Register
        Fxt::Point::Uint8* pt = (Fxt::Point::Uint8*) m_inputIoRegisterPoints[0];
        pt->write( byteTowrite );
    }
}

void Digital8::orInputs( uint8_t bitMaskToOR )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numInputs > 0 )
    {
        // Update the IO Register
        Fxt::Point::Uint8* pt = (Fxt::Point::Uint8*) m_inputIoRegisterPoints[0];
        pt->maskOr( bitMaskToOR );
    }
}

void Digital8::clearInputs( uint8_t bitMaskToAND )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numInputs > 0 )
    {
        // Update the IO Register
        Fxt::Point::Uint8* pt = (Fxt::Point::Uint8*) m_inputIoRegisterPoints[0];
        pt->maskAnd( bitMaskToAND );
    }
}

void Digital8::toggleInputs( uint8_t bitMaskToXOR )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numInputs > 0 )
    {
        // Update the IO Register
        Fxt::Point::Uint8* pt = (Fxt::Point::Uint8*) m_inputIoRegisterPoints[0];
        pt->maskXor( bitMaskToXOR );
    }
}

bool Digital8::getInputs( uint8_t& dstInputVal )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numInputs > 0 )
    {
        // Read the IO Register
        Fxt::Point::Uint8* pt = (Fxt::Point::Uint8*) m_inputIoRegisterPoints[0];
        return pt->read( dstInputVal );
    }

    return false;
}

void Digital8::setInputsInvalid()
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numInputs > 0 )
    {
        // Invalidate the IO Register
        Fxt::Point::Uint8* pt = (Fxt::Point::Uint8*) m_inputIoRegisterPoints[0];
        return pt->setInvalid();
    }
}


bool Digital8::getOutputs( uint8_t& dstOutputVal )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numOutputs > 0 )
    {
        // Read the IO Register
        Fxt::Point::Uint8* pt = (Fxt::Point::Uint8*) m_outputIoRegisterPoints[0];
        return pt->read( dstOutputVal );
    }

    return false;
}

void Digital8::setOutputsInvalid()
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numOutputs > 0 )
    {
        // Invalidate the IO Register
        Fxt::Point::Uint8* pt = (Fxt::Point::Uint8*) m_outputIoRegisterPoints[0];
        return pt->setInvalid();
    }
}
