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


#include "Wire64Base.h"
#include "Cpl/System/Assert.h"
#include <stdint.h>

///
using namespace Fxt::Component::Basic;


///////////////////////////////////////////////////////////////////////////////
Wire64Base::Wire64Base()
    : Common_()
{
    // Child class does all of the work
}

Wire64Base::~Wire64Base()
{
    // Nothing required
}


///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error Wire64Base::execute( int64_t currentTickUsec ) noexcept
{
    // NOTE: The method NEVER fails

    // Copy the inputs
    for ( unsigned i=0; i < m_numInputs; i++ )
    {
        copyPoint( i );
    }

    return Fxt::Type::Error::SUCCESS();
}

///////////////////////////////////////////////////////////////////////////////
bool Wire64Base::parseConfiguration( Cpl::Memory::ContiguousAllocator& generalAllocator,
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

    return true;
}


///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error Wire64Base::resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept
{
    // Resolve references
    if ( resolveInputOutputReferences( pointDb ) )
    {
        return m_error;
    }

    // Validate Point types
    if ( !Fxt::Point::Api::validatePointTypes( m_inputRefs, m_numInputs, getPointTypeGuid() ) )
    {
        m_error = fullErr( Err_T::INPUT_REFRENCE_BAD_TYPE );
        m_error.logIt( getTypeName() );
        return m_error;
    }
    if ( !Fxt::Point::Api::validatePointTypes( m_outputRefs, m_numOutputs, getPointTypeGuid() ) )
    {
        m_error = fullErr( Err_T::OUTPUT_REFRENCE_BAD_TYPE );
        m_error.logIt( getTypeName() );
        return m_error;
    }

    m_error = Fxt::Type::Error::SUCCESS();   // Set my state to 'ready-to-start'
    return m_error;
}
