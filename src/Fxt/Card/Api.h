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
#include "Fxt/Type/Error.h"
#include <stdint.h>



///
namespace Fxt {
///
namespace Card {


/** This abstract class defines the non-type specific operations that can be
    performed on an "IO Card".

    Note: IO Card semantics are NOT thread-safe.

    Note: IO Cards support being in a Container - however 'that' container
          is the Chassis Scanners.  This means if the application wants to maintain
          a list of IO Cards - it must provide its own wrapper.

 */
class Api : public Cpl::Container::Item
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
    /** This method is used by the Chassis Card scanner to update the its virtual
        input points (associated with the card) from the content of the IO
        Registers.

        Note: The Chassis Card scanner is responsible for ensuring the
              thread-safety/integrity of its virtual input points being
              updated.

        The method return false if unrecoverable error occurred; else true
        is returned.
     */
    virtual bool scanInputs() noexcept = 0;

    /** This method is used by the Chassis Card scanner to flush its virtual
        output points (associated with the card) to the card's IO Registers

        Note: The Chassis Card scanner is responsible for ensuring the
              thread-safety/integrity of its virtual output points being
              copied.

        The method return false if unrecoverable error occurred; else true
        is returned.
     */
    virtual bool flushOutputs() noexcept = 0;


public:
    /** This method returns the card's GUID (that identifies its type) as a
        text string in 8-4-4-4-12 format
     */
    virtual const char* getTypeGuid() const noexcept = 0;

    /// Returns the card's 'human readable' type name (note: this is NOT guaranteed to be unique)
    virtual const char* getTypeName() const noexcept = 0;

    /// This method returns the card's slot number within it chassis
    virtual uint8_t getSlotNumber() const noexcept = 0;

public:
    /** This method returns the current error state of the card.  A value
        of Fxt::Type::Err_T::SUCCESS indicates the card is operating properly
     */
    virtual Fxt::Type::Error getErrorCode() const noexcept = 0;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~Api() {}
};


};      // end namespaces
};
#endif  // end header latch
