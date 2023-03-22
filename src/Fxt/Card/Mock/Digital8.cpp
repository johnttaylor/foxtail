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
#define STARTING_INPUT_CHANNEL_NUM  1
#define ENDING_INPUT_CHANNEL_NUM    MAX_INPUT_CHANNELS

#define MAX_OUTPUT_CHANNELS         MAX_INPUT_CHANNELS
#define STARTING_OUTPUT_CHANNEL_NUM 1
#define ENDING_OUTPUT_CHANNEL_NUM   MAX_OUTPUT_CHANNELS

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
    // Parse channels
    if ( !cardObject["points"].isNull() )
    {
        // INPUTS
        JsonArray inputs = cardObject["points"]["inputs"];
        if ( !inputs.isNull() )
        {
            // Validate supported number of signals
            size_t nInputs = inputs.size();
            if ( nInputs > MAX_INPUT_CHANNELS )
            {
                m_error = fullErr( Err_T::TOO_MANY_INPUT_POINTS );
                m_error.logIt();
                return;
            }
            m_numInputs = (uint16_t)nInputs;

            // Create Virtual Points
            for ( size_t idx=0; idx < m_numInputs; idx++ )
            {
                uint16_t   channelNum_notUsed;
                JsonObject channelObj = inputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_virtualInputs,
                                       Fxt::Point::Uint8::GUID_STRING,
                                       false,
                                       channelObj,
                                       m_error,
                                       STARTING_INPUT_CHANNEL_NUM,
                                       ENDING_INPUT_CHANNEL_NUM,
                                       channelNum_notUsed,
                                       generalAllocator,
                                       cardStatefulDataAllocator,
                                       dbForPoints );

                // Stop processing if/when an error occurred
                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }

            }

            // Create IO Register Points
            for ( size_t idx=0; idx < m_numInputs; idx++ )
            {

                uint16_t   channelNum;
                JsonObject channelObj = inputs[idx].as<JsonObject>();
                Fxt::Point::Api* pt   = createPointForChannel( pointFactoryDb,
                                                               m_ioRegisterInputs,
                                                               Fxt::Point::Uint8::GUID_STRING,
                                                               true,
                                                               channelObj,
                                                               m_error,
                                                               STARTING_INPUT_CHANNEL_NUM,
                                                               ENDING_INPUT_CHANNEL_NUM,
                                                               channelNum,
                                                               generalAllocator,
                                                               cardStatefulDataAllocator,
                                                               dbForPoints );

                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }

                // Cache the IO Register PT.  NOTE: The IO Register index is function of the channel number!
                m_inputIoRegisterPoints[channelNum - 1] = pt;
            }
        }

        // OUTPUTS
        JsonArray outputs = cardObject["points"]["outputs"];
        if ( !outputs.isNull() )
        {
            // Validate supported number of signals
            size_t nOutputs = outputs.size();
            if ( nOutputs > MAX_OUTPUT_CHANNELS )
            {
                m_error = fullErr( Err_T::TOO_MANY_OUTPUT_POINTS );
                m_error.logIt();
                return;
            }
            m_numOutputs = (uint16_t) nOutputs;

            // Create Virtual Points
            for ( size_t idx=0; idx < m_numOutputs; idx++ )
            {
                uint16_t   channelNum_notUsed;
                JsonObject channelObj = outputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_virtualOutputs,
                                       Fxt::Point::Uint8::GUID_STRING,
                                       false,
                                       channelObj,
                                       m_error,
                                       STARTING_OUTPUT_CHANNEL_NUM,
                                       ENDING_OUTPUT_CHANNEL_NUM,
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
            for ( size_t idx=0; idx < m_numOutputs; idx++ )
            {
                uint16_t   channelNum;
                JsonObject channelObj = outputs[idx].as<JsonObject>();
                Fxt::Point::Api* pt   = createPointForChannel( pointFactoryDb,
                                                               m_ioRegisterOutputs,
                                                               Fxt::Point::Uint8::GUID_STRING,
                                                               true,
                                                               channelObj,
                                                               m_error,
                                                               STARTING_OUTPUT_CHANNEL_NUM,
                                                               ENDING_OUTPUT_CHANNEL_NUM,
                                                               channelNum,
                                                               generalAllocator,
                                                               cardStatefulDataAllocator,
                                                               dbForPoints );

                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }

                // Cache the IO Register PT.  NOTE: The IO Register index is function of the channel number!
                m_outputIoRegisterPoints[channelNum - 1] = pt;
            }
        }
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
