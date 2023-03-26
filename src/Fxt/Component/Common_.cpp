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


#include "Common_.h"
#include "Error.h"


///
using namespace Fxt::Component;


//////////////////////////////////////////////////
Common_::Common_()
    : m_error( Fxt::Type::Error::SUCCESS() )
    , m_numInputs( 0 )
    , m_numOutputs( 0 )
    , m_started( false )
{
}

Common_::~Common_()
{
    // ensure stop gets called
    stop();
}

//////////////////////////////////////////////////
Fxt::Type::Error Common_::start( uint64_t currentElapsedTimeUsec ) noexcept
{
    if ( !m_started && m_error == Fxt::Type::Error::SUCCESS() )
    {
        m_started = true;
        return Fxt::Type::Error::SUCCESS();
    }
    m_error = fullErr( Err_T::FAILED_START );
    m_error.logIt();
    return m_error;
}

void Common_::stop() noexcept
{
    if ( m_started )
    {
        m_started = false;
    }
}

bool Common_::isStarted() const noexcept
{
    return m_started;
}

Fxt::Type::Error Common_::getErrorCode() const noexcept
{
    return m_error;
}

/////////////////////////////////////////////
bool Common_::parseInputReferences( Cpl::Memory::ContiguousAllocator& generalAllocator,
                                    JsonVariant&                      obj,
                                    unsigned                          minPoints,
                                    unsigned                          maxPoints ) noexcept
{
    // Parse Point inputs
    JsonArray inputs = obj["inputs"];
    if ( !inputs.isNull() )
    {
        // Validate number of points
        size_t nRefs = inputs.size();
        if ( nRefs < minPoints || nRefs > maxPoints )
        {
            m_error = fullErr( Err_T::INCORRECT_NUM_INPUT_REFS );
            m_error.logIt( getTypeName() );
            return false;
        }
        m_numInputs = (unsigned) nRefs;

        // Allocate memory for managing inputs
        m_inputRefs = (Fxt::Point::Api**) generalAllocator.allocate( sizeof( Fxt::Point::Api* ) * m_numInputs );
        if ( m_inputRefs == nullptr )
        {
            m_error = fullErr( Fxt::Component::Err_T::OUT_OF_MEMORY );
            m_error.logIt( getTypeName() );
            return false;
        }

        if ( !parsePointReferences( (size_t*) m_inputRefs,  // Start by storing the point ID
                                    m_numInputs,
                                    inputs ) )
        {
            return false;
        }
    }

    else if ( minPoints > 0 )
    {
        return false;
    }

    return true;
}

bool Common_::parseOutputReferences( Cpl::Memory::ContiguousAllocator& generalAllocator,
                                     JsonVariant&                      obj,
                                     unsigned                          minPoints,
                                     unsigned                          maxPoints ) noexcept
{
    // Parse Point outputs
    JsonArray outputs = obj["outputs"];
    if ( !outputs.isNull() )
    {
        // Validate number of points
        size_t nRefs = outputs.size();
        if ( nRefs < minPoints || nRefs > maxPoints )
        {
            m_error = fullErr( Err_T::INCORRECT_NUM_OUTPUT_REFS );
            m_error.logIt( getTypeName() );
            return false;
        }
        m_numOutputs = (unsigned) nRefs;

        // Allocate memory for managing outputs
        m_outputRefs = (Fxt::Point::Api**) generalAllocator.allocate( sizeof( Fxt::Point::Api* ) * m_numOutputs );
        if ( m_outputRefs == nullptr )
        {
            m_error = fullErr( Fxt::Component::Err_T::OUT_OF_MEMORY );
            m_error.logIt( getTypeName() );
            return false;
        }

        if ( !parsePointReferences( (size_t*) m_outputRefs,  // Start by storing the point ID
                                    m_numOutputs,
                                    outputs ) )
        {
            return false;
        }
    }

    else if ( minPoints > 0 )
    {
        return false;
    }

    return true;
}

bool Common_::parsePointReferences( size_t              dstReferences[],
                                    unsigned            numPoints,
                                    JsonArray&          arrayObj )
{
    // Extract the Point inputs
    for ( unsigned i=0; i < numPoints; i++ )
    {
        JsonObject elem = arrayObj[i];
        if ( !parsePointReference( dstReferences, i, elem ) )
        {
            return false;
        }
    }

    return true;
}

bool Common_::parsePointReference( size_t           dstReferences[],
                                   unsigned         referenceIndex,
                                   JsonObject&      objInstance )
{
    uint32_t pointRef = objInstance["idRef"] | Fxt::Point::Api::INVALID_ID;
    if ( pointRef == Fxt::Point::Api::INVALID_ID )
    {
        m_error = fullErr( Fxt::Component::Err_T::BAD_INPUT_REFERENCE );
        m_error.logIt( getTypeGuid() );
        return false;
    }

    dstReferences[referenceIndex] = pointRef;
    return true;
}

bool Common_::parseInputReferences( Cpl::Memory::ContiguousAllocator& generalAllocator,
                                    JsonVariant&                      obj,
                                    const NamedRef_T                  names[],
                                    unsigned                          minPoints,
                                    unsigned                          maxPoints ) noexcept
{
    // Parse Point inputs
    JsonArray inputs = obj["inputs"];
    if ( !inputs.isNull() )
    {
        // Validate number of points
        size_t nRefs = inputs.size();
        if ( nRefs < minPoints || nRefs > maxPoints )
        {
            m_error = fullErr( Err_T::INCORRECT_NUM_INPUT_REFS );
            m_error.logIt( getTypeName() );
            return false;
        }
        m_numInputs = (unsigned) nRefs;

        // Allocate memory for managing inputs (ALWAYS allocate max number of inputs)
        m_inputRefs = (Fxt::Point::Api**) generalAllocator.allocate( sizeof( Fxt::Point::Api* ) * maxPoints );
        if ( m_inputRefs == nullptr )
        {
            m_error = fullErr( Fxt::Component::Err_T::OUT_OF_MEMORY );
            m_error.logIt( getTypeName() );
            return false;
        }

        // Set the array entries to all ones.  This is so 'missing optional points' are indicated by a -1
        memset( m_inputRefs, 0xFF, sizeof( sizeof( Fxt::Point::Api* ) * maxPoints ) );

        // Search for the named elements
        for ( unsigned i=0; i <= m_numInputs; i++ )
        {
            JsonObject elem = inputs[i];

            // Look-up expected names
            for ( unsigned j=0; j < maxPoints; j++ )
            {
                const char* keyVal = names[j].keyName;

                // Match found -->parse the reference
                if ( keyVal != nullptr && strcmp( keyVal, names[j].keyValue ) == 0 )
                {
                    parsePointReference( (size_t*) m_inputRefs, j, elem );
                }
            }
        }

        // Determine if there are missing required references
        for ( unsigned j=0; j < maxPoints; j++ )
        {
            // Throw an error if required KV pair is missing
            if ( m_inputRefs[j] == nullptr && names[j].required == true )
            {
                m_error = fullErr( Err_T::MISSING_REQUIRED_FIELD );
                m_error.logIt( "%s. Missing %s", getTypeGuid(), names[j].keyValue );
                return false;
            }
        }

        // If get here everything is GOOD
        return true;
    }

    return false;
}

bool Common_::parseOutputReferences( Cpl::Memory::ContiguousAllocator& generalAllocator,
                                     JsonVariant&                      obj,
                                     const NamedRef_T                  names[],
                                     unsigned                          minPoints,
                                     unsigned                          maxPoints ) noexcept
{
    // Parse Point outputs
    JsonArray outputs = obj["outputs"];
    if ( !outputs.isNull() )
    {
        // Validate number of points
        size_t nRefs = outputs.size();
        if ( nRefs < minPoints || nRefs > maxPoints )
        {
            m_error = fullErr( Err_T::INCORRECT_NUM_OUTPUT_REFS );
            m_error.logIt( getTypeName() );
            return false;
        }
        m_numOutputs = (unsigned) nRefs;

        // Allocate memory for managing inputs (ALWAYS allocate max number of outputs)
        m_outputRefs = (Fxt::Point::Api**) generalAllocator.allocate( sizeof( Fxt::Point::Api* ) * maxPoints );
        if ( m_outputRefs == nullptr )
        {
            m_error = fullErr( Fxt::Component::Err_T::OUT_OF_MEMORY );
            m_error.logIt( getTypeName() );
            return false;
        }

        // Set the array entries to all ones.  This is so 'missing optional points' are indicated by a -1
        memset( m_outputRefs, 0xFF, sizeof( sizeof( Fxt::Point::Api* ) * maxPoints ) );

        // Search for the named elements
        for ( unsigned i=0; i <= m_numOutputs; i++ )
        {
            JsonObject elem = outputs[i];

            // Look-up expected names
            for ( unsigned j=0; j < maxPoints; j++ )
            {
                const char* keyVal = names[j].keyName;

                // Match found -->parse the reference
                if ( keyVal != nullptr && strcmp( keyVal, names[j].keyValue ) == 0 )
                {
                    parsePointReference( (size_t*) m_outputRefs, j, elem );
                }
            }
        }

        // Determine if there are missing required references
        for ( unsigned j=0; j < maxPoints; j++ )
        {
            // Throw an error if required KV pair is missing
            if ( m_outputRefs[j] == nullptr && names[j].required == true )
            {
                m_error = fullErr( Err_T::MISSING_REQUIRED_FIELD );
                m_error.logIt( "%s. Missing %s", getTypeGuid(), names[j].keyValue );
                return false;
            }
        }

        // If get here everything is GOOD
        return true;
    }

    return false;
}

/////////////////////////////////////////////////
bool Common_::resolveInputOutputReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept
{
    // Resolve INPUT references
    if ( !resolveReferencesToPoint( pointDb,
                                    (Fxt::Point::Api **) m_inputRefs,    // Pass as array of generic Point pointers
                                    m_numInputs ) )
    {
        m_error = fullErr( Fxt::Component::Err_T::UNRESOLVED_INPUT_REFRENCE );
        m_error.logIt( getTypeName() );
        return false;
    }

    // Resolve OUTPUT references
    if ( !resolveReferencesToPoint( pointDb,
                                    (Fxt::Point::Api **) m_outputRefs,    // Pass as array of generic Point pointers
                                    m_numOutputs ) )
    {
        m_error = fullErr( Fxt::Component::Err_T::UNRESOLVED_OUTPUT_REFRENCE );
        m_error.logIt( getTypeName() );
        return false;
    }

    return true;
}

bool Common_::resolveReferencesToPoint( Fxt::Point::DatabaseApi&   pointDb,
                                        Fxt::Point::Api*           srcIdsAndDstPointers[],
                                        unsigned                   numElements )
{
    // Fail if the component HAS started (i.e. resolving inputs has to be done BEFORE starting the components)
    if ( m_started || m_error != Fxt::Type::Error::SUCCESS() )
    {
        return false;
    }

    for ( unsigned i=0; i < numElements; i++ )
    {
        uint32_t pointId = (size_t) (srcIdsAndDstPointers[i]);

        // Skip optional/missing entries (only occurs with 'named' references)
        if ( pointId == ((uint32_t) -1) )
        {
            srcIdsAndDstPointers[i] = nullptr;
        }

        // Look-up the point pointer for the referenced Point ID
        else
        {
            Fxt::Point::Api* pointPtr = pointDb.lookupById( pointId );
            if ( pointPtr == nullptr )
            {
                return false;
            }

            srcIdsAndDstPointers[i] = pointPtr;
        }
    }

    return true;
}

