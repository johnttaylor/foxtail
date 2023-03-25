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


#include "OnOff.h"
#include "Cpl/System/Assert.h"
#include "Fxt/Point/NumericHandlers.h"
#include <stdint.h>

///
using namespace Fxt::Component::Controller;


#define INPUT_IDX_PV            0   // Numeric type
#define INPUT_IDX_SP            1   // Numeric type
#define INPUT_IDX_HON           2   // Numeric type
#define INPUT_IDX_HOFF          3   // Numeric type

#define INPUT_IDX_MON           4   // Uint64
#define INPUT_IDX_MOFF          5   // Uint64
#define INPUT_IDX_RST           6   // Bool

#define INPUT_IDX_START_NUMERIC INPUT_IDX_PV
#define INPUT_NUM_NUMERIC       4

#define INPUT_IDX_START_UINT64  INPUT_IDX_MON
#define INPUT_NUM_UINT64        2

#define OUTPUT_IDX_OUT          0   // Bool
#define OUTPUT_IDX_NOTOUT       1   // Bool

#define STATE_IDX_STATE         0
#define STATE_IDX_TIME_MARKER   1
#define STATE_IDX_PREV_RESET    2

#define MIN_INPUTS              2
#define MIN_OUTPUTS             1

///////////////////////////////////////////////////////////////////////////////
OnOff::OnOff( JsonVariant&                       componentObject,
              Cpl::Memory::ContiguousAllocator&  generalAllocator,
              Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
              Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
              Fxt::Point::DatabaseApi&           dbForPoints )
    : m_numInputs( 0 )
    , m_numOutputs( 0 )
{
    memset( m_inputRefs, 0, sizeof( m_inputRefs ) );
    memset( m_outputRefs, 0, sizeof( m_outputRefs ) );
    memset( m_statefulPoints, 0, sizeof( m_statefulPoints ) );
    parseConfiguration( haStatefulDataAllocator, componentObject );
}

OnOff::~OnOff()
{
    // Nothing required
}

///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error OnOff::start( uint64_t currentElapsedTimeUsec ) noexcept
{

}

Fxt::Type::Error OnOff::execute( int64_t currentTickUsec ) noexcept
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

///////////////////////////////////////////////////////////////////////////////
const char* OnOff::getTypeGuid() const noexcept
{
    return GUID_STRING;
}

const char* OnOff::getTypeName() const noexcept
{
    return TYPE_NAME;
}

///////////////////////////////////////////////////////////////////////////////
bool OnOff::parseConfiguration( Cpl::Memory::ContiguousAllocator& haStatefulDataAllocator, JsonVariant & obj ) noexcept
{
    //
    // Parse Input Point references
    //

    // Check for the minimum number of input points
    JsonArray   inputs  = obj["inputs"];
    size_t      nInputs = inputs.size();
    if ( inputs.isNull() || nInputs < MIN_INPUTS )
    {
        m_error = fullErr( Err_T::INCORRECT_NUM_INPUT_REFS );
        m_error.logIt( getTypeName() );
        return false;
    }
    m_numInputs = (uint16_t) nInputs;

    // Parse PV
    size_t foundIdxNotUse;
    if ( !findAndparsePointReference( (size_t*) m_inputRefs,  // Start by storing the point ID
                                      "var",
                                      "PV",
                                      INPUT_IDX_PV,
                                      0,
                                      m_numInputs,
                                      inputs,
                                      true,
                                      foundIdxNotUse ) )
    {
        return false;
    }
    
    // Parse SP
    if ( !findAndparsePointReference( (size_t*) m_inputRefs,  // Start by storing the point ID
                                      "var",
                                      "SP",
                                      INPUT_IDX_SP,
                                      0,
                                      m_numInputs,
                                      inputs,
                                      true,
                                      foundIdxNotUse ) )
    {
        return false;
    }

    // Parse HON
    if ( !findAndparsePointReference( (size_t*) m_inputRefs,  // Start by storing the point ID
                                      "var",
                                      "HON",
                                      INPUT_IDX_HON,
                                      0,
                                      m_numInputs,
                                      inputs,
                                      false,
                                      foundIdxNotUse ) )
    {
        return false;
    }

    // Parse HOFF
    if ( !findAndparsePointReference( (size_t*) m_inputRefs,  // Start by storing the point ID
                                      "var",
                                      "HOFF",
                                      INPUT_IDX_HOFF,
                                      0,
                                      m_numInputs,
                                      inputs,
                                      false,
                                      foundIdxNotUse ) )
    {
        return false;
    }

    // Parse MON
    if ( !findAndparsePointReference( (size_t*) m_inputRefs,  // Start by storing the point ID
                                      "var",
                                      "MON",
                                      INPUT_IDX_MON,
                                      0,
                                      m_numInputs,
                                      inputs,
                                      false,
                                      foundIdxNotUse ) )
    {
        return false;
    }

    // Parse MOFF
    if ( !findAndparsePointReference( (size_t*) m_inputRefs,  // Start by storing the point ID
                                      "var",
                                      "MOFF",
                                      INPUT_IDX_MOFF,
                                      0,
                                      m_numInputs,
                                      inputs,
                                      false,
                                      foundIdxNotUse ) )
    {
        return false;
    }

    // Parse RST
    if ( !findAndparsePointReference( (size_t*) m_inputRefs,  // Start by storing the point ID
                                      "var",
                                      "RST",
                                      INPUT_IDX_RST,
                                      0,
                                      m_numInputs,
                                      inputs,
                                      false,
                                      foundIdxNotUse ) )
    {
        return false;
    }

    //
    // Parse Output Point references
    //
    
    // Check for the minimum number of output points
    JsonArray   outputs  = obj["outputs"];
    size_t      nOutputs = outputs.size();
    if ( outputs.isNull() || nOutputs < MIN_OUTPUTS )
    {
        m_error = fullErr( Err_T::INCORRECT_NUM_OUTPUT_REFS );
        m_error.logIt( getTypeName() );
        return false;
    }
    m_numOutputs = (uint16_t) nOutputs;

    // Parse O
    size_t out1foundIdx;
    if ( !findAndparsePointReference( (size_t*) m_outputRefs,  // Start by storing the point ID
                                      "var",
                                      "O",
                                      OUTPUT_IDX_OUT,
                                      0,
                                      m_numOutputs,
                                      outputs,
                                      false,
                                      out1foundIdx ) )
    {
        return false;
    }

    size_t out2foundIdx;
    if ( !findAndparsePointReference( (size_t*) m_outputRefs,  // Start by storing the point ID
                                      "var",
                                      "/O",
                                      OUTPUT_IDX_NOTOUT,
                                      0,
                                      m_numOutputs,
                                      outputs,
                                      false,
                                      out2foundIdx ) )
    {
        return false;
    }

    // Make sure we have at least one CORRECT output point
    if ( out1foundIdx == ((size_t) -1) && out2foundIdx == ((size_t) -1) )
    {
        m_error = fullErr( Err_T::MISSING_REQUIRED_FIELD );
        m_error.logIt( "%s. Must have 'O' or '/O'", getTypeName() );
        return false;
    }

    //
    // Create internal stateful data/points
    //
    m_statefulPoints[STATE_IDX_STATE]       = (Fxt::Point::Bool*) haStatefulDataAllocator.allocate( sizeof( Fxt::Point::Bool ) );
    m_statefulPoints[STATE_IDX_PREV_RESET]  = (Fxt::Point::Bool*) haStatefulDataAllocator.allocate( sizeof( Fxt::Point::Bool ) );
    m_statefulPoints[STATE_IDX_TIME_MARKER] = (Fxt::Point::Bool*) haStatefulDataAllocator.allocate( sizeof( Fxt::Point::Uint64 ) );
    if ( m_statefulPoints[STATE_IDX_STATE] == nullptr || 
         m_statefulPoints[STATE_IDX_PREV_RESET] == nullptr || 
         m_statefulPoints[STATE_IDX_TIME_MARKER] == nullptr )
    {
        m_error = fullErr( Err_T::OUT_OF_MEMORY );
        m_error.logIt( "%s. Stateful Allocator", getTypeName() );
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
Fxt::Type::Error OnOff::resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept
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

    // Validate INPUT Point types
    const char* numericGuid = m_inputRefs[0]->getTypeGuid();
    if ( Fxt::Point::Api::validatePointTypes( m_inputRefs+INPUT_IDX_START_NUMERIC, INPUT_NUM_NUMERIC, numericGuid ) == false )
    {
        m_error = fullErr( Err_T::INPUT_REFRENCE_BAD_TYPE );
        m_error.logIt( getTypeName() );
        return m_error;
    }
    if ( Fxt::Point::Api::validatePointTypes( m_inputRefs + INPUT_IDX_START_UINT64, INPUT_NUM_UINT64, Fxt::Point::Uint64::GUID_STRING ) == false )
    {
        m_error = fullErr( Err_T::INPUT_REFRENCE_BAD_TYPE );
        m_error.logIt( getTypeName() );
        return m_error;
    }
    if ( Fxt::Point::Api::validatePointTypes( m_inputRefs + INPUT_IDX_RST, 1, Fxt::Point::Bool::GUID_STRING ) == false )
    {
        m_error = fullErr( Err_T::INPUT_REFRENCE_BAD_TYPE );
        m_error.logIt( getTypeName() );
        return m_error;
    }

    // Validate that the input type IS a numeric type
    if ( !Fxt::Point::NumericHandlers::getIntegerPointAttributes( numericGuid ) )
    {
        if ( !Fxt::Point::NumericHandlers::getFloatPointAttributes( numericGuid ) )
        {
            m_error = fullErr( Err_T::INPUT_REFRENCE_BAD_TYPE );
            m_error.logIt( "%s. Not a numeric type", getTypeName() );
            return m_error;
        }
    }

    // Validate OUTPUT Point types
    if ( Fxt::Point::Api::validatePointTypes( (Fxt::Point::Api **) m_outputRefs, m_numOutputs, Fxt::Point::Bool::GUID_STRING ) == false )
    {
        m_error = fullErr( Err_T::OUTPUT_REFRENCE_BAD_TYPE );
        m_error.logIt( getTypeName() );
        return m_error;
    }

    m_error = Fxt::Type::Error::SUCCESS();   // Set my state to 'ready-to-start'
    return m_error;
}

