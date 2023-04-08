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
#include "Fxt/Card/IoFlushAsync.h"
#include "Fxt/Card/IoScanAsync.h"
#include "Cpl/System/Timer.h"
#include "Driver/RHTemp/Api.h"

/// Minimum driver-background sampling time in milliseconds
#ifndef OPTION_FXT_CARD_SENSOR_I2C_MIN_DRIVER_SAMPLE_TIME_MS
#define OPTION_FXT_CARD_SENSOR_I2C_MIN_DRIVER_SAMPLE_TIME_MS        100
#endif

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

    CARD OUTPUTS/INVALID-STATE:
        The card sets its outputs (i.e. heater-enabled) to false/off when the
        Output IO Register is in the invalid state.

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
      "typeName": "Fxt::Card::Sensor::I2C::RHTemperature",  // *Human readable type name
      "slot": <sensor slot>,                                // Physical identifier. The containing node dedicates what are the valid values for 'slot' (i.e. slot number maps to a specific driver instance)
      "driverInterval": <num_msec>,                         // Sampling rate/delay in milliseconds for the DRIVER level scanning.  The value must be >= 100ms
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
class RHTemperature : 
    public Fxt::Card::Common_, 
    public Fxt::Card::StartStopAsync,
    public Fxt::Card::IoFlushAsync,
    public Fxt::Card::IoScanAsync,
    public Cpl::System::Timer
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

    /// ITC Start response (runs in the chassis thread)
    void response( StartRspMsg& msg );

public:
    /// ITC Stop request (runs in the driver thread)
    void request( StopReqMsg& msg );

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
    /// Data structure used to transfer INPUT data between the chassis and driver threads
    struct InputData_T
    {
        float values[2];    //!< Contains RH and Temp values.  The order of the parameter are the SAME order as IO Register indexes
        bool  isValid[2];   //!< Valid state of the incoming values.  Order is the same as 'dataValues'
    };

    /// See Fxt::Card::IoFlushAsync
    void populateInputsTransferBuffer() noexcept;

    /// See Fxt::Card::IoFlushAsync
    void extractInputsTransferBuffer() noexcept;


protected:
    /// Data structure used to transfer OUTPUT data between the chassis and driver threads
    struct OutputData_T
    {
        bool values[1];    //!< Contains Heater enable value.
    };

    /// See Fxt::Card::IoFlushAsync
    void popuplateOutputsTransferBuffer() noexcept;

    /// See Fxt::Card::IoFlushAsync
    void extractOutputsTransferBuffer() noexcept;

protected:
    /// Background polling timer expired
    void expired( void ) noexcept;

protected:
    /// Driver instance
    Driver::RHTemp::Api*    m_driver;

    /// Polling delay, in milliseconds. 
    unsigned long           m_delayMs;

    /// Point Index for the RH input. 
    uint16_t                m_rhIndex;

    /// Point Index for the Temperature input. 
    uint16_t                m_tempIndex;


    /// Buffer used to transfer the cached Input values to the card/chassis. 
    InputData_T             m_transferInputsBuffer;

    /// Driver: latest input values
    InputData_T             m_drvReceivedValues;


    /// Buffer used to transfer the cached Output values to the driver. 
    OutputData_T            m_transferOutputsBuffer;

    /// Driver: Output value for heater
    OutputData_T            m_drvCommandedValues;

    
    /// Flag to force the heater state to update on start-up. 
    bool                    m_drvForceHeaterUpdate;

    /// Driver: Last heater enabled value sent to the driver.  
    bool                    m_drvLastHeaterEnable;
};



};      // end namespaces
};
};
};
#endif  // end header latch
