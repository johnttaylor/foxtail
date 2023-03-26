#ifndef Fxt_Component_Common_h_
#define Fxt_Component_Common_h_
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


#include "Fxt/Component/Api.h"
#include "Fxt/Component/Error.h"
#include "Fxt/Point/DatabaseApi.h"
#include "Cpl/Json/Arduino.h"
#include "Cpl/Memory/ContiguousAllocator.h"

///
namespace Fxt {
///
namespace Component {


/** This partially concrete class provide common infrastructure for a Card
 */
class Common_ : public Api
{
public:
    /// Constructor
    Common_();

    /// Destructor
    ~Common_();

public:
    /// See Fxt::Component::Api
    Fxt::Type::Error start( uint64_t currentElapsedTimeUsec ) noexcept;

    /// See Fxt::Component::Api
    void stop() noexcept;

    /// See Fxt::Component::Api
    bool isStarted() const noexcept;

    /// See Fxt::Component::Api
    Fxt::Type::Error getErrorCode() const noexcept;


protected:
    /// Struct used to parsed named input/output references
    struct NamedRef_T
    {
        const char* keyName;    //!< JSON Key name
        const char* keyValue;   //!< JSON Key value, i.e. the label of the named reference
        bool        required;   //!< When set to true, the KV pair is required point
    };

protected:
    /// Parse non-named inputs, i.e. order doesn't matter or order is defined
    bool parseInputReferences( Cpl::Memory::ContiguousAllocator& generalAllocator,
                               JsonVariant&                      obj,
                               unsigned                          minPoints,
                               unsigned                          maxPoints ) noexcept;

    /// Parse named inputs.  'maxPoints' is used to allocate m_inputRef array. names[] must have at least 'maxPoints' entries 
    bool parseInputReferences( Cpl::Memory::ContiguousAllocator& generalAllocator,
                               JsonVariant&                      obj,
                               const NamedRef_T                  names[],
                               unsigned                          minPoints,
                               unsigned                          maxPoints ) noexcept;


    /// Parse non-named outputs, i.e. order doesn't matter or order is defined
    bool parseOutputReferences( Cpl::Memory::ContiguousAllocator& generalAllocator,
                                JsonVariant&                      obj,
                                unsigned                          minPoints,
                                unsigned                          maxPoints ) noexcept;

    /// Parse named outputs.  'maxPoints' is used to allocate m_outputRef array. names[] must have at least 'maxPoints' entries 
    bool parseOutputReferences( Cpl::Memory::ContiguousAllocator& generalAllocator,
                                JsonVariant&                      obj,
                                const NamedRef_T                  names[],
                                unsigned                          minPoints,
                                unsigned                          maxPoints ) noexcept;

protected:
    /// Helper method to extract Point references
    bool parsePointReferences( size_t           dstReferences[],
                               unsigned         numPoints,
                               JsonArray&       arrayObj );

    /// Helper method to extract a SINGLE Point reference
    bool parsePointReference( size_t           dstReferences[],
                              unsigned         referenceIndex,
                              JsonObject&      objInstance );


    /// Helper that resolve Inputs and Outputs references
    bool resolveInputOutputReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept;

    /// Helper Resolve Point References
    bool resolveReferencesToPoint( Fxt::Point::DatabaseApi& pointDb,
                                   Fxt::Point::Api*         srcIdsAndDstPointers[],
                                   unsigned                 numElements );




protected:
    /// List of Input Points.  Note: Initially the point IDs are stored instead of pointers
    Fxt::Point::Api**                       m_inputRefs;

    /// List of Output Points. Note: Initially the point IDs are stored instead of pointers
    Fxt::Point::Api**                       m_outputRefs;

    /// Error state. A value of 0 indicates NO error
    Fxt::Type::Error                        m_error;

    /// Number of Input points
    unsigned                                m_numInputs;

    /// Number of Output points
    unsigned                                m_numOutputs;

    /// My started state
    bool             m_started;
};



};      // end namespaces
};
#endif  // end header latch
