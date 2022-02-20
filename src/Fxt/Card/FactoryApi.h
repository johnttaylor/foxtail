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
#include "Fxt/Point/RegisterBank.h"
#include "Fxt/Card/Api.h"
#include <stdint.h>
#include <stdlib.h>

///
namespace Fxt {
///
namespace Card {


/** This abstract class defines the interface for a 'Factory' that knows how
    to create a IO card of a specific type.

    The semantics of the Factory interface is NOT thread safe.
 */
class FactoryApi: public Cpl::Container::Item
{
public:
    /** This method creates and returns in instance of an IO Card.  The concrete
        type of the IO card is dependent on the concrete Factory that is ultimately
        invoked to create the IO card.

        Each individual Card Type defines the content/structure of its JSON
        'staticConfig' object.

        Each IO card is required to create internal Points for all of its
        'visible' inputs/outputs.  In addition, for the input and output sets
        of Points, the IO card is required to create the associated Points for
        the 'Register Banks' (i.e. a 1-to-1 mapping of internal points to 
        Register points).

        The 'startEndPointIdValue' is an INPUT to the function AND it is an
        OUTPUT.  For input it is the STARTING point ID to use when creating
        points.  For output it returns the 'next' PointId, i,e, the last
        assigned PointId plus one.  

        The method returns true when successful; else false (e.g. out-of-memory)
        is returned.
     */
    virtual bool create( Cpl::Memory::Allocator&    allocatorForCard, 
                         const char*                cardName,
                         uint16_t                   cardLocalId,
                         JsonVariant&               staticConfig,
                         Fxt::Point::BankApi&       internalInputsBank,
                         Fxt::Point::BankApi&       internalOutputsBank,
                         Fxt::Point::RegisterBank&  registerInputsBank,
                         Fxt::Point::RegisterBank&  registerOutputsBank,
                         uint32_t&                  startEndPointIdValue ) noexcept = 0;


    /** This method is used to destroy/free an IO card.  The same 'allocatorForCard'
        that was specified on the create() call must be supplied to the destroy() 
        call.

        Note: The factory will call stop() on the specified IO card before 
              destroying.
      */
    virtual void destroy( Api& cardToDestory, Cpl::Memory::Allocator& allocatorForCard ) noexcept = 0;


public:
    /// Virtual destructor to make the compiler happy
    virtual ~FactoryApi() {}
};


};      // end namespaces
};
#endif  // end header latch
