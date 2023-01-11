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


#include "ByteMux.h"
#include "Error.h"
#include "Cpl/System/Assert.h"
#include <stdint.h>

///
using namespace Fxt::Component::Digital;

#define MAX_BIT_OFFSET      7

///////////////////////////////////////////////////////////////////////////////
ByteMux::ByteMux( JsonVariant&                       componentObject,
                  Cpl::Memory::ContiguousAllocator&  generalAllocator,
                  Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                  Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                  Fxt::Point::DatabaseApi&           dbForPoints )
    : m_numInputs( 0 )
    , m_numOutputs( 0 )
{
    memset( &m_inputRefs, 0, sizeof( m_inputRefs ) );
    memset( &m_outputRefs, 0, sizeof( m_outputRefs ) );
    memset( &m_inputNegated, 0, sizeof( m_inputNegated ) );

    parseConfiguration( componentObject );
}

ByteMux::~ByteMux()
{
    // Nothing required
}

///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error ByteMux::execute( int64_t currentTickUsec ) noexcept
{
    // NOTE: The method NEVER fails

    // Walk all of my inputs
    uint8_t outValue = 0;
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
        uint8_t bitMask   = (inBitValue ? 1 : 0) << m_bitOffsets[i];
        outValue         |= bitMask;
    }

    // Update the output value
    m_outputRefs[0]->write( outValue );
    return Fxt::Type::Error::SUCCESS();
}

///////////////////////////////////////////////////////////////////////////////
const char* ByteMux::getTypeGuid() const noexcept
{
    return GUID_STRING;
}

const char* ByteMux::getTypeName() const noexcept
{
    return TYPE_NAME;
}

bool ByteMux::parseConfiguration( JsonVariant & obj ) noexcept
{
    // Parse Input Point references
    JsonArray inputs = obj["inputs"];
    if ( !inputs.isNull() )
    {
        unsigned numInputsFound;
        if ( !parsePointReferences( (size_t*) m_inputRefs,  // Start by storing the point ID
                                    MAX_INPUTS,
                                    inputs,
                                    fullErr( Fxt::Component::Err_T::TOO_MANY_INPUT_REFS ),
                                    fullErr( Fxt::Component::Err_T::BAD_INPUT_REFERENCE ),
                                    numInputsFound ) )
        {
            return false;
        }

        m_numInputs = numInputsFound;
    }

    // Parse input bit offsets and negate qualifiers
    for ( unsigned i=0; i < inputs.size(); i++ )
    {
        m_inputNegated[i] = inputs[i]["negate"] | false;
        m_bitOffsets[i]   = inputs[i]["bit"] | (MAX_BIT_OFFSET + 1);
        if ( m_bitOffsets[i] > MAX_BIT_OFFSET )
        {
            m_error = fullErr( Err_T::MUX_INVALID_BIT_OFFSET );
            return false;
        }
    }

    // Parse Output Point references
    JsonArray outputs = obj["outputs"];
    if ( !outputs.isNull() )
    {
        unsigned numOutputsFound;
        if ( !parsePointReferences( (size_t*) m_outputRefs, // Start by storing the point ID
                                    MAX_OUTPUTS,
                                    outputs,
                                    fullErr( Fxt::Component::Err_T::TOO_MANY_OUTPUT_REFS ),
                                    fullErr( Fxt::Component::Err_T::BAD_OUTPUT_REFERENCE ),
                                    numOutputsFound ) )
        {
            return false;
        }

        m_numOutputs = numOutputsFound;
    }


    return true;
}

///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error ByteMux::resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept
{
    // Resolve INPUT references
    if ( !Common_::resolveReferences( pointDb,
                                      (Fxt::Point::Api **) m_inputRefs,    // Pass as array of generic Point pointers
                                      m_numInputs ) )
    {
        m_error = fullErr( Fxt::Component::Err_T::UNRESOLVED_INPUT_REFRENCE );
        return m_error;
    }

    // Resolve OUTPUT references
    if ( !Common_::resolveReferences( pointDb,
                                      (Fxt::Point::Api **) m_outputRefs,    // Pass as array of generic Point pointers
                                      m_numOutputs ) )
    {
        m_error = fullErr( Fxt::Component::Err_T::UNRESOLVED_OUTPUT_REFRENCE );
        return m_error;
    }

    // Validate Point types
    if ( validatePointTypes( (Fxt::Point::Api **) m_inputRefs, m_numInputs, Fxt::Point::Bool::GUID_STRING ) == false )
    {
        m_error = fullErr( Fxt::Component::Err_T::INPUT_REFRENCE_BAD_TYPE );
        return m_error;
    }
    if ( validatePointTypes( (Fxt::Point::Api **) m_outputRefs, m_numOutputs, Fxt::Point::Uint8::GUID_STRING ) == false )
    {
        m_error = fullErr( Fxt::Component::Err_T::OUTPUT_REFRENCE_BAD_TYPE );
        return m_error;
    }

    m_error = fullErr( Err_T::SUCCESS );   // Set my state to 'ready-to-start'
    return m_error;
}

