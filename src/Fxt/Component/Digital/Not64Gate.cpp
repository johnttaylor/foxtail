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


#include "Not64Gate.h"
#include "Cpl/System/Assert.h"
#include "Fxt/Point/Bool.h"
#include <stdint.h>

///
using namespace Fxt::Component::Digital;


///////////////////////////////////////////////////////////////////////////////
Not64Gate::Not64Gate( JsonVariant&                       componentObject,
                      Cpl::Memory::ContiguousAllocator&  generalAllocator,
                      Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                      Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                      Fxt::Point::DatabaseApi&           dbForPoints )
    : m_numInputs( 0 )
    , m_numOutputs( 0 )
{
    parseConfiguration( generalAllocator, componentObject );
}

Not64Gate::~Not64Gate()
{
    // Nothing required
}

///////////////////////////////////////////////////////////////////////////////
const char* Not64Gate::getTypeGuid() const noexcept
{
    return GUID_STRING;
}

const char* Not64Gate::getTypeName() const noexcept
{
    return TYPE_NAME;
}

bool Not64Gate::parseConfiguration( Cpl::Memory::ContiguousAllocator& generalAllocator, JsonVariant & obj ) noexcept
{
    // Parse Input Point references
    JsonArray inputs = obj["inputs"];
    if ( !inputs.isNull() )
    {
        // Validate number of points
        size_t nInputs = inputs.size();
        if ( nInputs == 0 || nInputs > MAX_INPUTS )
        {
            m_error = fullErr( Err_T::INCORRECT_NUM_INPUT_REFS );
            m_error.logIt( getTypeName() );
            return false;
        }
        m_numInputs = (uint8_t) nInputs;

        // Allocate memory for managing input references
        m_inputRefs = (Fxt::Point::Bool**) generalAllocator.allocate( sizeof( Fxt::Point::Bool* ) * m_numInputs );
        if ( m_inputRefs == nullptr )
        {
            m_error = fullErr( Err_T::OUT_OF_MEMORY );
            m_error.logIt( getTypeName() );
            return false;
        }

        unsigned numInputsFoundNotUsed;
        if ( !parsePointReferences( (size_t*) m_inputRefs,  // Start by storing the point ID
                                    MAX_INPUTS,
                                    inputs,
                                    numInputsFoundNotUsed ) )
        {
            return false;
        }
    }

    // Parse Output Point references
    JsonArray outputs = obj["outputs"];
    if ( !outputs.isNull() )
    {
        // Validate number of points
        size_t nOutputs = outputs.size();
        if ( nOutputs == 0 || nOutputs > MAX_OUTPUTS )
        {
            m_error = fullErr( Err_T::INCORRECT_NUM_OUTPUT_REFS );
            m_error.logIt( getTypeName() );
            return false;
        }
        m_numOutputs = (uint8_t) nOutputs;

        // Allocate memory for managing input references
        m_outputRefs        = (Fxt::Point::Bool**) generalAllocator.allocate( sizeof( Fxt::Point::Bool* ) * m_numOutputs );
        m_outputPassthrough = (bool*) generalAllocator.allocate( sizeof( bool ) * m_numOutputs );
        if ( m_outputRefs == nullptr || m_outputPassthrough == nullptr )
        {
            m_error = fullErr( Err_T::OUT_OF_MEMORY );
            m_error.logIt( getTypeName() );
            return false;
        }

        unsigned numOutputsFoundNotUsed;
        if ( !parsePointReferences( (size_t*) m_outputRefs, // Start by storing the point ID
                                    MAX_OUTPUTS,
                                    outputs,
                                    numOutputsFoundNotUsed ) )
        {
            return false;
        }
    }

    // The number of inputs must match the number of outputs
    if ( m_numInputs != m_numOutputs )
    {

        m_error = fullErr( Err_T::MISMATCHED_INPUTS_OUTPUTS );
        m_error.logIt( "%s. in=%u, out=%u", getTypeName(), m_numInputs, m_numOutputs );
        return false;
    }

    // Parse output negate qualifiers (Note: if negate is TRUE - that means do NOT - NOT the input)
    for ( unsigned i=0; i < m_numOutputs; i++ )
    {
        m_outputPassthrough[i] = outputs[i]["negate"] | false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error Not64Gate::resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept
{
    // Resolve INPUT references
    if ( !Common_::resolveReferences( pointDb,
                                      (Fxt::Point::Api **) m_inputRefs,    // Pass as array of generic Point pointers
                                      m_numInputs ) )
    {
        m_error = fullErr( Err_T::UNRESOLVED_INPUT_REFRENCE );
        m_error.logIt( getTypeName() );
        return m_error;
    }

    // Resolve OUTPUT references
    if ( !Common_::resolveReferences( pointDb,
                                      (Fxt::Point::Api **) m_outputRefs,    // Pass as array of generic Point pointers
                                      m_numOutputs ) )
    {
        m_error = fullErr( Err_T::UNRESOLVED_OUTPUT_REFRENCE );
        m_error.logIt( getTypeName() );
        return m_error;
    }

    // Validate Point types
    if ( Fxt::Point::Api::validatePointTypes( (Fxt::Point::Api **) m_inputRefs, m_numInputs, Fxt::Point::Bool::GUID_STRING ) == false )
    {
        m_error = fullErr( Err_T::INPUT_REFRENCE_BAD_TYPE );
        m_error.logIt( getTypeName() );
        return m_error;
    }
    if ( Fxt::Point::Api::validatePointTypes( (Fxt::Point::Api **) m_outputRefs, m_numOutputs, Fxt::Point::Bool::GUID_STRING ) == false )
    {
        m_error = fullErr( Err_T::OUTPUT_REFRENCE_BAD_TYPE );
        m_error.logIt( getTypeName() );
        return m_error;
    }

    m_error = Fxt::Type::Error::SUCCESS();   // Set my state to 'ready-to-start'
    return m_error;
}

Fxt::Type::Error Not64Gate::execute( int64_t currentTickUsec ) noexcept
{
    // NOTE: The method NEVER fails

    // Read all of my inputs!
    for ( int i=0; i < m_numInputs; i++ )
    {
        bool inVal = true;

        // Set the outputs to invalid if at least one input is invalid
        if ( !m_inputRefs[i]->read( inVal ) )
        {
            // Invalidate the corresponding output
            m_outputRefs[i]->setInvalid();
        }

        // NOT or (NOT NOT) the output
        else
        {
            bool finalOut = m_outputPassthrough[i] ? inVal : !inVal;
            m_outputRefs[i]->write( finalOut );
        }
    }

    return Fxt::Type::Error::SUCCESS();
}