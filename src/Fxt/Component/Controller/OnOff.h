#ifndef Fxt_Component_Controller_OnOff_h_
#define Fxt_Component_Controller_OnOff_h_
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


#include "Fxt/Component/Common_.h"
#include "Cpl/Json/Arduino.h"
#include "Fxt/Point/Bool.h"
#include "Fxt/Point/Uint64.h"
#include "Fxt/Point/BankApi.h"

///
namespace Fxt {
///
namespace Component {
///
namespace Controller {


/** This concrete class implements a Component that is a simple On/Off
    controller with optional hysteresis and optional minimum on/off times

    Inputs:
        Process Variable (PV)
        Setpoint (SP)
        Optional on-hysteresis (HON). Default is 0.
        Optional off-hysteresis (HOFF). Default is 0.
        Optional minimum on time (MON).  Default is 0. Units:= microseconds
        Optional minimum off time (MOFF).  Default is 0. Units:= microseconds
        Reset (RST).  A rising edge on this input resets the controller to it initial state.

        Note: PV, SP, Hon, and Hoff MUST all be the same numeric type.

    Outputs:
        On State (O)
        NOT On state (/O)

    Initial state:
        if ( PV > SP ) then O:= true else O:= false

    Logic:
        if ( PV > (SP + HON) AND deltaTime >= MON ) THEN O:= true
        if ( PV < (SP - HOFF) AND deltaTime >= MOFF ) THEN O:= false


    IF one or more of the Input signals are invalid, THEN the output signals
    are invalid.

    The component HAS stateful data
        Current State        (Bool)
        Timer Mark           (Uint64)
        Previous Reset Value (Bool)

    \code

    JSON Definition
    --------------------
    {
       "name": "Controller#1"                               // *Text label for the component
       "type": "fec7f73f-982b-4adb-a6c7-837a457b2822",      // Identifies the card type.  Value comes from the Supported/Available-card-list
       "typeName": "Fxt::Component::Controller::OnOff"      // *OPTIONAL: Human readable type name
       "inputs": [                                          // Array of Point references that supply the Component's input values.
          {
            "name": "indoor temp",                          // *Human readable name for the input signal
            "var": "PV",                                    // MUST BE: PV.  Process variable
            "type": "708745fa-cef6-4364-abad-063a40f35cbc", // REQUIRED Type for the input signal.  Can be Any numeric type.
            "typeName": "Fxt::Point::Float",                // *OPTIONAL: Human readable Type name for the input signal
            "idRef": 4294967295                             // Point ID Reference to the point to read the input value from
          },
          {
            "name": "Cooling setpoint",                     // *Human readable name for the input signal
            "var": "SP",                                    // MUST BE: SP.  Setpoint
            "type": "708745fa-cef6-4364-abad-063a40f35cbc", // REQUIRED Type for the input signal.  Can be Any numeric type.
            "typeName": "Fxt::Point::Float",                // *OPTIONAL: Human readable Type name for the input signal
            "idRef": 4294967295                             // Point ID Reference to the point to read the input value from
          },
          {                                                 // OPTIONAL
            "name": "On differential",                      // *Human readable name for the input signal
            "var": "HON",                                   // MUST BE: HON.  On hysteresis
            "type": "708745fa-cef6-4364-abad-063a40f35cbc", // REQUIRED Type for the input signal.  Can be Any numeric type.
            "typeName": "Fxt::Point::Float",                // *OPTIONAL: Human readable Type name for the input signal
            "idRef": 4294967295                             // Point ID Reference to the point to read the input value from
          },
          {                                                 // OPTIONAL
            "name": "Off differential",                     // *Human readable name for the input signal
            "var": "HOFF",                                  // MUST BE: HOFF.  Off hysteresis
            "type": "708745fa-cef6-4364-abad-063a40f35cbc", // REQUIRED Type for the input signal.  Can be Any numeric type.
            "typeName": "Fxt::Point::Float",                // *OPTIONAL: Human readable Type name for the input signal
            "idRef": 4294967295                             // Point ID Reference to the point to read the input value from
          },
          {                                                 // OPTIONAL
            "name": "Min On time",                          // *Human readable name for the input signal
            "var": "MON",                                   // MUST BE: MON.  Minimum ON time in microseconds
            "type": "708745fa-cef6-4364-abad-063a40f35cbc", // REQUIRED Type for the input signal.  Can be Any numeric type.
            "typeName": "Fxt::Point::Uint64",               // *OPTIONAL: Human readable Type name for the input signal
            "idRef": 4294967295                             // Point ID Reference to the point to read the input value from
          },
          {                                                 // OPTIONAL
            "name": "Min Off time",                         // *Human readable name for the input signal
            "var": "MOFF",                                   // MUST BE: MOFF.  Minimum OFF time in microseconds
            "type": "708745fa-cef6-4364-abad-063a40f35cbc", // REQUIRED Type for the input signal.  Can be Any numeric type.
            "typeName": "Fxt::Point::Uint64",               // *OPTIONAL: Human readable Type name for the input signal
            "idRef": 4294967295                             // Point ID Reference to the point to read the input value from
          },
          {                                                 // OPTIONAL
            "name":"Start-over"                             // *Human readable name for the input signal
            "var":"RST"                                     // MUST BE: RST.  Reset pulse (reset to initial state on the rising edge)
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // REQUIRED Type for the output signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the output signal
            "idRef":4294967295,                             // Point ID reference to the point that is updated with the output value
          }
       ],
       "outputs": [                                         // Array of Point reference where the Component's writes it output values to.  Number of elements: 1-2, must be at least one output point
          {                                                 // Optional
            "name":"Output"                                 // *Human readable name for the output signal
            "var":"O"                                       // MUST BE: O.  Is true when the controller is in the "On state"
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // REQUIRED Type for the output signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the output signal
            "idRef":4294967295,                             // Point ID reference to the point that is updated with the output value
          },
          {                                                 // Optional
            "name":"/Output"                                // Human readable name for the output signal
            "var":"/O"                                      // MUST BE: /O.  Is false when the controller is in the "On state"
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // REQUIRED Type for the output signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the output signal
            "idRef":4294967295,                             // Point ID reference to the point that is updated with the output value
          }
       ]
    }

    *The field is NOT parsed/used by the firmware


    \endcode
 */
class OnOff : public Fxt::Component::Common_
{
public:
    /// Type ID for the card
    static constexpr const char*    GUID_STRING = "fec7f73f-982b-4adb-a6c7-837a457b2822";

    /// Type name for the card
    static constexpr const char*    TYPE_NAME   = "Fxt::Component::Controller::OnOff";

    /// Number of internal Stateful points
    static constexpr const size_t   HA_STATEFUL_NUM_POINTS = 3;

    /// Size (in bytes) of Stateful data that will be allocated on the HA Heap
    static constexpr const size_t   HA_STATEFUL_HEAP_SIZE = (2 * sizeof( Fxt::Point::Bool::StateBlock_T ) + sizeof( Fxt::Point::Uint64::StateBlock_T) );

public:
    /// Maximum number of Input signals
    static constexpr unsigned MAX_INPUTS = 7;

    /// Maximum number of Output signals
    static constexpr unsigned MAX_OUTPUTS = 2;

public:
    /// Constructor
    OnOff( JsonVariant&                       componentObject,
           Cpl::Memory::ContiguousAllocator&  generalAllocator,
           Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
           Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
           Fxt::Point::DatabaseApi&           dbForPoints );

    /// Destructor
    ~OnOff();

public:
    /// See Fxt::Component::Api
    Fxt::Type::Error resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept;

    /// See Fxt::Component::Api
    Fxt::Type::Error execute( int64_t currentTickUsec ) noexcept;

public:
    /// See Fxt::Component::Api
    const char* getTypeGuid() const noexcept;

    /// See Fxt::Component::Api
    const char* getTypeName() const noexcept;


protected:
    /// Helper method to parse the card's JSON config
    bool parseConfiguration( Cpl::Memory::ContiguousAllocator& haStatefulDataAllocator, JsonVariant& obj ) noexcept;

protected:
    /// List of Input Points Pointers.  Note: Initially the point IDs are stored instead of pointers
    Fxt::Point::Api*    m_inputRefs[MAX_INPUTS];

    /// List of Output Points Pointers. Note: Initially the point IDs are stored instead of pointers
    Fxt::Point::Bool*   m_outputRefs[MAX_OUTPUTS];

    /// List of stateful points/internal-data
    Fxt::Point::Api*    m_statefulPoints[HA_STATEFUL_NUM_POINTS];

    /// Number of Input points
    uint8_t             m_numInputs;

    /// Number of Output points
    uint8_t             m_numOutputs;
};



};      // end namespaces
};
};
#endif  // end header latch