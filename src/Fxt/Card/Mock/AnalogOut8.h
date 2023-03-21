#ifndef Fxt_Card_Mock_AnalogOut8_h_
#define Fxt_Card_Mock_AnalogOut8_h_
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


#include "Fxt/Point/Float.h"
#include "Fxt/Card/Common_.h"
#include "Cpl/Json/Arduino.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/Dm/MailboxServer.h"

///
namespace Fxt {
///
namespace Card {
///
namespace Mock {


/** This concrete class implements a mocked/emulated IO card that
    supports 8 Analog Outputs.  The analog values are of type 'float' and
    have units of 'Volts'

    In general the implementation is NOT thread safe, however the get/set methods
    ARE thread safe to allow the application/test/console to drive/access the
    inputs/outputs.

    The class dynamically creates numerous thingys and objects when it is created.
    This memory is allocated via a 'Contiguous Allocator'.  This means the this
    class will NOT free the allocated memory in its destructor, it WILL however
    call the destructor on any objects it directly created using the allocator.
    The semantics with the Application is that the Application is RESPONSIBLE for
    freeing/recycling the memory in the Contiguous Allocator when Card(s) are
    deleted.

    \code

    JSON Definition
    --------------------
    {
      "name": "My AnalogOut8 Card",                         // *Text label for the card
      "id": 0,                                              // *ID assigned to the card
      "type": "dcec26d7-8dea-4e1b-946d-4a0313677173",       // Identifies the card type.  Value comes from the Supported/Available-card-list
      "typeName": "Fxt::Card::Mock::AnalogOut8",            // *Human readable type name
      "slot": 0,                                            // Physical identifier, e.g. its the card position in the Node's physical chassis
      "points": {
        "outputs": [                                        // Outputs. The card supports 8 output values. Each input is exposed a Float point
          {
            "channel": 1                                    // Range: 1 to 8.  (For a physical card this would map back to connector/terminal identifiers)
            "id": 0,                                        // ID assigned to the Virtual Point that represents the input value
            "ioRegId": 1,                                   // The ID of the Point's IO register.
            "name": "My output#1 name"                       // *Text label for the input signal
            "type": "708745fa-cef6-4364-abad-063a40f35cbc", // REQUIRED Type for the input signal
            "typeName": "Fxt::Point::Float",                // *OPTIONAL: Human readable Type name for the input signal
            "initial": {                                    // OPTIONAL initial value
              "valid": true|false                           // Initial valid state for the IO Register point
              "val": <float>                                // Initial value for the input point. Only required when 'valid' is true
              "id": 2                                       // The ID of the internal point that is used store the initial value in binary form
            }
          }
        ]
      }
    }

    *The field is NOT parsed/used by the firmware

    \endcode
 */
class AnalogOut8 : public Fxt::Card::Common_
{
public:
    /// Type ID for the card
    static constexpr const char*    GUID_STRING = "dcec26d7-8dea-4e1b-946d-4a0313677173";

    /// Type name for the card
    static constexpr const char*    TYPE_NAME   = "Fxt::Card::Mock::AnalogOut8";

    /// Size (in bytes) of Stateful data that will be allocated on the Card Heap
    static constexpr const size_t   CARD_STATEFUL_HEAP_SIZE = (8 * sizeof( Fxt::Point::Float::StateBlock_T ));

    /// Size (in bytes) of Stateful data that will be allocated on the HA Heap
    static constexpr const size_t   HA_STATEFUL_HEAP_SIZE = (8 * sizeof( Fxt::Point::Float::StateBlock_T ));

public:
    /// Constructor
    AnalogOut8( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                Fxt::Point::DatabaseApi&           dbForPoints,
                JsonVariant&                       cardObject,
                Cpl::Dm::MailboxServer*            cardMboxNotUsed = nullptr,
                void*                              extraArgsNotUsed = nullptr );

public:
    /// See Fxt::Card::Api
    bool start( Cpl::Itc::PostApi& chassisMbox, uint64_t currentElapsedTimeUsec ) noexcept;

    /// See Fxt::Card::Api
    void stop( Cpl::Itc::PostApi& chassisMbox ) noexcept;

    /// See Fxt::Card::Api
    bool scanInputs( uint64_t currentElapsedTimeUsec ) noexcept;

    /// See Fxt::Card::Api
    bool flushOutputs( uint64_t currentElapsedTimeUsec ) noexcept;

    /// See Fxt::Card::Api
    const char* getTypeGuid() const noexcept;

    /// See Fxt::Card::Api
    const char* getTypeName() const noexcept;

public:
    /** Returns (via the argument list) the current output value for a channel.
        If the 'channelNumber' is invalid - false is returned.
     */
    bool getOutputs( uint8_t channelNumber, float& dstValue, bool& dstIsValid );

protected:
    /// Helper method to parse the card's JSON config
    void parseConfiguration( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                             Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                             Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                             Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                             Fxt::Point::DatabaseApi&           dbForPoints,
                             JsonVariant&                       cardObject ) noexcept;



protected:
    /// Mutex to provide thread safety for the application driving/reading the mocked IO
    Cpl::System::Mutex  m_lock;

    /// Number of output channels
    uint16_t            m_numOutputs;

};



};      // end namespaces
};
};
#endif  // end header latch
