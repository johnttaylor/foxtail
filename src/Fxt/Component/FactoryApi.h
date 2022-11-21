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
#include "Cpl/Memory/Allocator.h"
#include "Cpl/Json/Arduino.h"
#include "Fxt/Component/Api.h"
#include "Fxt/Point/BankApi.h"
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
          is the Factory Database.  This means if the application what to maintain
          a list of factories - it must provide its own wrapper.

    \code
    Required/Defined JSON fields/structure:

       {
          "name":           "<human readable name for the Component - not required to be unique>",
          "type":           "<Component's Type GUID: 8-4-4-4-12 format>",
          "typeName":       "<OPTIONAL: human readable component type>",
          "exeOrder":       <execution order within the Logic Chain. Range: 0-64K>,
          "inputs": [       // Array of Point reference that supply the Component's input values
               {
                 "name":    "<human readable name for the input>",
                 "type":    "<Point type as string (unique within the Node)>",
                 "idRef":   <Reference to a Point ID, i.e. where to read the input value from>,
               },
               ...
            ],
          "outputs": [      // Array of Point reference where the Component's writes it output values to
               {
                 "name":    "<human readable name for the output>",
                 "type":    "<Point type as string (unique within the Node)>",
                 "idRef":   <Reference to a Point ID, i.e. where to write the output value to>,
               },
               ...
            ],
          "stateful": [    // OPTIONAL Array of Point definition that the Component uses to store stateful data
               {
                 "name":    "<human readable name for the internal data>",
                 "type":    "<Point type as string (unique within the Node),
                 "id":      <Point ID>,
                 "initial": {           // OPTIONAL initial value/state specifier for the Point
                   "valid": true|false  // Initial valid state for the internal point
                   "val":               <point value as defined by the Point type's fromJSON syntax - only required when 'valid' is 'true'>
                   "id":                <The ID of the internal 'setter' point that is used store the initial value in binary form>
                 }
               },
               ...
            ]
        }

   \endcode
 */
class FactoryApi: public Cpl::Container::Item
{
public:
    /** This method creates a Card and inserts the instance into the Card 
        Database. The concrete type of the IO card is dependent on the concrete 
        Factory that is ultimately invoked to create the IO card.

        Each individual Card Type defines the content/structure of its JSON
        'cardObject'.  The 'cardObject' must contain sufficient information to 
        construct a Point Descriptor for each internal Point and each 
        external/visible/Register Point created by the IO card.

        NOTE: Only the 'Virtual Points' are accessible via Point Descriptors

        Note: The IO Card is responsible for allocating the memory for the
              Point Descriptors and the memory must stay in scope until the
              IO card is destroyed.
 
        The method returns the Component's execution order index via the 'exeOrder'
        function argument.

        The method returns a pointer to the created card when successful; else 
        if an error occurred (e.g. out-of-memory) nullptr is returned.
        When an error occurs, the 'cardErrorCode' argument is updated with 
        details of the error.
      */
    virtual Api* create( Fxt::Point::BankApi&               statePointBank, 
                         JsonVariant&                       componentObject, 
                         Fxt::Type::Error&                  componentErrorCode,
                         Cpl::Memory::ContiguousAllocator&  generalAllocator,
                         Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                         Fxt::Point::DatabaseApi&           dbForPoints,
                         uint16_t&                          exeOrder ) noexcept = 0;


    /** This method is used to destroy/free an IO card.  

        Note: The factory will call stop() on the specified IO card before 
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
