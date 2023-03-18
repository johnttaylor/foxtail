#ifndef Fxt_Node_FactoryApi_h_
#define Fxt_Node_FactoryApi_h_
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

#include "Cpl/Json/Arduino.h"
#include "Fxt/Point/DatabaseApi.h"
#include "Fxt/Point/FactoryDatabase.h"
#include "Fxt/Card/FactoryDatabase.h"
#include "Fxt/Component/FactoryDatabase.h"
#include "Fxt/Node/Api.h"
#include <stdint.h>
#include <stdlib.h>

///
namespace Fxt {
///
namespace Node {

/** This abstract class defines the interface for a 'Factory' that knows how
    to create a Node of a specific type.

    The semantics of the Factory interface is NOT thread safe.

   \code
    Required/Defined JSON fields/structure:
        {
          "name":                   "*<human readable name for the Node - not required to be unique>",
          "type":                   "<Nodes's Type GUID: 8-4-4-4-12 format>",
          "typeName":               "*<OPTIONAL: human readable Node type>",
          "id":                     <*Global (to the Enterprise) ID for the Chassis.  Range: 0-64K>,
          "chassis": [              // List of Chassis (along with it IO Cards, Logic Chains, etc.)
            {...},
            ...
          ],
        }

    *The field is NOT parsed/used by the firmware

    \endcode
 */
class FactoryApi
{
public:
    /** This method is used to destroy/free an Node.

        Note: The factory will call stop() on the specified Node before
              destroying.
      */
    virtual void destroy( Fxt::Node::Api& nodeToDestroy ) noexcept = 0;

    /** This method creates a Node. The concrete type of the Node is
        dependent on the concrete Factory that is ultimately invoked to create
        the Node.

        The method returns a pointer to the created Node when successful; else
        if an error occurred (e.g. out-of-memory) nullptr is returned.
        When an error occurs, the 'nodeErrorode' argument is updated with
        details of the error.
      */
    virtual Api* createFromJSON( JsonVariant&             nodeJsonObject,
                                 Fxt::Point::DatabaseApi& dbForPoints,
                                 Fxt::Type::Error&        nodeErrorode ) noexcept = 0;



public:
    /** This method returns the GUID for the type of Node that the factory
        is able to create.  The return value is a null terminated string with
        a 8-4-4-4-12 formatted GUID.
     */
    virtual const char* getGuid() const noexcept = 0;

    /// Returns the node's 'human readable' type name (note: this is NOT guaranteed to be unique)
    virtual const char* getTypeName() const noexcept = 0;

    /** This method return the maximum chassis the Node supports
     */
    virtual uint8_t getMaxAllowedChassis() const noexcept = 0;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~FactoryApi() {}
};


};      // end namespaces
};
#endif  // end header latch
