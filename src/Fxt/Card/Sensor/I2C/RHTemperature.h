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


#include "Fxt/Point/Float.h"
#include "Fxt/Point/Bool.h"
#include "Fxt/Card/Common_.h"
#include "Cpl/Json/Arduino.h"
#include "Fxt/Card/StartStopAsync.h"
#include "Cpl/System/Timer.h"
#include "Driver/RHTemp/Api.h"
#include "Cpl/System/Mutex.h"

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

    The card requires a 'driver thread' to execute in independently of the calls to
    scanInputs() and flushOutputs().  This is so that the sensor can be scanned
    in the background so a 'current' value is available when scanInputs() is
    called.

    NOTES: 
        - The Factory class is responsible for providing the Mailbox
        - The application is responsible for providing the driver instance
          via the RHTemperatureDriver class.  The look-up for the driver 
          instance is based on slot number.

    \code

    JSON Definition
    --------------------
    {
      "name": "My RH/Temperature Card",                     // *Text label for the card
      "id": 0,                                              // *ID assigned to the card
      "type": "9fd17cc7-88c1-46bc-8a8c-6f76ab4e6eee",       // Identifies the card type.  Value comes from the Supported/Available-card-list
      "typename": "Fxt::Card::Sensor::I2C::RHTemperature",  // *Human readable type name
      "slot": <sensor slot>,                                // Physical identifier. The containing node dedicates what are the valid values for 'slot' (i.e. slot number maps to a specific driver instance)
      "driverInterval": <num_msec>,                         // Sampling rate/delay in milliseconds for the DRIVER level scanning.  The value must be >= 20ms
      "points": {
        "inputs": [                                         // Inputs.
          {
            "channel": 1                                    // Channel 1 represents/holds the RH value
            "id": 0,                                        // ID assigned to the Virtual Point that represents the input value
            "ioRegId": 0,                                   // The ID of the Point's IO register.
            "name": "RH Input value"                        // *Text label for the input signal
            "type": "708745fa-cef6-4364-abad-063a40f35cbc", // REQUIRED Type for the input signal
            "typeName": "Fxt::Point::float",                // *OPTIONAL: Human readable Type name for the input signal
            "initial": {
              "valid": true|false                           // Initial valid state for the IO Register point
              "val": <float>                                // Initial value for the input point. Only required when 'valid' is true
              "id": 0                                       // The ID of the internal point that is used store the initial value in binary form
            }
          },
          {
            "channel": 2                                    // Channel 2 represents/holds the Temperature value
            "id": 0,                                        // ID assigned to the Virtual Point that represents the input value
            "ioRegId": 0,                                   // The ID of the Point's IO register.
            "name": "Temperature Input value"               // *Text label for the input signal
            "type": "708745fa-cef6-4364-abad-063a40f35cbc", // REQUIRED Type for the input signal
            "typeName": "Fxt::Point::float",                // *OPTIONAL: Human readable Type name for the input signal
            "initial": {
              "valid": true|false                           // Initial valid state for the IO Register point
              "val": <float>                                // Initial value for the input point. Only required when 'valid' is true
              "id": 0                                       // The ID of the internal point that is used store the initial value in binary form
            }
          }
        ],
        "outputs": [                                        // OPTIONAL Outputs. 
          {
            "channel": 1                                    // Channel 1 represents the output for enabling/disabling the heater
            "id": 0,                                        // ID assigned to the Virtual Point that represents the output value
            "ioRegId": 0,                                   // The ID of the Point's IO register.
            "name": "Heater enable"                         // *Text label for the output signal
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // REQUIRED Type for the output signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the output signal
            "initial": {
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
class RHTemperature : public Fxt::Card::Common_, public Fxt::Card::StartStopAsync, public Cpl::System::Timer
{
public:
    /// Type ID for the card
    static constexpr const char*    GUID_STRING = "9fd17cc7-88c1-46bc-8a8c-6f76ab4e6eee";

    /// Type name for the card
    static constexpr const char*    TYPE_NAME   = "Fxt::Card::Sensor::I2C::RHTemperature";

    /// Size (in bytes) of Stateful data that will be allocated on the Card Heap
    static constexpr const size_t   CARD_STATEFUL_HEAP_SIZE = (2 * 2 * sizeof( Fxt::Point::Float::StateBlock_T ) + 
                                                                sizeof( Fxt::Point::Bool::StateBlock_T ));

    /// Size (in bytes) of Stateful data that will be allocated on the HA Heap
    static constexpr const size_t   HA_STATEFUL_HEAP_SIZE = (1 * sizeof( Fxt::Point::Bool::StateBlock_T ));

public:
    /// Constructor
    RHTemperature( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                   Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                   Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                   Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                   Fxt::Point::DatabaseApi&           dbForPoints,
                   JsonVariant&                       cardObject,
                   Cpl::Dm::MailboxServer*            cardMbox,
                   void*                              extraArgsNotUsed );

public:
    /// See Fxt::Card::Api (invokes the ITC start request)
    bool start( Cpl::Itc::PostApi& chassisMbox, uint64_t currentElapsedTimeUsec ) noexcept;

    /// See Fxt::Card::Api (invokes the ITC stop request)
    void stop( Cpl::Itc::PostApi& chassisMbox ) noexcept;

    /// See Fxt::Card::Api
    const char* getTypeGuid() const noexcept;

    /// See Fxt::Card::Api
    const char* getTypeName() const noexcept;

    /// See Fxt::Card::Api
    bool scanInputs( uint64_t currentElapsedTimeUsec ) noexcept;

    /// See Fxt::Card::Api
    bool flushOutputs( uint64_t currentElapsedTimeUsec ) noexcept;

public:
    /// ITC Start request (runs in the driver thread)
    void request( StartReqMsg& msg );

    /// ITC Stop request (runs in the driver thread)
    void request( StopReqMsg& msg );

public:
    /// ITC Start response (runs in the chassis thread)
    void response( StartRspMsg& msg );

    /// ITC Stop response (runs in the chassis thread)
    void response( StopRspMsg& msg );

protected:
    /// Helper method to parse the card's JSON config
    void parseConfiguration( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                             Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                             Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                             Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                             Fxt::Point::DatabaseApi&           dbForPoints,
                             JsonVariant&                       cardObject ) noexcept;

protected:
    /// Background polling timer expired
    void expired( void ) noexcept;

protected:
    /// Mutex need exchange data between the Chassis and Driver threads
    Cpl::System::Mutex      m_lock;

    /// Driver instance
    Driver::RHTemp::Api*    m_driver;

    /// Polling delay, in milliseconds. SHARED between threads - but is constant value - so no critical section is needed
    unsigned long           m_delayMs;

    /// Last sampled RH & Temperature values. SHARED between threads
    float                   m_samples[2];

    /// Point Index for the RH input. SHARED between threads - but is constant value - so no critical section is needed
    uint16_t                m_rhIndex;

    /// Point Index for the Temperature input. SHARED between threads - but is constant value - so no critical section is needed
    uint16_t                m_tempIndex;

    /// Flag that indicates I have valid sampled values. SHARED between threads
    bool                    m_validSamples;

    /// Pending Heater output value. SHARED between threads
    bool                    m_heaterEnable;

    /// Flag that indicate if there is pending update to the Heater output. SHARED between threads
    bool                    m_pendingHeaterUpdate;

    /// State of my underlying driver (this member is not used/accessed in the DRIVER Thread)
    bool                    m_driverStarted;

    /// Last known heater enable state (so we can detect changes - so we are constantly updating the sensor)
    bool                    m_lastHeaterEnabled;
};



};      // end namespaces
};
};
};
#endif  // end header latch
