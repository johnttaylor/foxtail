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


#include "MuxBase.h"
#include "Error.h"
#include "Fxt/Component/Error.h"
#include "Cpl/System/Assert.h"

#include <stdint.h>

///
using namespace Fxt::Component::Digital;

///////////////////////////////////////////////////////////////////////////////
MuxBase::MuxBase()
    : Common_()
{
    // All of the work is done in the child class
}

MuxBase::~MuxBase()
{
    // Nothing required
}

///////////////////////////////////////////////////////////////////////////////
bool MuxBase::getInputBit( unsigned refIdx, bool& dstBitValue )
{
    // Get the input bit
    bool inBitValue;
    Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_inputRefs[refIdx];
    if ( !pt->read( inBitValue ) )
    {
        // Set the output to invalid if any input is invalid
        m_outputRefs[0]->setInvalid();
        return false;
    }

    dstBitValue = m_inputNegated[refIdx] ? !inBitValue : inBitValue;
    return true;
}


///////////////////////////////////////////////////////////////////////////////
bool MuxBase::parseConfiguration( Cpl::Memory::ContiguousAllocator& generalAllocator,
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
    m_bitOffsets   = (uint8_t*) generalAllocator.allocate( sizeof( uint8_t ) * m_numInputs );
    m_inputNegated = (bool*) generalAllocator.allocate( sizeof( bool ) * m_numInputs );
    if ( m_bitOffsets == nullptr || m_inputNegated == nullptr )
    {
        m_error = fullErr( Fxt::Component::Err_T::OUT_OF_MEMORY );
        m_error.logIt( getTypeName() );
        return false;
    }

    // Parse input bit offsets and negate qualifiers
    JsonArray inputs = obj["inputs"];
    for ( unsigned i=0; i < m_numInputs; i++ )
    {
        m_inputNegated[i] = inputs[i]["negate"] | false;
        m_bitOffsets[i]   = inputs[i]["bit"] | maxInputs;
        if ( m_bitOffsets[i] >= maxInputs )
        {
            m_error = fullErr( Err_T::MUX_INVALID_BIT_OFFSET );
            m_error.logIt();
            return false;
        }
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error MuxBase::resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept
{
    // Resolve references
    if ( resolveInputOutputReferences( pointDb ) )
    {
        return m_error;
    }

    // Validate INPUT Point types 
    if ( Fxt::Point::Api::validatePointTypes( m_inputRefs, m_numInputs, Fxt::Point::Bool::GUID_STRING ) == false )
    {
        m_error = fullErr( Fxt::Component::Err_T::INPUT_REFRENCE_BAD_TYPE );
        m_error.logIt( getTypeName() );
        return m_error;
    }

    if ( !validateOutputType() )
    {
        m_error = fullErr( Fxt::Component::Err_T::OUTPUT_REFRENCE_BAD_TYPE );
        m_error.logIt( getTypeName() );
        return m_error;
    }

    m_error = fullErr( Err_T::SUCCESS );   // Set my state to 'ready-to-start'
    return m_error;
}

