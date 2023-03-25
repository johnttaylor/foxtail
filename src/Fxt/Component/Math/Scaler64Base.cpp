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


#include "Scaler64Base.h"
#include "Cpl/System/Assert.h"
#include "Fxt/Point/Bool.h"
#include <stdint.h>

///
using namespace Fxt::Component::Math;


///////////////////////////////////////////////////////////////////////////////
Scaler64Base::Scaler64Base()
    : m_numInputOuputPairs( 0 )
{
    // Child class does all of the work
}

Scaler64Base::~Scaler64Base()
{
    // Nothing required
}


///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error Scaler64Base::execute( int64_t currentTickUsec ) noexcept
{
    // NOTE: The method NEVER fails

    // Scale the inputs
    for ( int i=0; i < m_numInputOuputPairs; i++ )
    {
        if ( !calculateOutput( i ) )
        {
            // Set the output to invalid when the input is invalid
            m_outputRefs[i]->setInvalid();
        }
    }

    return Fxt::Type::Error::SUCCESS();
}

///////////////////////////////////////////////////////////////////////////////
bool Scaler64Base::parseConfiguration( Cpl::Memory::ContiguousAllocator& generalAllocator, JsonVariant & obj ) noexcept
{
    // Input Point references
    JsonArray inputs = obj["inputs"];
    if ( inputs.isNull() )
    {
        m_error = fullErr( Err_T::MISSING_REQUIRED_FIELD );
        m_error.logIt();
        return false;
    }

    // Validate number of points
    size_t nInputs = inputs.size();
    if ( nInputs == 0 || nInputs > MAX_INPUTS )
    {
        m_error = fullErr( Fxt::Component::Err_T::INCORRECT_NUM_INPUT_REFS );
        m_error.logIt( getTypeName() );
        return false;
    }
    m_numInputOuputPairs = (uint8_t) nInputs;

    // Allocate internal memory
    m_inputRefs  = (Fxt::Point::Api**) generalAllocator.allocate( sizeof( Fxt::Point::Bool* ) * m_numInputOuputPairs );
    m_outputRefs = (Fxt::Point::Api**) generalAllocator.allocate( sizeof( Fxt::Point::Bool* ) * m_numInputOuputPairs );
    if ( allocateKonstants( generalAllocator, m_numInputOuputPairs ) == false || 
         m_inputRefs == nullptr ||
         m_outputRefs == nullptr )
    {
        m_error = fullErr( Fxt::Component::Err_T::OUT_OF_MEMORY );
        m_error.logIt( getTypeName() );
        return false;
    }

    // Parse Input point references
    unsigned numFoundNotUsed;
    if ( !parsePointReferences( (size_t*) m_inputRefs,  // Start by storing the point ID
                                MAX_OUTPUTS,
                                inputs,
                                numFoundNotUsed ) )
    {
        return false;
    }

    // Output Point references
    JsonArray outputs = obj["outputs"];
    if ( inputs.isNull() )
    {
        m_error = fullErr( Err_T::MISSING_REQUIRED_FIELD );
        m_error.logIt();
        return false;
    }

    // Validate number of points
    size_t nOutputs = outputs.size();
    if ( nOutputs == 0 || nOutputs > MAX_OUTPUTS )
    {
        m_error = fullErr( Fxt::Component::Err_T::INCORRECT_NUM_OUTPUT_REFS );
        m_error.logIt( getTypeName() );
        return false;
    }
    if ( nOutputs != m_numInputOuputPairs )
    {
        m_error = fullErr( Err_T::MISMATCHED_INPUTS_OUTPUTS );
        m_error.logIt();
        return false;
    }

    // Parse Output Point references
    if ( !parsePointReferences( (size_t*) m_outputRefs, // Start by storing the point ID
                                MAX_OUTPUTS,
                                outputs,
                                numFoundNotUsed ) )
    {
        return false;
    }


    // Parse the Konstants
    for ( unsigned i=0; i < m_numInputOuputPairs; i++ )
    {
        JsonObject elem = inputs[i];
        // Validate the json
        if ( elem["m"].isNull() || elem["b"].isNull() ||
             !parseJsonKonstants( i, elem ) )
        {
            m_error = fullErr( Err_T::MISSING_REQUIRED_FIELD );
            m_error.logIt();
            return false;
        }
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error Scaler64Base::resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept
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
    if ( !validateInputTypes() )
    {
        m_error = fullErr( Err_T::INPUT_REFRENCE_BAD_TYPE );
        m_error.logIt( getTypeName() );
        return m_error;
    }
    if ( !validateOututTypes() )
    {
        m_error = fullErr( Err_T::OUTPUT_REFRENCE_BAD_TYPE );
        m_error.logIt( getTypeName() );
        return m_error;
    }


    m_error = Fxt::Type::Error::SUCCESS();   // Set my state to 'ready-to-start'
    return m_error;
}
