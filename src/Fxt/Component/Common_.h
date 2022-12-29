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
    /// Helper method to extract Point references
    bool parsePointReferences( size_t           dstReferences[],
                               unsigned         maxNumReferences,
                               JsonArray&       arrayObj,
                               Fxt::Type::Error errTooMany,
                               Fxt::Type::Error errBadRef,
                               unsigned&        numRefsFound );

    /// Helper Resolve Point References
    bool resolveReferences( Fxt::Point::DatabaseApi& pointDb,
                            Fxt::Point::Api*         srcIdsAndDstPointers[],
                            unsigned                 numElements );

protected:
    /// Time marker (in microseconds) of last execution cycle
    uint64_t         m_lastExeCycleTimeUsec;

    /// Error state. A value of 0 indicates NO error
    Fxt::Type::Error m_error;

    /// My started state
    bool             m_started;
};



};      // end namespaces
};
#endif  // end header latch
