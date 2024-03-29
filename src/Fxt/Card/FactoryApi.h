#ifndef Fxt_Card_FactoryApi_h_
#define Fxt_Card_FactoryApi_h_
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
#include "Fxt/Point/Bank.h"
#include "Fxt/Card/Api.h"
#include "Cpl/Itc/PostApi.h"
#include <stdint.h>
#include <stdlib.h>

///
namespace Fxt {
///
namespace Card {

/** This abstract class defines the interface for a 'Factory' that knows how
    to create a IO card of a specific type.

    The semantics of the Factory interface is NOT thread safe.

    Note: Factories support being in a Container - however 'that' container
          is the Factory Database.  This means if the application what to maintain
          a list of factories - it must provide its own wrapper.

    \code
    Required/Defined JSON fields/structure:
       {
          "name":           "*<human readable name for the card - not required to be unique>",
          "id":             *<ID.  Must be unique. Range: 0-64K (note: NOT used by the firmware)>,
          "type":           "<Card's Type GUID: 8-4-4-4-12 format>",
          "typeName":       "*<OPTIONAL: human readable card type>",
          "slot":           <chassis slot number. Range: 0-255>,
          "points": {
             "<groupingName>": [ // Array of points with the same Fxt::Point type, e.g "analogInputs", "digitalOutputs", etc.
               {
                 "channel":             <reference to a physical terminal/connector as a number. Range: 1 to 64K>
                 "id":                  <Virtual Point ID.  Must be unique.  Range: 0-4GB>,
                 "ioRegId":             <The ID of the Point's IO register. Must be unique.  Range: 0-4GB>,
                 "name":                "*<human readable name for the point/channel/IO>"
                 "type":                "<Points's Type GUID: 8-4-4-4-12 format>",
                 "typeCfg:              <OPTIONAL type configuration for complex types, e.g. "typeCfg":{"numElems":2}>,
                 "typeName":            "*<OPTIONAL: human readable point type>",
                 "initial": {           // OPTIONAL initial value/state specifier for the Point
                   "valid": true|false  // Initial valid state for the internal point,
                   "val":               <point value as defined by the Point type's fromJSON syntax - only required when 'valid' is 'true' OR when 'valid' is ommitted>,
                   "id":                <The ID of the internal 'setter' point that is used store the initial value in binary form>
                 }
               },
             "<grouping2Name>":[{..},...]
          }
       }

    *The field is NOT parsed/used by the firmware

    \endcode
 */
class FactoryApi: public Cpl::Container::Item
{
public:
    /** This method creates a Card. The concrete type of the IO card is 
        dependent on the concrete Factory that is ultimately invoked to create 
        the IO card.

        The method returns a pointer to the created card when successful; else 
        if an error occurred (e.g. out-of-memory) nullptr is returned.
        When an error occurs, the 'cardErrorCode' argument is updated with 
        details of the error.
      */
    virtual Api* create( JsonVariant&                       cardObject, 
                         Fxt::Type::Error&                  cardErrorCode,
                         Cpl::Memory::ContiguousAllocator&  generalAllocator,
                         Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                         Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                         Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                         Fxt::Point::DatabaseApi&           dbForPoints  ) noexcept = 0;


    /** This method is used to destroy/free an IO card.  

        Note: The factory will call stop() on the specified IO card before 
              destroying.
      */
    virtual void destroy( Api& cardToDestory ) noexcept = 0;

public:
    /** This method returns the GUID for the type of IO card that the factory 
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
