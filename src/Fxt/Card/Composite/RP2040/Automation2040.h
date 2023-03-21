#ifndef Fxt_Card_Composite_RP2040_Automation2040_h_
#define Fxt_Card_Composite_RP2040_Automation2040_h_
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
#include "Fxt/Point/Float.h"
#include "Fxt/Card/Common_.h"
#include "Cpl/Json/Arduino.h"
#include "Driver/Automation2040/Api.h"
#include "Cpl/Dm/MailboxServer.h"

///
namespace Fxt {
///
namespace Card {
///
namespace Composite {
///
namespace RP2040 {


/** This concrete class implements an IO card that supports the basic GPIO and
    Analog inputs on the Pimoroni Automation2040W board.

    Note: The IO operations (aka the low level driver) for the card execute
          in the chassis thread.

    \code

    JSON Definition
    --------------------
    {
      "name": "My Digital Card",                            // *Text label for the card
      "id": 0,                                              // *ID assigned to the card
      "type": "d0580e5c-ce7b-47a7-b8de-e43e6ad68c49",       // Identifies the card type.  Value comes from the Supported/Available-card-list
      "typeName": "Fxt::Card::Composite::RP2040::Automation2040",         // *Human readable type name
      "slot": 0,                                            // MUST be set to zero
      "points": {
        "inputs": [                                         // OPTIONAL: Inputs. The application can use none, some, or all of the Available inputs
          {
            "channel": 1|2|3|4                              // Buffered Digital Inputs #1 - #4
            "id": 0,                                        // ID assigned to the Virtual Point that represents the input value
            "ioRegId": 0,                                   // The ID of the Point's IO register.
            "name": "My input name"                         // *Text label for the input signal
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // REQUIRED Type for the input signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the input signal
            "initial": {                                    // OPTIONAL
              "valid": true|false                           // Initial valid state for the IO Register point
              "val": true|false                             // Initial value for the input point. Only required when 'valid' is true
              "id": 0                                       // The ID of the internal point that is used store the initial value in binary form
            }
          },
          {
            "channel": 11|12|13                             // Analog Inputs #1 - #3.  Units: Volts. Range: 0 to 40V
            "id": 0,                                        // ID assigned to the Virtual Point that represents the input value
            "ioRegId": 0,                                   // The ID of the Point's IO register.
            "name": "My input name"                         // *Text label for the input signal
            "type": "708745fa-cef6-4364-abad-063a40f35cbc", // REQUIRED Type for the input signal
            "typeName": "Fxt::Point::Float",                // *OPTIONAL: Human readable Type name for the input signal
            "initial": {                                    // OPTIONAL
              "valid": true|false                           // Initial valid state for the IO Register point
              "val": <float>                                // Initial value for the input point. Only required when 'valid' is true
              "id": 0                                       // The ID of the internal point that is used store the initial value in binary form
            }
          },
          {
            "channel": 21|22                                // On-board Momentary buttons: ButtonA = channel 21, ButtonB = channel 22. True:= button pressed.
            "id": 0,                                        // ID assigned to the Virtual Point that represents the input value
            "ioRegId": 0,                                   // The ID of the Point's IO register.
            "name": "My input name"                         // *Text label for the input signal
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // REQUIRED Type for the input signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the input signal
            "initial": {                                    // OPTIONAL
              "valid": true|false                           // Initial valid state for the IO Register point
              "val": true|false                             // Initial value for the input point. Only required when 'valid' is true. True:= button pressed
              "id": 0                                       // The ID of the internal point that is used store the initial value in binary form
            }
          },
          {
            "channel": 31                                   // On-board temperature. Units: degrees Centigrade.  Note: Not very accurate reading
            "id": 0,                                        // ID assigned to the Virtual Point that represents the input value
            "ioRegId": 0,                                   // The ID of the Point's IO register.
            "name": "My input name"                         // *Text label for the input signal
            "type": "708745fa-cef6-4364-abad-063a40f35cbc", // REQUIRED Type for the input signal
            "typeName": "Fxt::Point::Float",                // *OPTIONAL: Human readable Type name for the input signal
            "initial": {                                    // OPTIONAL
              "valid": true|false                           // Initial valid state for the IO Register point
              "val": <float>                                // Initial value for the input point. Only required when 'valid' is true
              "id": 0                                       // The ID of the internal point that is used store the initial value in binary form
            }
          },
       ],
        "outputs": [                                        // OPTIONAL: Outputs. The application can use none, some, or all of the Available outputs
          {
            "channel": 1|2|3                                // Current Sourcing Digital Outputs #1 - #3. True:=signal high
            "id": 0,                                        // ID assigned to the Virtual Point that represents the output value
            "ioRegId": 0,                                   // The ID of the Point's IO register.
            "name": "My output name"                        // *Text label for the output signal
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // REQUIRED Type for the output signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the output signal
            "initial": {                                    // OPTIONAL
              "valid": true|false                           // Initial valid state for the IO Register point
              "val": true|false                             // Initial value for the input point. Only required when 'valid' is true
              "id": 0                                       // The ID of the internal point that is used store the initial value in binary form
            }
          },
         {
            "channel": 11|12|13                             // Relay Outputs #1 - #3. True:= Relay Actuated
            "id": 0,                                        // ID assigned to the Virtual Point that represents the output value
            "ioRegId": 0,                                   // The ID of the Point's IO register.
            "name": "My output name"                        // *Text label for the output signal
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // REQUIRED Type for the output signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the output signal
            "initial": {                                    // OPTIONAL
              "valid": true|false                           // Initial valid state for the IO Register point
              "val": true|false                             // Initial value for the input point. Only required when 'valid' is true
              "id": 0                                       // The ID of the internal point that is used store the initial value in binary form
            }
          },
          {
            "channel": 21|22                                // LED for On-board Momentary buttons: ButtonA LED = channel 21, ButtonB LED = channel 22. True:= LED On
            "id": 0,                                        // ID assigned to the Virtual Point that represents the output value
            "ioRegId": 0,                                   // The ID of the Point's IO register.
            "name": "My output name"                        // *Text label for the output signal
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // REQUIRED Type for the output signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the output signal
            "initial": {                                    // OPTIONAL
              "valid": true|false                           // Initial valid state for the IO Register point
              "val": true|false                             // Initial value for the input point. Only required when 'valid' is true
              "id": 0                                       // The ID of the internal point that is used store the initial value in binary form
            }
          },
          {
            "channel": 31                                   // 'Connected' LED. True:= LED On
            "id": 0,                                        // ID assigned to the Virtual Point that represents the output value
            "ioRegId": 0,                                   // The ID of the Point's IO register.
            "name": "My output name"                        // *Text label for the output signal
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // REQUIRED Type for the output signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the output signal
            "initial": {                                    // OPTIONAL
              "valid": true|false                           // Initial valid state for the IO Register point
              "val": true|false                             // Initial value for the input point. Only required when 'valid' is true
              "id": 0                                       // The ID of the internal point that is used store the initial value in binary form
            }
          }
       ]
      }
    }

    *The field is NOT parsed/used by the firmware

    \endcode
 */
class Automation2040 : public Fxt::Card::Common_
{
public:
    /// Type ID for the card
    static constexpr const char*    GUID_STRING = "d0580e5c-ce7b-47a7-b8de-e43e6ad68c49";

    /// Type name for the card
    static constexpr const char*    TYPE_NAME   = "Fxt::Card::Composite::RP2040::Automation2040";

    /// Size (in bytes) of Stateful data that will be allocated on the Card Heap
    static constexpr const size_t   CARD_STATEFUL_HEAP_SIZE = ((2 * 6 * sizeof( Fxt::Point::Bool::StateBlock_T )) +     // Digital Inputs  (VPoints + IO Registers)
                                                                (2 * 4 * sizeof( Fxt::Point::Float::StateBlock_T )) +   // Analog Inputs   (VPoints + IO Registers)
                                                                (9 * sizeof( Fxt::Point::Bool::StateBlock_T )));        // Digital Outputs (IO Registers)

    /// Size (in bytes) of Stateful data that will be allocated on the HA Heap
    static constexpr const size_t   HA_STATEFUL_HEAP_SIZE = (9 * sizeof( Fxt::Point::Bool::StateBlock_T ));             // Digital Outputs (VPoints)

public:
    /// Constructor
    Automation2040( Cpl::Memory::ContiguousAllocator&  generalAllocator,
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

    /// Helper method that populates the channel-num-TO-driver-call map
    bool updateInputMap( size_t ioRegisterIdx, uint16_t channelNum );

    /// Helper method that populates the channel-num-TO-driver-call map
    bool updateOutputMap( size_t ioRegisterIdx, uint16_t channelNum );


protected:
    /// Internal function signature for reading driver inputs
    typedef void (*DriverInputFunc_T)(Fxt::Point::Api* dstPt, int inIdentifier);
    
    /// Internal function signature for writing driver outputs
    typedef void (*DriverOutputFunc_T)(int outIdentifier, bool outValue);

    /// Internal Structure used when mapping card channel numbers to specific driver input calls
    struct DriverReader_T
    {
        DriverInputFunc_T func;         //!< Function that calls into the driver
        int               inputId;      //!< Identifies which driver IO identifier used when calling the driver
    };

    /// Internal Structure used when mapping card channel numbers to specific driver output calls
    struct DriverWriter_T
    {
        DriverOutputFunc_T func;        //!< Function that calls into the driver
        int                outputId;    //!< Identifies which driver IO identifier used when calling the driver
    };

protected:
    /// Maximum number of Input channels
    static constexpr const unsigned   MAX_INPUT_CHANNELS = 6 + 3 + 1;

    /// Maximum number of Output channels
    static constexpr const unsigned   MAX_OUTPUT_CHANNELS = 3 + 3 + 2 + 1;

    /// Maps IO Register index to driver inputs
    DriverReader_T  m_inputMap[MAX_INPUT_CHANNELS];

    /// Maps IO Register index to driver outputs
    DriverWriter_T  m_outputMap[MAX_OUTPUT_CHANNELS];

    /// Number of input channels
    uint16_t    m_numInputs;

    /// Number of output channels
    uint16_t    m_numOutputs;
};



}       // end namespaces
}
}
}
#endif  // end header latch
