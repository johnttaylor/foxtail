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


#include "AndGateBase.h"
#include "Cpl/System/Assert.h"
#include "Fxt/Point/Bool.h"
#include <stdint.h>

///
using namespace Fxt::Component::Digital;


///////////////////////////////////////////////////////////////////////////////
AndGateBase::AndGateBase()
    : Common_()
{
    // All the work is done in the child class's constructor
}

AndGateBase::~AndGateBase()
{
    // Nothing required
}

bool AndGateBase::parseConfiguration( Cpl::Memory::ContiguousAllocator& generalAllocator,
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

    // Allocate memory for internal lists
    m_outputNegated = (bool*) generalAllocator.allocate( sizeof( bool ) * m_numOutputs );
    if ( m_outputNegated == nullptr )
    {
        m_error = fullErr( Err_T::OUT_OF_MEMORY );
        m_error.logIt( getTypeName() );
        return false;
    }

    // Parse output negate qualifiers
    JsonArray outputs = obj["outputs"];
    for ( unsigned i=0; i < m_numOutputs; i++ )
    {
        m_outputNegated[i] = outputs[i]["negate"] | false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error AndGateBase::resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept
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

Fxt::Type::Error AndGateBase::execute( int64_t currentTickUsec ) noexcept
{
    // NOTE: The method NEVER fails

    // Read all of my inputs!  
    bool outputVal = true;
    for ( unsigned i=0; i < m_numInputs; i++ )
    {
        bool temp = true;

        // Set the outputs to invalid if at least one input is invalid
        Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_inputRefs[i];
        if ( !pt->read( temp ) )
        {
            // Invalidate the outputs
            for ( unsigned i=0; i < m_numOutputs; i++ )
            {
                m_outputRefs[i]->setInvalid();
            }
            return Fxt::Type::Error::SUCCESS();
        }

        // AND the individual inputs
        outputVal &= temp;
    }

    // If I get here all of the inputs have valid values -->generate output signals
    for ( unsigned i=0; i < m_numOutputs; i++ )
    {
        bool finalOut = m_outputNegated[i] ? !outputVal : outputVal;
  
        Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_outputRefs[i];
        pt->write( finalOut );
    }

    return Fxt::Type::Error::SUCCESS();
}