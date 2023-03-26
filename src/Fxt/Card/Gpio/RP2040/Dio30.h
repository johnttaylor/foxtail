#ifndef Fxt_Card_Gpio_RP2040_Dio30_h_
#define Fxt_Card_Gpio_RP2040_Dio30_h_
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


#include "Fxt/Point/Bool.h"
#include "Fxt/Card/Common_.h"
#include "Cpl/Json/Arduino.h"
#include "Driver/DIO/InOut.h"
#include "Cpl/Dm/MailboxServer.h"

///
namespace Fxt {
///
namespace Card {
///
namespace Gpio {
///
namespace RP2040 {


/** This concrete class implements an IO card that supports 30 digital inputs and
    30 digital outputs.

    Note: The IO operations (aka the low level driver) for the card execute
          in the chassis thread.

    \code

    JSON Definition
    --------------------
    {
      "name": "My Digital Card",                            // *Text label for the card
      "id": 0,                                              // *ID assigned to the card
      "type": "c896faf0-6ea2-47d6-a1a6-7e4074c32a43",       // Identifies the card type.  Value comes from the Supported/Available-card-list
      "typeName": "Fxt::Card::Gpio::RP2040::Dio30",         // *Human readable type name
      "slot": 0,                                            // Physical identifier, e.g. its the card position in the Node's physical chassis
      "points": {
        "inputs": [                                         // OPTIONAL: Inputs. The card supports 30 input points that are exposed as individual Bool points
          {
            "channel": 10                                   // Selects GPIO signal on the MCU. where n = GPIO signal + 1. Range is: 1 - 30. GPIO9 = 9+1
            "id": 0,                                        // ID assigned to the Virtual Point that represents the input value
            "ioRegId": 0,                                   // The ID of the Point's IO register.
            "name": "My input name"                         // *Text label for the input signal
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // REQUIRED Type for the input signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the input signal
            "pulls: "none|up|down",                         // OPTIONAL: Internal Pull up/down resistor configuration for the channel/pin.  If not specified - the default is "none"
            "initial": {
              "valid": true|false                           // Initial valid state for the IO Register point
              "val": true|false                             // Initial value for the input point. Only required when 'valid' is true
              "id": 0                                       // The ID of the internal point that is used store the initial value in binary form
            }
          },
          {...}
        ],
        "outputs": [                                        // OPTIONAL: Outputs. The card supports 30 output points that is exposed individual Bool points
          {
            "channel": 26                                   // Selects GPIO signal on the MCU. where n = GPIO signal + 1. Range is: 1 - 30. GPIO25/LED = 25+1
            "id": 0,                                        // ID assigned to the Virtual Point that represents the output value
            "ioRegId": 0,                                   // The ID of the Point's IO register.
            "name": "My output name"                        // *Text label for the output signal
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // REQUIRED Type for the output signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the output signal
            "pulls: "none|up|down",                         // OPTIONAL: Internal Pull up/down resistor configuration for the channel/pin.  If not specified - the default is "none"
            "initial": {
              "valid": true|false                           // Initial valid state for the IO Register point
              "val": true|false                             // Initial value for the input point. Only required when 'valid' is true
              "id": 0                                       // The ID of the internal point that is used store the initial value in binary form
            }
          },
          {...}
        ]
      }
    }

    *The field is NOT parsed/used by the firmware

    \endcode
 */
class Dio30 : public Fxt::Card::Common_
{
public:
    /// Type ID for the card
    static constexpr const char*    GUID_STRING = "c896faf0-6ea2-47d6-a1a6-7e4074c32a43";

    /// Type name for the card
    static constexpr const char*    TYPE_NAME   = "Fxt::Card::Gpio::RP2040::Dio30";

    /// Maximum number of Input channels
    static constexpr const size_t   MAX_INPUT_CHANNELS  = 30;

    /// Maximum number of Output channels
    static constexpr const size_t   MAX_OUTPUT_CHANNELS = 30;

    /// Size (in bytes) of Stateful data that will be allocated on the Card Heap
    static constexpr const size_t   CARD_STATEFUL_HEAP_SIZE = (3 * MAX_INPUT_CHANNELS * sizeof( Fxt::Point::Bool::StateBlock_T ));

    /// Size (in bytes) of Stateful data that will be allocated on the HA Heap
    static constexpr const size_t   HA_STATEFUL_HEAP_SIZE = (MAX_OUTPUT_CHANNELS * sizeof( Fxt::Point::Bool::StateBlock_T ));


public:
    /// Constructor
    Dio30( Cpl::Memory::ContiguousAllocator&  generalAllocator,
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
    const char* getTypeGuid() const noexcept;

    /// See Fxt::Card::Api
    const char* getTypeName() const noexcept;

    /// See Fxt::Card::Api
    bool scanInputs( uint64_t currentElapsedTimeUsec ) noexcept;

    /// See Fxt::Card::Api
    bool flushOutputs( uint64_t currentElapsedTimeUsec ) noexcept;


protected:
    /// Helper method to parse the card's JSON config
    void parseConfiguration( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                             Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                             Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                             Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                             Fxt::Point::DatabaseApi&           dbForPoints,
                             JsonVariant&                       cardObject ) noexcept;

    /// Helper method to parse the Pull-up resistor option
    void parseDriverConfig( JsonObject& channelObj, Driver::DIO::InOut::Config_T cfg[], size_t arrayIdx, uint16_t channelNum );

protected:
    /// Driver configuration: Inputs
    Driver::DIO::InOut::Config_T m_driverInCfg[30];

    /// Driver configuration: Outputs
    Driver::DIO::InOut::Config_T m_driverOutCfg[30];
};



}       // end namespaces
}
}
}
#endif  // end header latch
