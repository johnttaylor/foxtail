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


#include "DemuxBase.h"
#include "Error.h"
#include "Cpl/System/Assert.h"
#include <stdint.h>

///
using namespace Fxt::Component::Digital;


///////////////////////////////////////////////////////////////////////////////
DemuxBase::DemuxBase()
    : Common_()
{
    // All of the work is done the child class
}

DemuxBase::~DemuxBase()
{
    // Nothing required
}

///////////////////////////////////////////////////////////////////////////////
void DemuxBase::invalidateAllOutputs() noexcept
{
    for ( unsigned i=0; i < m_numOutputs; i++ )
    {
        m_outputRefs[i]->setInvalid();
    }
}

///////////////////////////////////////////////////////////////////////////////
bool DemuxBase::parseConfiguration( Cpl::Memory::ContiguousAllocator& generalAllocator,
                                    JsonVariant& obj,
                                    unsigned minInputs,
                                    unsigned maxInputs,
                                    unsigned minOutputs,
                                    unsigned maxOutputs ) noexcept
{
    // Parse references
    if ( !parseInputReferences( generalAllocator, obj, minInputs, maxInputs ) ||
         !parseOutputReferences( generalAllocator, obj, minOutputs, maxOutputs ) )
    {
        return false;
    }

    // Allocate memory for internal lists
    m_bitOffsets    = (uint8_t*) generalAllocator.allocate( sizeof( uint8_t ) * m_numOutputs );
    m_outputNegated = (bool*) generalAllocator.allocate( sizeof( bool ) * m_numOutputs );
    if ( m_bitOffsets == nullptr || m_outputNegated == nullptr )
    {
        m_error = fullErr( Fxt::Component::Err_T::OUT_OF_MEMORY );
        m_error.logIt( getTypeName() );
        return false;
    }

    // Parse output bit offsets and negate qualifiers
    JsonArray outputs = obj["outputs"];
    for ( unsigned i=0; i < m_numOutputs; i++ )
    {
        m_outputNegated[i] = outputs[i]["negate"] | false;
        m_bitOffsets[i]    = outputs[i]["bit"] | maxOutputs / 2;  // There are up to 2 outputs per bit!
        if ( m_bitOffsets[i] >= maxOutputs / 2 )
        {
            m_error = fullErr( Err_T::DEMUX_INVALID_BIT_OFFSET );
            m_error.logIt( getTypeName() );
            return false;
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error DemuxBase::resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept
{
    // Resolve references
    if ( resolveInputOutputReferences( pointDb ) )
    {
        return m_error;
    }

    // Validate Output Point types
    if ( Fxt::Point::Api::validatePointTypes( m_outputRefs, m_numOutputs, Fxt::Point::Bool::GUID_STRING ) == false )
    {
        m_error = fullErr( Fxt::Component::Err_T::OUTPUT_REFRENCE_BAD_TYPE );
        m_error.logIt( getTypeName() );
        return m_error;
    }

    // Validate Input type
    if ( Fxt::Point::Api::validatePointTypes( m_inputRefs, 1, getInputPointTypeGuid() ) )
    {
        m_error = fullErr( Fxt::Component::Err_T::INPUT_REFRENCE_BAD_TYPE );
        m_error.logIt( getTypeName() );
        return m_error;
    }

    m_error = fullErr( Err_T::SUCCESS );   // Set my state to 'ready-to-start'
    return m_error;
}

