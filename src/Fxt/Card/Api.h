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


/// ERROR Code: NO Error, i.e. the card is operating properly
#define FXT_CARD_ERR_NO_ERROR                   0

/// ERROR Code: Unable to allocate memory for the card's Point Banks
#define FXT_CARD_ERR_MEMORY_POINT_BANKS         1

/// ERROR Code: Unable to allocate memory for the card's Input Point Descriptors
#define FXT_CARD_ERR_MEMORY_INPUT_DESCRIPTORS   2

/// ERROR Code: Unable to allocate memory for the card's Output Point Descriptors
#define FXT_CARD_ERR_MEMORY_OUTPUT_DESCRIPTORS  3

/// ERROR Code: Configuration contains the wrong GUID (i.e. the JSON object calls out a different card type)
#define FXT_CARD_ERR_GUID_WRONG_TYPE            4

/// ERROR Code: Configuration does NOT contain a LocalId value
#define FXT_CARD_ERR_CARD_MISSING_LOCAL_ID      5

/// ERROR Code: Unable to allocate memory for the card's name
#define FXT_CARD_ERR_MEMORY_CARD_NAME           6

/// ERROR Code: Configuration does NOT contain a LocalId value for one of it Points
#define FXT_CARD_ERR_POINT_MISSING_LOCAL_ID     7

/// ERROR Code: Configuration contains TOO many input Points (max is 32)
#define FXT_CARD_ERR_TOO_MANY_INPUT_POINTS      8

/// ERROR Code: Configuration contains TOO many output Points (max is 32)
#define FXT_CARD_ERR_TOO_MANY_OUTPUT_POINTS     9

/// ERROR Code: Configuration contains duplicate or our-of-range Channel IDs
#define FXT_CARD_ERR_BAD_CHANNEL_ASSIGNMENTS    10

/// ERROR Code: Configuration does NOT contain a Slot ID
#define FXT_CARD_ERR_CARD_MISSING_SLOT_ID       11

/// ERROR Code: Configuration does NOT contain a Card name
#define FXT_CARD_ERR_CARD_MISSING_NAME          12

/// ERROR Code: Code to use for the start of card-specific Error codes
#define FXT_CARD_ERR_START_CARD_SPECIFIC        100


///
namespace Fxt {
///
namespace Card {


/** This abstract class defines the non-type specific operations that can be
    performed on an "IO Card".

    Note: IO Card semantics are NOT thread-safe.

    Note: IO Cards support being in a Container - however 'that' container
          is the Card Database.  This means if the application wants to maintain
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
    /// This method returns the card's 'User facing local ID'
    virtual uint32_t getLocalId() const noexcept = 0;

    /// This method returns the card's text name/label (as defined by the user)
    virtual const char* getName() const noexcept = 0;

    /** This method returns the card's GUID (that identifies its type) as a
        text string in 8-4-4-4-12 format
     */
    virtual const char* getGuid() const noexcept = 0;

    /// Returns the card's 'human readable' type name (note: this is NOT guaranteed to be unique)
    virtual const char* getTypeName() const noexcept = 0;

    /// Returns the card's physical identifier in the Node's physical chassis
    virtual uint16_t getSlot() const noexcept = 0;

public:
    /** This method returns the current error state/number of the card.  A value
        of zero/ERR_NO_ERROR indicates the card is operating properly
     */
    virtual uint32_t getErrorCode() const noexcept = 0;

    /** This method returns a text string associated with the specified
        error code.  If the specified error code is not-valid/out-of-range for
        the card, a null pointer is returned.
     */
    virtual const char* getErrorText( uint32_t errCode ) const noexcept;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~Api() {}
};


};      // end namespaces
};
#endif  // end header latch
