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


#include "Digital.h"
#include "Cpl/System/Assert.h"
#include "Cpl/Point/Bool.h"
#include <stdint.h>

///
using namespace Fxt::Card::HW::Mock;

Cpl::Type::Guid_T Digital::g_guid;

///////////////////////////////////////////////////////////////////////////////
Digital::Digital( const char*                                                    cardName,
                  uint16_t                                                       cardLocalId,
                  JsonVariant&                                                   staticConfigObject,
                  JsonVariant&                                                   runtimeConfigObject,
                  Fxt::Point::BankApi&                                           internalInputsBank,
                  Fxt::Point::BankApi&                                           registerInputsBank,
                  Fxt::Point::BankApi&                                           virtualInputsBank,
                  Fxt::Point::BankApi&                                           internalOutputsBank,
                  Fxt::Point::BankApi&                                           registerOutputsBank,
                  Fxt::Point::BankApi&                                           virtualOutputsBank,
                  uint32_t&                                                      startEndPointIdValue,
                  Cpl::Container::Dictionary<Cpl::Container::KeyUinteger32_T>    virtualPointDatabase,
                  Cpl::Memory::ContiguousAllocator&                              allocator )
    : Fxt::Card::Common_( cardName, cardLocalId, internalInputsBank, registerInputsBank, virtualInputsBank, internalOutputsBank, m_registerOutputsBank, m_virtualOutputsBank )
    , m_allocator( allocator )
    , m_configErrString( "" )
    , m_numInputs( 0 )
    , m_numOutputs( 0 )
    , m_initInputVals( 0 )
    , m_initOutputVals( 0 )
    , m_configErr( false )
{
    memset( &m_internalInDescriptors, 0, sizeof( m_internalInDescriptors ) );
    memset( &m_internalOutDescriptors, 0, sizeof( m_internalOutDescriptors ) );
    memset( &m_registerInDescriptors, 0, sizeof( m_registerInDescriptors ) );
    memset( &m_registerOutDescriptors, 0, sizeof( m_registerOutDescriptors ) );
    memset( &m_virtualInDescriptors, 0, sizeof( m_virtualInDescriptors ) );
    memset( &m_virtualOutDescriptors, 0, sizeof( m_virtualOutDescriptors ) );

    parseStaticConfig( staticConfigObject );
    parseRuntimeConfig( runtimeConfigObject );
}

void Digital::parseStaticConfig( JsonVariant& obj ) noexcept
{
    m_numInputs      = obj["numInputs"] | 0;
    m_numOutputs     = obj["numOutputs"] | 0;
    m_initInputVals  = obj["initialInputstVals"] | 0;
    m_initOutputVals = obj["initialOutputVals"] | 0;
}

// TODO -->only need one set of Descriptors -->can be reused for the internal/regsiters
void Digital::parseRuntimeConfig( JsonVariant& obj, Cpl::Container::Dictionary<Cpl::Container::KeyUinteger32_T> virtualPointDatabase ) noexcept
{
    // Parse input descriptors
    if ( !obj["descriptors"].isNull() )
    {
        JsonArray inputs = obj["descriptors"]["inputs"];
        if ( !inputs.isNull() )
        {
            // Check for Mismatch input configuration
            if ( inputs.size() != m_numInputs )
            {
                m_configErr = true;
                m_configErrString = "Mismatch inputs.  Number of Inputs does not match array of Input descriptors";
                return;
            }

            // Create the Input descriptors
            if ( !createDescriptors( m_internalInDescriptors, inputs, m_numInputs, "Failed to allocate memory for Virtual Input Descriptors fields." ) )
            {
                return;
            }
            if ( !createDescriptors( m_registerInDescriptors, inputs, m_numInputs, "Failed to allocate memory for Register Input Descriptors fields." ) )
            {
                return;
            }
            if ( !createDescriptors( m_virtualInDescriptors, inputs, m_numInputs, "Failed to allocate memory for Register Input Descriptors fields." ) )
            {
                return;
            }

            // All Virtual Point Descriptors to the database
            for ( int i=0; i < m_numInputs; i++ )
            {
                virtualPointDatabase.insert( m_virtualInDescriptors[i] );
            }
        }

        // Parse Output descriptors
        JsonArray outputs = obj["descriptors"]["outputs"];
        if ( !outputs.isNull() )
        {
            // Check for Mismatch output configuration
            if ( outputs.size() != m_numOutputs )
            {
                m_configErr = true;
                m_configErrString = "Mismatch outputs.  Number of Outputs does not match array of Outputs descriptors";
                return;
            }

            // Create the Input descriptors
            if ( !createDescriptors( m_internalOutDescriptors, outputs, m_numOutputs, "Failed to allocate memory for Virtual Output Descriptors fields." ) )
            {
                return;
            }
            if ( !createDescriptors( m_registerOutDescriptors, outputs, m_numOutputs, "Failed to allocate memory for Register Output Descriptors fields." ) )
            {
                return;
            }
            if ( !createDescriptors( m_virtualOutDescriptors, outputs, m_numOutputs, "Failed to allocate memory for Register Output Descriptors fields." ) )
            {
                return;
            }

            // All Virtual Point Descriptors to the database
            for ( int i=0; i < m_numOutputs; i++ )
            {
                virtualPointDatabase.insert( m_virtualOutDescriptors[i] );
            }
        }
    }
}

bool Digital::createDescriptors( Fxt::Point::Descriptor* descriptors, JsonArray& json, int8_t numDescriptors, const char* errMsg )
{
    // Initialize the descriptor elements
    for ( int i=0; i < numDescriptors; i++ )
    {
        uint32_t    localId       = json[i]["id"];
        const char* name          = json[i]["name"] | "";
        char*       nameMemoryPtr = (char*) m_allocator.allocate( strlen( name ) + 1 );
        if ( nameMemoryPtr == nullptr )
        {
            m_configErr       = true;
            m_configErrString = errMsg;
            return false;
        }
        strcpy( nameMemoryPtr, name );
        descriptors[i].configure( nameMemoryPtr, localId, Cpl::Point::Bool::create, nullptr );
    }

    return true;
}

bool Digital::start() noexcept
{
    // Do NOT start if my configuration failed
    if ( m_configErr )
    {
        return false;
    }

    m_inputVals  = m_initInputVals;
    m_outputVals = m_initOutputVals;
    return Common_::start();
}

bool Digital::stop() noexcept
{
    return Common_::stop();
}

const Cpl::Type::Guid_T& Digital::getGuid() const noexcept
{
}

/// See Fxt::Card::Api
const char* Digital::getTypeName() const noexcept
{
    return "Fxt::Card::HW::Mock::Digital";
}


///////////////////////////////////////////////////////////////////////////////
