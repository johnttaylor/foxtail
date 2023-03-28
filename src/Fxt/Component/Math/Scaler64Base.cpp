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
    : Common_()
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
    for ( unsigned i=0; i < m_numInputs; i++ )
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
bool Scaler64Base::parseConfiguration( Cpl::Memory::ContiguousAllocator& generalAllocator,
                                       JsonVariant&                      obj,
                                       unsigned                          minInputs,
                                       unsigned                          maxInputs,
                                       unsigned                          minOutputs,
                                       unsigned                          maxOutputs ) noexcept
{
    // Parse references
    if ( !parseInputReferences( generalAllocator, obj, minInputs, maxInputs ) ||
         !parseOutputReferences( generalAllocator, obj, minOutputs, maxOutputs ) )
    {
        return false;
    }

    // The number of inputs must match the number of outputs
    if ( m_numInputs != m_numOutputs )
    {
        m_error = fullErr( Err_T::MISMATCHED_INPUTS_OUTPUTS );
        m_error.logIt( "%s. in=%u, out=%u", getTypeName(), m_numInputs, m_numOutputs );
        return false;
    }

    // Allocate memory for internal lists
    if ( allocateKonstants( generalAllocator, m_numInputs ) == false )
    {
        m_error = fullErr( Fxt::Component::Err_T::OUT_OF_MEMORY );
        m_error.logIt( getTypeName() );
        return false;
    }

    // Parse the Konstants
    JsonArray inputs = obj["inputs"];
    for ( unsigned i=0; i < m_numInputs; i++ )
    {
        JsonObject elem = inputs[i];

        // Validate the json
        if ( elem["m"].isNull() || elem["b"].isNull() ||
             !parseJsonKonstants( i, elem ) )
        {
            m_error = fullErr( Err_T::MISSING_REQUIRED_FIELD );
            m_error.logIt( getTypeName() );
            return false;
        }
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error Scaler64Base::resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept
{
    // Resolve references
    if ( resolveInputOutputReferences( pointDb ) )
    {
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
