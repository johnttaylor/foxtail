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
    : Common_()
{
    parseConfiguration( generalAllocator, componentObject, 1, MAX_INPUTS, 1, MAX_OUTPUTS );
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

bool Not64Gate::parseConfiguration( Cpl::Memory::ContiguousAllocator& generalAllocator,
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
    m_outputPassthrough = (bool*) generalAllocator.allocate( sizeof( bool ) * m_numOutputs );
    if ( m_outputRefs == nullptr || m_outputPassthrough == nullptr )
    {
        m_error = fullErr( Err_T::OUT_OF_MEMORY );
        m_error.logIt( getTypeName() );
        return false;
    }

    // Parse output negate qualifiers (Note: if negate is TRUE - that means do NOT - NOT the input)
    JsonArray outputs = obj["outputs"];
    for ( unsigned i=0; i < m_numOutputs; i++ )
    {
        m_outputPassthrough[i] = outputs[i]["negate"] | false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error Not64Gate::resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept
{
    // Resolve references
    if ( resolveInputOutputReferences( pointDb ) )
    {
        return m_error;
    }

    // Validate Point types
    if ( Fxt::Point::Api::validatePointTypes( m_inputRefs, m_numInputs, Fxt::Point::Bool::GUID_STRING ) == false )
    {
        m_error = fullErr( Err_T::INPUT_REFRENCE_BAD_TYPE );
        m_error.logIt( getTypeName() );
        return m_error;
    }
    if ( Fxt::Point::Api::validatePointTypes( m_outputRefs, m_numOutputs, Fxt::Point::Bool::GUID_STRING ) == false )
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
    for ( unsigned i=0; i < m_numInputs; i++ )
    {
        bool inVal = true;

        // Set the outputs to invalid if at least one input is invalid
        Fxt::Point::Bool* inPt = (Fxt::Point::Bool*) m_inputRefs[i];
        if ( !inPt->read( inVal ) )
        {
            // Invalidate the corresponding output
            m_outputRefs[i]->setInvalid();
        }

        // NOT or (NOT NOT) the output
        else
        {
            bool finalOut = m_outputPassthrough[i] ? inVal : !inVal;
           
            Fxt::Point::Bool* outPt = (Fxt::Point::Bool*) m_outputRefs[i];
            outPt->write( finalOut );
        }
    }

    return Fxt::Type::Error::SUCCESS();
}