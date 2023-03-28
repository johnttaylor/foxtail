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


///////////////////////////////////////////////////////////////////////////////
Digital8::Digital8( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                    Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                    Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                    Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                    Fxt::Point::DatabaseApi&           dbForPoints,
                    JsonVariant&                       cardObject,
                    Cpl::Dm::MailboxServer*            cardMboxNotUsed,
                    void*                              extraArgsNotUsed )
    : Fxt::Card::Common_( MAX_INPUTS, MAX_OUTPUTS )
{
    if ( initialize( generalAllocator, cardObject ) )
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
    if ( !Fxt::Point::Api::validatePointTypes( m_inputIoRegisterPoints, m_numInputs, Fxt::Point::Bool::GUID_STRING ) ||
         !Fxt::Point::Api::validatePointTypes( m_outputIoRegisterPoints, m_numOutputs, Fxt::Point::Bool::GUID_STRING ) )
    {
        m_error = fullErr( Err_T::POINT_WRONG_TYPE );
        m_error.logIt( getTypeName() );
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
void Digital8::writeInputs( uint8_t bitsToWrite )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numInputs > 0 )
    {
        // Set the IO Registers - LSb first
        for ( uint8_t idx=0, mask=1; idx < MAX_INPUTS; idx++, mask <<= 1 )
        {
            bool newBitVal = (bitsToWrite & mask) == mask ? true : false;
            writeInput( idx, newBitVal );
        }
    }
}

void Digital8::writeInput( uint8_t bitPosition, bool newValue )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numInputs > 0 )
    {
        Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_inputIoRegisterPoints[bitPosition];
        if ( pt )
        {
            pt->write( newValue );
        }
    }
}

void Digital8::setInputs( uint8_t bitMaskToSet )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numInputs > 0 )
    {
        // OR the IO Registers - LSb first
        for ( uint8_t idx=0, mask=1; idx < MAX_INPUTS; idx++, mask <<= 1 )
        {
            bool setBit = (bitMaskToSet & mask) == mask ? true : false;
            if ( setBit )
            {
                setInputBit( idx );
            }
        }
    }
}

void Digital8::setInputBit( uint8_t bitPosition )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numInputs > 0 )
    {
        Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_inputIoRegisterPoints[bitPosition];
        if ( pt )
        {
            pt->write( true );
        }
    }
}

void Digital8::clearInputs( uint8_t bitMaskToClear )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numInputs > 0 )
    {
        // Clear the IO Registers - LSb first
        for ( uint8_t idx=0, mask=1; idx < MAX_INPUTS; idx++, mask <<= 1 )
        {
            bool clearBit = (bitMaskToClear & mask) == mask ? true : false;
            if ( clearBit )
            {
                clearInputBit( idx );
            }
        }
    }
}

void Digital8::clearInputBit( uint8_t bitPosition )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numInputs > 0 )
    {
        Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_inputIoRegisterPoints[bitPosition];
        if ( pt )
        {
            pt->write( false );
        }
    }
}

void Digital8::toggleInputs( uint8_t bitMaskToToggle )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numInputs > 0 )
    {
        // Toggle the IO Registers - LSb first
        for ( uint8_t idx=0, mask=1; idx < MAX_INPUTS; idx++, mask <<= 1 )
        {
            bool toggleBit = (bitMaskToToggle & mask) == mask ? true : false;
            if ( toggleBit )
            {
                toggleInputBit( idx );
            }
        }
    }
}

void Digital8::toggleInputBit( uint8_t bitPosition )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numInputs > 0 )
    {
        Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_inputIoRegisterPoints[bitPosition];
        if ( pt )
        {
            bool curVal = false;
            pt->read( curVal );
            pt->write( !curVal );
        }
    }
}


bool Digital8::getInputs( uint8_t& dstInputVal )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numInputs > 0 )
    {
        // Read all of the IO Registers - LSb first
        dstInputVal = 0;
        for ( uint8_t idx=0, mask=1; idx < MAX_INPUTS; idx++, mask <<= 1 )
        {
            bool inBit = false;
            if ( !getInput( idx, inBit ) )
            {
                return false;
            }
            if ( inBit )
            {
                dstInputVal |= mask;
            }
        }

        return true;
    }

    return false;
}

bool Digital8::getInput( uint8_t bitPosition, bool& dstInputVal )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numInputs > 0 )
    {
        Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_inputIoRegisterPoints[bitPosition];
        if ( pt )
        {
            return pt->read( dstInputVal );
        }
    }

    return true;
}

void Digital8::setInputsInvalid( uint8_t bitMaskToInvalidate )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numInputs > 0 )
    {
        // Toggle the IO Registers - LSb first
        for ( uint8_t idx=0, mask=1; idx < MAX_INPUTS; idx++, mask <<= 1 )
        {
            bool invalidate = (bitMaskToInvalidate & mask) == mask ? true : false;
            if ( invalidate )
            {
                setInputInvalid( idx );
            }
        }
    }
}

void Digital8::setInputInvalid( uint8_t bitPosition )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numInputs > 0 )
    {
        Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_inputIoRegisterPoints[bitPosition];
        if ( pt )
        {
            return pt->setInvalid();
        }
    }
}


//////////////////////////////////////////////////////////////////
bool Digital8::getOutputs( uint8_t& dstOutputVal )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numOutputs > 0 )
    {
        // Read all of the IO Registers - LSb first
        dstOutputVal = 0;
        for ( uint8_t idx=0, mask=1; idx < MAX_OUTPUTS; idx++, mask <<= 1 )
        {
            bool outBit = false;
            if ( !getOutput( idx, outBit ) )
            {
                return false;
            }
            if ( outBit )
            {
                dstOutputVal |= mask;
            }
        }

        return true;
    }

    return false;
}


bool Digital8::getOutput( uint8_t bitPosition, bool& dstOutputVal )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numOutputs > 0 )
    {
        Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_outputIoRegisterPoints[bitPosition];
        if ( pt )
        {
            return pt->read( dstOutputVal );
        }
    }

    return true;
}

void Digital8::setOutputsInvalid( uint8_t bitMaskToInvalidate )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numOutputs > 0 )
    {
        // Toggle the IO Registers - LSb first
        for ( uint8_t idx=0, mask=1; idx < MAX_OUTPUTS; idx++, mask <<= 1 )
        {
            bool invalidate = (bitMaskToInvalidate & mask) == mask ? true : false;
            if ( invalidate )
            {
                setOutputInvalid( idx );
            }
        }
    }
}

void Digital8::setOutputInvalid( uint8_t bitPosition )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_numOutputs > 0 )
    {
        Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_outputIoRegisterPoints[bitPosition];
        if ( pt )
        {
            return pt->setInvalid();
        }
    }
}

