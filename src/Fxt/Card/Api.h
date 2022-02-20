#ifndef Fxt_Card_Api_h_
#define Fxt_Card_Api_h_
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
#include "Cpl/Type/Guid.h"
#include <stdint.h>

///
namespace Fxt {
///
namespace Card {


/** This abstract class defines the non-type specific operations that can be
    performed on an "IO Card".   

    Note: IO Card semantics are NOT thread-safe.
 */
class Api: public Cpl::Container::Item
{
public:
    /** This method is used to start/activate the IO card.  If the card fails
        to be started the method returns false; else true is returned.

        A card can be started/stopped multiple times. When a card is created
        it is in the 'stopped' state.
     */
    virtual bool start() noexcept = 0;

    /** This method is used to stop/deactivate the IO card.  If the card fails
        to be stopped the method returns false; else true is returned.

        A card MUST be in the 'stopped state' before it can be deleted/destroyed
     */
    virtual bool stop() noexcept = 0;

    /** This method returns true if the card is in the started state
     */
    virtual bool isStarted() const noexcept = 0;


public:
    /// This method returns the card's 'User facing local ID'
    virtual uint16_t getLocalId() const noexcept = 0;

    /// This method returns the card's text name/label (as defined by the user)
    virtual const char* getName() const noexcept = 0;

    /// This method returns the card's GUID (that identifies its type)
    virtual const Cpl::Type::Guid_T& getGuid() const noexcept = 0;

    /// Returns the card's 'human readable' type name (note: this is NOT guaranteed to be unique)
    virtual const char* getTypeName() const noexcept = 0;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~Api() {}
};


};      // end namespaces
};
#endif  // end header latch
