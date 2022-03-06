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
#include "Cpl/Container/Dictionary.h"
#include "Fxt/Point/Bank.h"
#include "Fxt/Card/Api.h"
#include "Fxt/Card/Banks.h"
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
          is the Card Database.  This means if the application what to maintain
          a list of factories - it must provide its own wrapper.
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

        NOTE: Only the 'Virtual Points' are accessible via Point Descriptors/
              'User Facing Local IDs'.

        Note: The IO Card is responsible for allocating the memory for the
              Point Descriptors and the memory must stay in scope until the
              IO card is destroyed.
 
        The method returns true when successful; else false (e.g. out-of-memory)
        is returned.
      */
    virtual bool create( JsonVariant& cardObject ) noexcept = 0;


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
