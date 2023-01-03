#ifndef Fxt_Node_Mock_Kestral_Factory_h_
#define Fxt_Node_Mock_Kestral_Factory_h_
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


#include "colony_config.h"
#include "Fxt/Node/FactoryCommon_.h"
#include "Fxt/Node/Mock/Kestrel/Node.h"
#include "Cpl/System/Thread.h"

///
namespace Fxt {
///
namespace Node {
///
namespace Mock {
///
namespace Kestrel {


/** This concrete class implements the Fxt::Node::FactoryApi interface.
 */
class Factory : public Fxt::Node::FactoryCommon_
{
public:
    /// Constructor
    Factory( size_t                              sizeGeneralHeap,
             size_t                              sizeCardStatefulHeap,
             size_t                              sizeHaStatefulHeap,
             Cpl::System::SharedEventHandlerApi* eventHandler = nullptr );

public:
    /// See Fxt::Node::FactoryCommon_
    Api* createNode( uint8_t                    numChassis,
                     JsonVariant                nodeJsonObject,
                     Fxt::Type::Error&          nodeErrorCode ) noexcept;

public:
    /// See Fxt::Node::FactoryApi
    const char* getGuid() const noexcept { return Node::GUID_STRING; }

    /// See Fxt::Node::FactoryApi
    uint8_t getMaxAllowedChassis() const noexcept { return Node::MAX_ALLOWED_CHASSIS; }

    /// See Fxt::Node::FactoryApi
    const char* getTypeName() const noexcept { return Node::TYPE_NAME; }

protected:
    /// Shared Event handler for a Chassis thread
    Cpl::System::SharedEventHandlerApi* m_eventHandler;

    /// Size, in bytes, of a Node's general heap
    size_t  m_sizeGeneralHeap;

    /// Size, in bytes, of a Node's Card stateful heap
    size_t  m_sizeCardStatefulHeap;

    /// Size, in bytes, of a Node's HA Stateful heap
    size_t  m_sizeHaStatefulHeap;
};



}       // end namespaces
}
}
}
#endif  // end header latch
