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
#include <new>

///
using namespace Fxt::Card::Mock;

#define MAX_INPUT_CHANNELS      1
#define INPUT_POINT_OFFSET      0

#define MAX_OUTPUT_CHANNELS     1
#define OUTPUT_POINT_OFFSET     (MAX_INPUT_CHANNELS)

#define TOTAL_MAX_CHANNELS      (MAX_INPUT_CHANNELS + MAX_OUTPUT_CHANNELS)

///////////////////////////////////////////////////////////////////////////////
Digital8::Digital8( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                    Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                    Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                    Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                    Fxt::Point::DatabaseApi&           dbForPoints,
                    JsonVariant&                       cardObject,
                    Cpl::Itc::PostApi*                 cardMbox )
    : Fxt::Card::Common_( TOTAL_MAX_CHANNELS, generalAllocator, cardObject )
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
    // Parse channels
    if ( !cardObject["points"].isNull() )
    {
        // INPUTS
        JsonArray inputs = cardObject["points"]["inputs"];
        if ( !inputs.isNull() )
        {
            // Validate supported number of signals
            size_t numInputs = inputs.size();
            if ( numInputs > MAX_INPUT_CHANNELS )
            {
                m_error = fullErr( Err_T::TOO_MANY_INPUT_POINTS );
                m_error.logIt();
                return;
            }

            // Create Virtual Points
            for ( size_t idx=0; idx < numInputs; idx++ )
            {
                // Stop processing if/when an error occurred
                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }
                
                uint16_t   channelNum_notUsed;
                JsonObject channelObj = inputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_virtualInputs,
                                       Fxt::Point::Uint8::GUID_STRING,
                                       false,
                                       channelObj,
                                       m_error,
                                       MAX_INPUT_CHANNELS,
                                       INPUT_POINT_OFFSET,
                                       channelNum_notUsed,
                                       generalAllocator,
                                       cardStatefulDataAllocator,
                                       dbForPoints );
            }

            // Create IO Register Points
            for ( size_t idx=0; idx < numInputs; idx++ )
            {
                // Stop processing if/when an error occurred
                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }

                uint16_t   channelNum_notUsed;
                JsonObject channelObj = inputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_ioRegisterInputs,
                                       Fxt::Point::Uint8::GUID_STRING,
                                       true,
                                       channelObj,
                                       m_error,
                                       MAX_INPUT_CHANNELS,
                                       INPUT_POINT_OFFSET,
                                       channelNum_notUsed,
                                       generalAllocator,
                                       cardStatefulDataAllocator,
                                       dbForPoints );
            }
        }

        // OUTPUTS
        JsonArray outputs = cardObject["points"]["outputs"];
        if ( !outputs.isNull() )
        {
            // Validate supported number of signals
            size_t numOutputs = outputs.size();
            if ( numOutputs > MAX_OUTPUT_CHANNELS )
            {
                m_error = fullErr( Err_T::TOO_MANY_OUTPUT_POINTS );
                m_error.logIt();
                return;
            }

            // Create Virtual Points
            for ( size_t idx=0; idx < numOutputs ; idx++ )
            {
                uint16_t   channelNum_notUsed;
                JsonObject channelObj = outputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_virtualOutputs,
                                       Fxt::Point::Uint8::GUID_STRING,
                                       false,
                                       channelObj,
                                       m_error,
                                       MAX_OUTPUT_CHANNELS,
                                       OUTPUT_POINT_OFFSET,
                                       channelNum_notUsed,
                                       generalAllocator,
                                       haStatefulDataAllocator,     // All Output Virtual Points are part of the HA Data set
                                       dbForPoints );

                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }
            }

            // Create IO Register Points
            for ( size_t idx=0; idx < numOutputs; idx++ )
            {
                uint16_t   channelNum_notUsed;
                JsonObject channelObj = outputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_ioRegisterOutputs,
                                       Fxt::Point::Uint8::GUID_STRING,
                                       true,
                                       channelObj,
                                       m_error,
                                       MAX_OUTPUT_CHANNELS,
                                       OUTPUT_POINT_OFFSET,
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
bool Digital8::start( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Call the parent's start-up actions
    if ( Common_::start( currentElapsedTimeUsec ) )
    {
        // Set the initial IO Register values (both inputs and outputs)
        for ( unsigned i=0; i < TOTAL_MAX_CHANNELS; i++ )
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



void Digital8::stop() noexcept
{
    Common_::stop();
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

    if ( m_ioRegisterPoints[INPUT_POINT_OFFSET] != nullptr )
    {
        // Update the IO Register
        Fxt::Point::Uint8* pt = (Fxt::Point::Uint8*) m_ioRegisterPoints[INPUT_POINT_OFFSET];
        pt->write( byteTowrite );
    }
}

void Digital8::orInputs( uint8_t bitMaskToOR )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_ioRegisterPoints[INPUT_POINT_OFFSET] != nullptr )
    {
        // Update the IO Register
        Fxt::Point::Uint8* pt = (Fxt::Point::Uint8*) m_ioRegisterPoints[INPUT_POINT_OFFSET];
        pt->maskOr( bitMaskToOR );
    }
}



void Digital8::clearInputs( uint8_t bitMaskToAND )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_ioRegisterPoints[INPUT_POINT_OFFSET] != nullptr )
    {
        // Update the IO Register
        Fxt::Point::Uint8* pt = (Fxt::Point::Uint8*) m_ioRegisterPoints[INPUT_POINT_OFFSET];
        pt->maskAnd( bitMaskToAND );
    }
}

void Digital8::toggleInputs( uint8_t bitMaskToXOR )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_ioRegisterPoints[INPUT_POINT_OFFSET] != nullptr )
    {
        // Update the IO Register
        Fxt::Point::Uint8* pt = (Fxt::Point::Uint8*) m_ioRegisterPoints[INPUT_POINT_OFFSET];
        pt->maskXor( bitMaskToXOR );
    }
}

bool Digital8::getOutputs( uint8_t& dstOutputVal )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_ioRegisterPoints[OUTPUT_POINT_OFFSET] != nullptr )
    {
        // Read the IO Register
        Fxt::Point::Uint8* pt = (Fxt::Point::Uint8*) m_ioRegisterPoints[OUTPUT_POINT_OFFSET];
        return pt->read( dstOutputVal );
    }

    return false;
}

bool Digital8::getInputs( uint8_t& dstInputVal )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_ioRegisterPoints[INPUT_POINT_OFFSET] != nullptr )
    {
        // Read the IO Register
        Fxt::Point::Uint8* pt = (Fxt::Point::Uint8*) m_ioRegisterPoints[INPUT_POINT_OFFSET];
        return pt->read( dstInputVal );
    }

    return false;
}

void Digital8::setInputsInvalid()
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_ioRegisterPoints[INPUT_POINT_OFFSET] != nullptr )
    {
        // Invalidate the IO Register
        Fxt::Point::Uint8* pt = (Fxt::Point::Uint8*) m_ioRegisterPoints[INPUT_POINT_OFFSET];
        return pt->setInvalid();
    }
}

void Digital8::setOutputsInvalid()
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( m_ioRegisterPoints[OUTPUT_POINT_OFFSET] != nullptr )
    {
        // Invalidate the IO Register
        Fxt::Point::Uint8* pt = (Fxt::Point::Uint8*) m_ioRegisterPoints[OUTPUT_POINT_OFFSET];
        return pt->setInvalid();
    }
}
