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
And16Gate::And16Gate( JsonVariant&                       componentObject,
                      Cpl::Memory::ContiguousAllocator&  generalAllocator,
                      Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                      Fxt::Point::BankApi&               statePointBank,
                      Fxt::Point::DatabaseApi&           dbForPoints )
    : m_numInputs( 0 )
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
                                    fullErr( Err_T::TOO_MANY_INPUT_REFS ),
                                    fullErr( Err_T::BAD_INPUT_REFERENCE ),
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
                                    fullErr( Err_T::TOO_MANY_INPUT_REFS ),
                                    fullErr( Err_T::BAD_INPUT_REFERENCE ),
                                    numOutputsFound ) )
        {
            return false;
        }

        m_numOutputs = numOutputsFound;
    }

    // Parse output negate qualifiers
    for ( unsigned i=0; i < outputs.size(); i++ )
    {
        m_outputNegated[i] = outputs[i]["negate"] | false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error And16Gate::resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept
{
    // Resolve INPUT references
    if ( !Common_::resolveReferences( pointDb,
                                      (Fxt::Point::Api **) m_inputRefs,    // Pass as array of generic Point pointers
                                      m_numInputs ) )
    {
        m_error = fullErr( Err_T::UNRESOLVED_INPUT_REFRENCE );
        return m_error;
    }

    // Resolve OUTPUT references
    if ( !Common_::resolveReferences( pointDb,
                                      (Fxt::Point::Api **) m_outputRefs,    // Pass as array of generic Point pointers
                                      m_numOutputs ) )
    {
        m_error = fullErr( Err_T::UNRESOLVED_OUTPUT_REFRENCE );
        return m_error;
    }

    // Validate Point types
    if ( validatePointTypes( (Fxt::Point::Api **) m_inputRefs, m_numInputs ) == false )
    {
        m_error = fullErr( Err_T::INPUT_REFRENCE_BAD_TYPE );
        return m_error;
    }
    if ( validatePointTypes( (Fxt::Point::Api **) m_outputRefs, m_numOutputs ) == false )
    {
        m_error = fullErr( Err_T::OUTPUT_REFRENCE_BAD_TYPE );
        return m_error;
    }

    m_error = Fxt::Type::Error::SUCCESS();   // Set my state to 'ready-to-start'
    return m_error;
}

bool And16Gate::validatePointTypes( Fxt::Point::Api* arrayOfPoints[], uint8_t numPoints )
{
    for ( uint8_t i=0; i < numPoints; i++ )
    {
        if ( arrayOfPoints[i] != nullptr )
        {
            if ( strcmp( arrayOfPoints[i]->getTypeGuid(), Fxt::Point::Bool::GUID_STRING ) != 0 )
            {
                return false;
            }
        }
    }

    return true;
}

Fxt::Type::Error And16Gate::execute( int64_t currentTickUsec ) noexcept
{
    // NOTE: The method NEVER fails

    // Read all of my inputs!
    bool outputVal = true;
    for ( int i=0; i < m_numInputs; i++ )
    {
        bool temp = true;

        // Set the outputs to invalid if at least one input is invalid
        if ( !m_inputRefs[i]->read( temp ) )
        {
            // Invalidate the outputs
            for ( int i=0; i < m_numOutputs; i++ )
            {
                m_outputRefs[i]->setInvalid();
            }
            return Fxt::Type::Error::SUCCESS();
        }

        // AND the individual inputs
        outputVal &= temp;
    }

    // If I get here all of the inputs have valid values -->generate output signals
    for ( int i=0; i < m_numOutputs; i++ )
    {
        bool finalOut = m_outputNegated[i] ? !outputVal : outputVal;
        m_outputRefs[i]->write( finalOut );
    }

    return Fxt::Type::Error::SUCCESS();
}