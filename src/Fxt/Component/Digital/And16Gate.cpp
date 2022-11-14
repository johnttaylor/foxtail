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


#include "And16Gate.h"
#include "Cpl/System/Assert.h"
#include "Fxt/Point/Bool.h"
#include <stdint.h>
#include <new>

///
using namespace Fxt::Component::Digital;


///////////////////////////////////////////////////////////////////////////////
And16Gate::And16Gate( uint16_t     exeOrder,
                      JsonVariant& componentObject )
    : Fxt::Component::Common_( exeOrder )
    , m_numInputs( 0 )
    , m_numOutputs( 0 )
{
    memset( &m_inputRefs, 0, sizeof( m_inputRefs ) );
    memset( &m_outputRefs, 0, sizeof( m_outputRefs ) );
    memset( &m_outputNegated, 0, sizeof( m_outputNegated ) );

    parseConfiguration( componentObject );
}

And16Gate::~And16Gate()
{
    // Nothing required
}

///////////////////////////////////////////////////////////////////////////////
const char* And16Gate::getTypeGuid() const noexcept
{
    return GUID_STRING;
}

const char* And16Gate::getTypeName() const noexcept
{
    return TYPE_NAME;
}

bool And16Gate::parseConfiguration( JsonVariant & obj ) noexcept
{
    // Parse Input Point references
    JsonArray inputs = obj["inputs"];
    if ( !inputs.isNull() )
    {
        unsigned numInputsFound;
        if ( !parsePointReferences( (size_t*) m_inputRefs,  // Start by storing the point ID
                                    MAX_INPUTS,
                                    inputs,
                                    FXT_COMPONENT_ERR_TOO_MANY_INPUT_REFS,
                                    FXT_COMPONENT_ERR_BAD_INPUT_REFERENCE,
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
                                    MAX_INPUTS,
                                    outputs,
                                    FXT_COMPONENT_ERR_TOO_MANY_INPUT_REFS,
                                    FXT_COMPONENT_ERR_BAD_INPUT_REFERENCE,
                                    numOutputsFound ) )
        {
            return false;
        }

        m_numOutputs = numOutputsFound;
    }

    // Parse output negate qualifiers
    for ( int i=0; i < outputs.size(); i++ )
    {
        m_outputNegated[i] = outputs[i]["negate"] | false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool And16Gate::resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept
{
    // Resolve INPUT references
    if ( !Common_::resolveReferences( pointDb,
                                      (Fxt::Point::Api **) m_inputRefs,    // Pass as array of generic Point pointers
                                      m_numInputs ) )
    {
        m_error = FXT_COMPONENT_ERR_UNRESOLVED_INPUT_REFRENCE;
        return false;
    }

    // Resolve OUTPUT references
    if ( !Common_::resolveReferences( pointDb,
                                      (Fxt::Point::Api **) m_outputRefs,    // Pass as array of generic Point pointers
                                      m_numOutputs ) )
    {
        m_error = FXT_COMPONENT_ERR_UNRESOLVED_OUTPUT_REFRENCE;
        return false;
    }

    return true;
}

bool And16Gate::execute( int64_t currentTickUsec, Err_T& componentErrorCode ) noexcept
{
    // NOTE: The method NEVER fails

    // Read all of my inputs!
    bool outputVal = true;
    for ( int i=0; i < m_numInputs && outputVal; i++ )
    {
        // Set the outputs to invalid if at least one input is invalid
        if ( !m_inputRefs[i]->read( outputVal ) )
        {
            // Invalidate the outputs
            for ( int i=0; i < m_numOutputs; i++ )
            {
                m_outputRefs[i]->setInvalid();
            }
            return true;
        }
    }

    // If I get here all of the inputs have valid values -->generate output signals
    for ( int i=0; i < m_numOutputs; i++ )
    {
        bool finalOut = m_outputNegated[i] ? !outputVal : outputVal;
        m_outputRefs[i]->write( finalOut );
    }

    return true;
}
