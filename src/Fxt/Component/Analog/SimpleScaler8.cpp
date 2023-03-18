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


#include "SimpleScaler8.h"
#include "Cpl/System/Assert.h"
#include "Fxt/Point/Bool.h"
#include <stdint.h>

///
using namespace Fxt::Component::Analog;


///////////////////////////////////////////////////////////////////////////////
SimpleScaler8::SimpleScaler8( JsonVariant&                       componentObject,
                              Cpl::Memory::ContiguousAllocator&  generalAllocator,
                              Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                              Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                              Fxt::Point::DatabaseApi&           dbForPoints )
    : m_numInputOuputPairs( 0 )
{
    memset( &m_inputRefs, 0, sizeof( m_inputRefs ) );
    memset( &m_outputRefs, 0, sizeof( m_outputRefs ) );

    parseConfiguration( componentObject );
}

SimpleScaler8::~SimpleScaler8()
{
    // Nothing required
}

///////////////////////////////////////////////////////////////////////////////
const char* SimpleScaler8::getTypeGuid() const noexcept
{
    return GUID_STRING;
}

const char* SimpleScaler8::getTypeName() const noexcept
{
    return TYPE_NAME;
}

bool SimpleScaler8::parseConfiguration( JsonVariant & obj ) noexcept
{
    // Parse Input Point references
    JsonArray inputs = obj["inputs"];
    if ( inputs.isNull() )
    {
        m_error = fullErr( Err_T::MISSING_REQUIRED_FIELD );
        m_error.logIt();
        return false;
    }
    else
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

        m_numInputOuputPairs = numInputsFound;
    }

    // Parse Output Point references
    JsonArray outputs = obj["outputs"];
    if ( outputs.isNull() )
    {
        m_error = fullErr( Err_T::MISSING_REQUIRED_FIELD );
        m_error.logIt();
        return false;
    }
    else
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

        if ( numOutputsFound != m_numInputOuputPairs )
        {
            m_error      = fullErr( Err_T::MISMATCHED_INPUTS_OUTPUTS );
            m_error.logIt();
            return false;
        }
    }

    // Parse the Konstants
    for ( unsigned i=0; i < m_numInputOuputPairs; i++ )
    {
        // Validate the json
        if ( inputs[i]["m"].isNull() || inputs[i]["b"].isNull() ||
             inputs[i]["m"].is<float>() == false || inputs[i]["b"].is<float>() == false )
        {
            m_error = fullErr( Err_T::MISSING_REQUIRED_FIELD );
            m_error.logIt();
            return false;
        }

        m_konstants[i].m = inputs[i]["m"];
        m_konstants[i].b = inputs[i]["b"];
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error SimpleScaler8::resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept
{
    // Resolve INPUT references
    if ( !Common_::resolveReferences( pointDb,
                                      (Fxt::Point::Api **) m_inputRefs,    // Pass as array of generic Point pointers
                                      m_numInputOuputPairs ) )
    {
        m_error = fullErr( Err_T::UNRESOLVED_INPUT_REFRENCE );
        m_error.logIt();
        return m_error;
    }

    // Resolve OUTPUT references
    if ( !Common_::resolveReferences( pointDb,
                                      (Fxt::Point::Api **) m_outputRefs,    // Pass as array of generic Point pointers
                                      m_numInputOuputPairs ) )
    {
        m_error = fullErr( Err_T::UNRESOLVED_OUTPUT_REFRENCE );
        m_error.logIt();
        return m_error;
    }

    // Validate Point types
    if ( Fxt::Point::Api::validatePointTypes( (Fxt::Point::Api **) m_inputRefs, m_numInputOuputPairs, Fxt::Point::Float::GUID_STRING ) == false )
    {
        m_error = fullErr( Err_T::INPUT_REFRENCE_BAD_TYPE );
        m_error.logIt();
        return m_error;
    }
    if ( Fxt::Point::Api::validatePointTypes( (Fxt::Point::Api **) m_outputRefs, m_numInputOuputPairs, Fxt::Point::Float::GUID_STRING ) == false )
    {
        m_error = fullErr( Err_T::OUTPUT_REFRENCE_BAD_TYPE );
        m_error.logIt();
        return m_error;
    }

    m_error = Fxt::Type::Error::SUCCESS();   // Set my state to 'ready-to-start'
    return m_error;
}

Fxt::Type::Error SimpleScaler8::execute( int64_t currentTickUsec ) noexcept
{
    // NOTE: The method NEVER fails

    // Scale the inputs
    for ( int i=0; i < m_numInputOuputPairs; i++ )
    {
        // Set the output to invalid when the input is invalid
        float x;
        if ( !m_inputRefs[i]->read( x ) )
        {
            // Invalidate the output
            m_outputRefs[i]->setInvalid();
        }

        // Calculate the output
        else
        {
            float out = m_konstants[i].m * x + m_konstants[i].b;
            m_outputRefs[i]->write( out );
        }
    }

    return Fxt::Type::Error::SUCCESS();
}