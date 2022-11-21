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


#include "ByteSplitter.h"
#include "Error.h"
#include "Cpl/System/Assert.h"
#include <stdint.h>
#include <new>

///
using namespace Fxt::Component::Digital;

#define MAX_BIT_OFFSET      7

///////////////////////////////////////////////////////////////////////////////
ByteSplitter::ByteSplitter( JsonVariant&                       componentObject,
                            Cpl::Memory::ContiguousAllocator&  generalAllocator,
                            Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                            Fxt::Point::DatabaseApi&           dbForPoints )
    : m_numInputs( 0 )
    , m_numOutputs( 0 )
{
    memset( &m_inputRefs, 0, sizeof( m_inputRefs ) );
    memset( &m_outputRefs, 0, sizeof( m_outputRefs ) );
    memset( &m_outputNegated, 0, sizeof( m_outputNegated ) );

    parseConfiguration( componentObject );
}

ByteSplitter::~ByteSplitter()
{
    // Nothing required
}

///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error ByteSplitter::execute( int64_t currentTickUsec ) noexcept
{
    // NOTE: The method NEVER fails

    // Get my input
    uint8_t inValue;
    if ( !m_inputRefs[0]->read( inValue ) )
    {
        // Set the outputs to invalid if at least one input is invalid
        for ( int i=0; i < m_numOutputs; i++ )
        {
            m_outputRefs[i]->setInvalid();
        }
        return fullErr( Err_T::SUCCESS );
    }

    // Derive my outputs
    for ( int i=0; i < m_numOutputs; i++ )
    {
        uint8_t bitMask   = 1 << m_bitOffsets[i];
        bool    outputVal = bitMask & inValue;
        bool    finalOut  = m_outputNegated[i] ? !outputVal : outputVal;
        m_outputRefs[i]->write( finalOut );
    }

    return fullErr( Err_T::SUCCESS );
}

///////////////////////////////////////////////////////////////////////////////
const char* ByteSplitter::getTypeGuid() const noexcept
{
    return GUID_STRING;
}

const char* ByteSplitter::getTypeName() const noexcept
{
    return TYPE_NAME;
}

bool ByteSplitter::parseConfiguration( JsonVariant & obj ) noexcept
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

    // Parse output bit offsets and negate qualifiers
    for ( unsigned i=0; i < outputs.size(); i++ )
    {
        m_outputNegated[i] = outputs[i]["negate"] | false;
        m_bitOffsets[i]    = outputs[i]["bit"] | (MAX_BIT_OFFSET+1);
        if ( m_bitOffsets[i] > MAX_BIT_OFFSET )
        {
            m_error = fullErr( Err_T::SPLITTER_INVALID_BIT_OFFSET );
            return false;
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error ByteSplitter::resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept
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
    if ( validatePointTypes( (Fxt::Point::Api **) m_inputRefs, m_numInputs, Fxt::Point::Uint8::GUID_STRING) == false )
    {
        m_error = fullErr( Fxt::Component::Err_T::INPUT_REFRENCE_BAD_TYPE );
        return m_error;
    }
    if ( validatePointTypes( (Fxt::Point::Api **) m_outputRefs, m_numOutputs, Fxt::Point::Bool::GUID_STRING ) == false )
    {
        m_error = fullErr( Fxt::Component::Err_T::OUTPUT_REFRENCE_BAD_TYPE );
        return m_error;
    }

    m_error = fullErr( Err_T::SUCCESS);   // Set my state to 'ready-to-start'
    return m_error;
}

bool ByteSplitter::validatePointTypes( Fxt::Point::Api* arrayOfPoints[], uint8_t numPoints, const char* expectedGUID )
{
    for ( uint8_t i=0; i < numPoints; i++ )
    {
        if ( arrayOfPoints[i] != nullptr )
        {
            if ( strcmp( arrayOfPoints[i]->getTypeGuid(), expectedGUID ) != 0 )
            {
                return false;
            }
        }
    }

    return true;
}

