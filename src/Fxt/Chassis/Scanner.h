#ifndef Fxt_LogicChain_Chain_h_
#define Fxt_LogicChain_Chain_h_
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


#include "Fxt/LogicChain/Api.h"
#include "Fxt/LogicChain/Error.h"
#include "Fxt/Point/DatabaseApi.h"
#include "Cpl/Json/Arduino.h"
#include "Cpl/Memory/ContiguousAllocator.h"

///
namespace Fxt {
///
namespace LogicChain {


/** This concrete class implements the LogicChain interface
 */
class Chain : public Api
{
public:
    /// Constructor
    Chain( Cpl::Memory::ContiguousAllocator&   generalAllocator,
           uint16_t                            numComponents,
           uint16_t                            numAutoPoints );

    /// Destructor
    ~Chain();

public:
    /// See Fxt::LogicChain::Api
    Fxt::Type::Error resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept;

    /// See Fxt::LogicChain::Api
    Fxt::Type::Error start( uint64_t currentElapsedTimeUsec ) noexcept;

    /// See Fxt::LogicChain::Api
    void stop() noexcept;

    /// See Fxt::LogicChain::Api
    bool isStarted() const noexcept;

    /// See Fxt::LogicChain::Api
    Fxt::Type::Error execute( int64_t currentTickUsec ) noexcept;

    /// See Fxt::LogicChain::Api
    Fxt::Type::Error getErrorCode() const noexcept;

    /// See Fxt::LogicChain::Api
    Fxt::Type::Error add( Fxt::Component::Api& componentToAdd ) noexcept;

    /// See Fxt::LogicChain::Api
    Fxt::Type::Error add( Fxt::Point::Api& autoPointToAdd ) noexcept;

protected:
    /// Array/List of components in the logic chain
    Fxt::Component::Api**               m_components;

    /// Array/List of Auto points in the logic chain
    Fxt::Point::Api**                   m_autoPoints;

    /// Error state. A value of 0 indicates NO error
    Fxt::Type::Error                    m_error;

    /// Number of components
    uint16_t                            m_numComponents;

    /// Number of Auto Point
    uint16_t                            m_numAutoPoints;

    /// Array index for the next Component add operation
    uint16_t                            m_nextComponentIdx;

    /// Array index for the next Auto Points add operation
    uint16_t                            m_nextAutoPtsIdx;

    /// My started state
    bool                                m_started;
};



};      // end namespaces
};
#endif  // end header latch
