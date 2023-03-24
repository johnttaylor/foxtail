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


#include "Mux.h"
#include "Error.h"
#include "Fxt/Component/Error.h"
#include "Cpl/System/Assert.h"

#include <stdint.h>

///
using namespace Fxt::Component::Digital;

///////////////////////////////////////////////////////////////////////////////
Mux::Mux( JsonVariant&                       componentObject,
          Cpl::Memory::ContiguousAllocator&  generalAllocator,
          Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
          Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
          Fxt::Point::DatabaseApi&           dbForPoints )
    : m_numInputs( 0 )
    , m_numOutputs( 0 )
{
    parseConfiguration( generalAllocator, componentObject );
}

Mux::~Mux()
{
    // Nothing required
}

///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error Mux::execute( int64_t currentTickUsec ) noexcept
{
    // NOTE: The method NEVER fails

    // Walk all of my inputs
    uint64_t outValue = 0;
    for ( int i=0; i < m_numInputs; i++ )
    {
        // Get the input bit
        bool inBitValue;
        if ( !m_inputRefs[i]->read( inBitValue ) )
        {
            // Set the output to invalid if any input is invalid
            m_outputRefs[0]->setInvalid();
            return Fxt::Type::Error::SUCCESS();
        }

        inBitValue        = m_inputNegated[i] ? !inBitValue : inBitValue;
        uint64_t bitMask  = ((uint64_t)inBitValue) << m_bitOffsets[i];
        outValue         |= bitMask;
    }

    // Update the output value
    (m_outAttributesPtr->writeFunc)( m_outputRefs[0], outValue );
    return Fxt::Type::Error::SUCCESS();
}

///////////////////////////////////////////////////////////////////////////////
const char* Mux::getTypeGuid() const noexcept
{
    return GUID_STRING;
}

const char* Mux::getTypeName() const noexcept
{
    return TYPE_NAME;
}

bool Mux::parseConfiguration( Cpl::Memory::ContiguousAllocator& generalAllocator, JsonVariant & obj ) noexcept
{
    // Parse Output Point references
    JsonArray outputs = obj["outputs"];
    if ( !outputs.isNull() )
    {
        // Validate number of points
        size_t nOutputs = outputs.size();
        if ( nOutputs == 0 || nOutputs > MAX_OUTPUTS )
        {
            m_error = fullErr( Fxt::Component::Err_T::INCORRECT_NUM_OUTPUT_REFS );
            m_error.logIt( getTypeName() );
            return false;
        }
        m_numOutputs = (uint8_t) nOutputs;

        // Allocate memory for managing input references
        m_outputRefs = (Fxt::Point::Api**) generalAllocator.allocate( sizeof( Fxt::Point::Bool* ) * m_numOutputs );
        if ( m_outputRefs == nullptr )
        {
            m_error = fullErr( Fxt::Component::Err_T::OUT_OF_MEMORY );
            m_error.logIt( getTypeName() );
            return false;
        }

        unsigned numOutputsFoundNotUsed;
        if ( !parsePointReferences( (size_t*) m_outputRefs, // Start by storing the point ID
                                    MAX_OUTPUTS,
                                    outputs,
                                    fullErr( Fxt::Component::Err_T::TOO_MANY_OUTPUT_REFS ),
                                    fullErr( Fxt::Component::Err_T::BAD_OUTPUT_REFERENCE ),
                                    numOutputsFoundNotUsed ) )
        {
            return false;
        }

        // Get output type
        const char* guid = outputs[0]["type"];
        if ( guid == nullptr )
        {
            m_error = fullErr( Fxt::Component::Err_T::OUTPUT_REFRENCE_BAD_TYPE );
            m_error.logIt( getTypeName() );
            return false;
        }

        // Validate Output size
        m_outAttributesPtr = Fxt::Point::NumericHandlers::getIntegerPointAttributes( guid );
        if ( m_outAttributesPtr == nullptr )
        {
            m_error = fullErr( Err_T::MUX_INVALID_OUTPUT_TYPE );
            m_error.logIt( getTypeName() );
            return false;
        }

        // Validate that the input bits do NOT exceed the output bits
        if ( m_numInputs > m_outAttributesPtr->numBits )
        {
            m_error = fullErr( Err_T::MUX_INPUT_BITS_EXCEED_OUTPUT );
            m_error.logIt( getTypeName() );
            return false;
        }
    }

    // Parse Input Point references
    JsonArray inputs = obj["inputs"];
    if ( !inputs.isNull() )
    {
        // Validate number of points
        size_t nInputs = inputs.size();
        if ( nInputs == 0 || nInputs > MAX_INPUTS )
        {
            m_error = fullErr( Fxt::Component::Err_T::INCORRECT_NUM_INPUT_REFS );
            m_error.logIt( getTypeName() );
            return false;
        }
        m_numInputs = (uint8_t)nInputs;

        // Allocate memory for managing input references
        m_inputRefs    = (Fxt::Point::Bool**) generalAllocator.allocate( sizeof( Fxt::Point::Bool* ) * m_numInputs );
        m_bitOffsets   = (uint8_t*) generalAllocator.allocate( sizeof( uint8_t ) * m_numInputs );
        m_inputNegated = (bool*) generalAllocator.allocate( sizeof( bool ) * m_numInputs );
        if ( m_inputRefs == nullptr || m_bitOffsets == nullptr || m_inputNegated == nullptr )
        {
            m_error = fullErr( Fxt::Component::Err_T::OUT_OF_MEMORY );
            m_error.logIt( getTypeName() );
            return false;
        }

        unsigned numInputsFoundNotUsed;
        if ( !parsePointReferences( (size_t*) m_inputRefs,  // Start by storing the point ID
                                    MAX_INPUTS,
                                    inputs,
                                    fullErr( Fxt::Component::Err_T::TOO_MANY_INPUT_REFS ),
                                    fullErr( Fxt::Component::Err_T::BAD_INPUT_REFERENCE ),
                                    numInputsFoundNotUsed ) )
        {
            return false;
        }
    }

    // Parse input bit offsets and negate qualifiers
    for ( unsigned i=0; i < m_numInputs; i++ )
    {
        m_inputNegated[i] = inputs[i]["negate"] | false;
        m_bitOffsets[i]   = inputs[i]["bit"] | (m_outAttributesPtr->numBits);
        if ( m_bitOffsets[i] >= m_outAttributesPtr->numBits )
        {
            m_error = fullErr( Err_T::MUX_INVALID_BIT_OFFSET );
            m_error.logIt();
            return false;
        }
    }


    return true;
}


///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error Mux::resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept
{
    // Resolve INPUT references
    if ( !Common_::resolveReferences( pointDb,
                                      (Fxt::Point::Api **) m_inputRefs,    // Pass as array of generic Point pointers
                                      m_numInputs ) )
    {
        m_error = fullErr( Fxt::Component::Err_T::UNRESOLVED_INPUT_REFRENCE );
        m_error.logIt( getTypeName() );
        return m_error;
    }

    // Resolve OUTPUT references
    if ( !Common_::resolveReferences( pointDb,
                                      (Fxt::Point::Api **) m_outputRefs,    // Pass as array of generic Point pointers
                                      m_numOutputs ) )
    {
        m_error = fullErr( Fxt::Component::Err_T::UNRESOLVED_OUTPUT_REFRENCE );
        m_error.logIt( getTypeName() );
        return m_error;
    }

    // Validate Point types (note: Output type has already been validated)
    if ( Fxt::Point::Api::validatePointTypes( (Fxt::Point::Api **) m_inputRefs, m_numInputs, Fxt::Point::Bool::GUID_STRING ) == false )
    {
        m_error = fullErr( Fxt::Component::Err_T::INPUT_REFRENCE_BAD_TYPE );
        m_error.logIt( getTypeName() );
        return m_error;
    }

    m_error = fullErr( Err_T::SUCCESS );   // Set my state to 'ready-to-start'
    return m_error;
}

