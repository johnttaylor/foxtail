#ifndef Fxt_Component_FactoryApi_h_
#define Fxt_Component_FactoryApi_h_
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

#include "Cpl/Container/Item.h"
#include "Cpl/Json/Arduino.h"
#include "Cpl/Memory/ContiguousAllocator.h"
#include "Fxt/Point/FactoryDatabaseApi.h"
#include "Fxt/Component/Api.h"
#include <stdint.h>
#include <stdlib.h>

///
namespace Fxt {
///
namespace Component {

/** This abstract class defines the interface for a 'Factory' that knows how
    to create a Component of a specific type.

    The semantics of the Factory interface is NOT thread safe.

    Note: Factories support being in a Container - however 'that' container
          is the Factory Database.  This means if the application wants to maintain
          a list of factories - it must provide its own wrapper.

    \code
    Required/Defined JSON fields/structure:

       {
          "name":               "*<human readable name for the Component - not required to be unique>",
          "type":               "<Component's Type GUID: 8-4-4-4-12 format>",
          "typeName":           "*<OPTIONAL: human readable component type>",
          "inputs": [           // Array of Point reference that supply the Component's input values
               {
                 "name":        "*<human readable name for the input>",
                 "type":        "*<Points's Type GUID: 8-4-4-4-12 format>",
                 "typeName":    "*<OPTIONAL: human readable point type>",
                 "idRef":       <Reference to a Point ID, i.e. where to read the input value from>,
                 "idRefName":   "*OPTIONAL: human readable point label"

               },
               ...
            ],
          "outputs": [          // Array of Point reference where the Component's writes it output values to
               {
                 "name":        "*<human readable name for the input>",
                 "type":        "*<Points's Type GUID: 8-4-4-4-12 format>",
                 "typeName":    "*<OPTIONAL: human readable point type>",
                 "idRef":       <Reference to a Point ID, i.e. where to read the input value from>,
                 "idRefName":   "*OPTIONAL: human readable point label"
               },
               ...
            ],
          "stateful": [         // OPTIONAL Array of Point definition that the Component uses to store stateful data
               {
                "id":           <ID. Note: the ID key can vary, e.g. 'id', 'ioRegId', etc.>,
                "type":         "<Points's Type GUID: 8-4-4-4-12 format>",
                "typeCfg:       <OPTIONAL type configuration for complex types, e.g. "typeCfg":{"numElems":2}>,
                "typeName":     "*<OPTIONAL: human readable point type>",
                "name":         "*<human readable name for the point>"
                "initial": {    // OPTIONAL initial value/state specifier for the Point
                    "valid":    <true|false  // Initial valid state for the internal point>,
                    "val":      <point value as defined by the Point type's fromJSON syntax - only required when 'valid' is 'true' OR when 'valid' is ommitted>
                    "id":       <The ID of the internal 'setter' point that is used store the initial value in binary form>
                 }
               },
               ...
            ]
        }

    *The field is NOT parsed/used by the firmware

   \endcode
 */
class FactoryApi: public Cpl::Container::Item
{
public:
    /** This method creates a Component. The concrete type of the Component is 
        dependent on the concrete Factory that is ultimately invoked to create the 
        Component.

        Each individual Component Type defines the content/structure of its JSON
        'componentObject'.

        The method returns a pointer to the created component when successful; 
        else if an error occurred (e.g. out-of-memory) nullptr is returned.
        When an error occurs, the 'componentErrorCode' argument is updated with 
        details of the error.
      */
    virtual Api* create( JsonVariant&                       componentObject, 
                         Fxt::Type::Error&                  componentErrorCode,
                         Cpl::Memory::ContiguousAllocator&  generalAllocator,
                         Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                         Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                         Fxt::Point::DatabaseApi&           dbForPoints ) noexcept = 0;


    /** This method is used to destroy/free a Component.  

        Note: The factory will call stop() on the specified Component before 
              destroying.
      */
    virtual void destroy( Api& componentToDestory ) noexcept = 0;

public:
    /** This method returns the GUID for the type of Component that the factory 
        is able to create.  The return value is a null terminated string with
        a 8-4-4-4-12 formatted GUID.
     */
    virtual const char* getGuid() const noexcept = 0;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~FactoryApi() {}
};


};      // end namespaces
};
#endif  // end header latch
