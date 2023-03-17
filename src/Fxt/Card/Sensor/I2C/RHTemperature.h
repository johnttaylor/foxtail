#ifndef Fxt_Card_Sensor_I2C_RHTemperature_h_
#define Fxt_Card_Sensor_I2C_RHTemperature_h_
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

///
namespace Fxt {
///
namespace Card {
///
namespace Sensor {
///
namespace I2C {


/** This concrete class implements an IO card that is a single RH/Temperature
    device connect to the Node via an I2C bus.  The card reports a single RH
    value (in %) and a single Temperature value (in degrees Centigrade).  In
    addition, the card supports enabling/disabling a on-board heating element
    to 'dry out' the sensor.  However, when the heater is on - the temperature
    reading is NOT accurate.


    \code

    JSON Definition
    --------------------
    {
      "name": "My Digital Card",                            // Text label for the card
      "id": 0,                                              // ID assigned to the card
      "type": "9fd17cc7-88c1-46bc-8a8c-6f76ab4e6eee",       // Identifies the card type.  Value comes from the Supported/Available-card-list
      "typename": "Fxt::Node::SBC::PiPicoDemo::Dio30",      // *Human readable type name
      "slot": 0,                                            // *Physical identifier, e.g. its the card position in the Node's physical chassis
      "points": {
        "inputs": [                                         // Inputs. The card supports 30 input points that are exposed as individual Bool points
          {
            "channel": 10                                   // Selects GPIO signal on the MCU. where n = GPIO signal + 1. Range is: 1 - 30. GPIO9 = 9+1
            "id": 0,                                        // ID assigned to the Virtual Point that represents the input value
            "ioRegId": 0,                                   // The ID of the Point's IO register.
            "name": "My input name"                         // Text label for the input signal
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // *REQUIRED Type for the input signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the input signal
            "initial": {
              "valid": true|false                           // Initial valid state for the IO Register point
              "val": <integer>                              // Initial value for the input point. Only required when 'valid' is true
              "id": 0                                       // The ID of the internal point that is used store the initial value in binary form
            }
          },
          {...}
        ],
        "outputs": [                                        // Outputs. The card supports 30 output points that is exposed individual Bool points
          {
            "channel": 26                                   // Selects GPIO signal on the MCU. where n = GPIO signal + 1. Range is: 1 - 30. GPIO25/LED = 25+1
            "id": 0,                                        // ID assigned to the Virtual Point that represents the output value
            "ioRegId": 0,                                   // The ID of the Point's IO register.
            "name": "My output name"                        // Text label for the output signal
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // *REQUIRED Type for the output signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the output signal
            "initial": {
              "valid": true|false                           // Initial valid state for the IO Register point
              "val": <integer>                              // Initial value for the input point. Only required when 'valid' is true
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
class RHTemperature : public Fxt::Card::Common_
{
public:
    /// Type ID for the card
    static constexpr const char*    GUID_STRING = "9fd17cc7-88c1-46bc-8a8c-6f76ab4e6eee";

    /// Type name for the card
    static constexpr const char*    TYPE_NAME   = "Fxt::Card::Sensor::I2C::RHTemperature";

    /// Size (in bytes) of Stateful data that will be allocated on the Card Heap
    static constexpr const size_t   CARD_STATEFUL_HEAP_SIZE = (1 * 3 * sizeof( Fxt::Point::Bool::StateBlock_T ));

    /// Size (in bytes) of Stateful data that will be allocated on the HA Heap
    static constexpr const size_t   HA_STATEFUL_HEAP_SIZE = (1 * sizeof( Fxt::Point::Bool::StateBlock_T ));

public:
    /// Constructor
    RHTemperature( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                   Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                   Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                   Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                   Fxt::Point::DatabaseApi&           dbForPoints,
                   JsonVariant&                       cardObject );

public:
    /// See Fxt::Card::Api
    bool start( uint64_t currentElapsedTimeUsec ) noexcept;

    /// See Fxt::Card::Api
    void stop() noexcept;

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

};



};      // end namespaces
};
};
};
#endif  // end header latch
