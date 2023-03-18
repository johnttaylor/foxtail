#ifndef Fxt_Node_FactoryCommon_h_
#define Fxt_Node_FactoryCommon_h_
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


#include "Fxt/Node/FactoryApi.h"
#include "Fxt/Node/Error.h"

///
namespace Fxt {
///
namespace Node {


/** This partially concrete class provide common infrastructure for a Node
    Factory.
 */
class FactoryCommon_ : public Fxt::Node::FactoryApi
{
public:
    /// Constructor
    FactoryCommon_();

    /// Destructor
    ~FactoryCommon_();

public:
    /// See Fxt::Node::FactoryApi
    void destroy( Api& nodeToDestroy ) noexcept;

    /// See Fxt::Node::FactoryApi
    Api* createFromJSON( JsonVariant&             nodeJsonObject,
                         Fxt::Point::DatabaseApi& dbForPoints,
                         Fxt::Type::Error&        nodeErrorode ) noexcept;

protected:
    /// Helper method that perform the Node specific create.  Assumes the new(std::nothrow) is used to allocate the Node instance
    virtual Api* createNode( uint8_t                    numChassis,
                             Fxt::Point::DatabaseApi&   pointDb,
                             JsonVariant                nodeJsonObject,
                             Fxt::Type::Error&          nodeErrorCode ) noexcept = 0;

public:
    // NOTE: The following members are made public to facilitate accessing the factory databases

    /// Point Factory DB
    static Fxt::Point::FactoryDatabase     g_pointFactoryDb;

    /// Card Factory DB
    static Fxt::Card::FactoryDatabase      g_cardFactoryDb;

    /// Component Factory DB
    static Fxt::Component::FactoryDatabase g_componentFactoryDb;
};


};      // end namespaces
};
#endif  // end header latch
