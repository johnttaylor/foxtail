#ifndef Fxt_Component_Digital_AndGateBase_h_
#define Fxt_Component_Digital_AndGateBase_h_
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
#include "Fxt/Point/BankApi.h"

///
namespace Fxt {
///
namespace Component {
///
namespace Digital {


/** This partially concrete class implements a Component that functions as AND gate
    for boolean inputs.  Up to N inputs are supported, and has two outputs. The
    first output is result of the AND operation and the second output is the
    logical complement of the AND output

    N is defined by the concrete child class.

    IF one or more of the Input signals are invalid, THEN the output signals
    are invalid.

    The component has NO stateful data

    \code

    JSON Definition
    --------------------
    {
       "name": "AND Gate#1"                                 // *Text label for the component
       "type": "e62e395c-d27a-4821-bba9-aa1e6de42a05",      // Identifies the card type.  Value comes from the Supported/Available-card-list
       "typeName": "Fxt::Component::Digital::And8Gate"      // *OPTIONAL: Human readable type name
       "inputs": [                                          // Array of Point references that supply the Component's input values.  Number of elements: 1-16
          {
            "name": "Signal A",                             // *human readable name for the input value
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // REQUIRED Type for the input signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the input signal
            "idRef": 4294967295                             // Point ID Reference to the point to read the input value from
          },
          ...
       ],
       "outputs": [                                         // Array of Point reference where the Component's writes it output values to.  Number of elements: 1-2
          {
            "name":"Product"                                // *Human readable name for the output signal
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // REQUIRED Type for the output signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the output signal
            "idRef":4294967295,                             // Point ID reference to the point that is updated with the output value
            "negate":false                                  // Optional: (defaults to false) When set to false - the output is result of ANDing the input signals.  When set to true the output is logical complement of ANDing result
          },
          {
            "name":"/Product"                               // *Human readable name for the output signal
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // REQUIRED Type for the output signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the output signal
            "idRef":4294967295,                             // Point ID reference to the point that is updated with the output value
            "negate":true                                   // Optional: (defaults to false) When set to false - the output is result of ANDing the input signals.  When set to true the output is logical complement of ANDing result
          }
       ]
    }

    *The field is NOT parsed/used by the firmware

    \endcode
 */
class AndGateBase : public Fxt::Component::Common_
{
public:
    /// Size (in bytes) of Stateful data that will be allocated on the HA Heap
    static constexpr const size_t   HA_STATEFUL_HEAP_SIZE = 0;

public:
    /// Maximum number of Output signals
    static constexpr unsigned MAX_OUTPUTS = 2;

protected:
    /// Constructor
    AndGateBase();

    /// Destructor
    ~AndGateBase();

public:
    /// See Fxt::Component::Api
    Fxt::Type::Error resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept;

    /// See Fxt::Component::Api
    Fxt::Type::Error execute( int64_t currentTickUsec ) noexcept;


protected:
    /// Helper method to parse the card's JSON config
    bool parseConfiguration( Cpl::Memory::ContiguousAllocator& generalAllocator, JsonVariant& obj, unsigned maxInputs ) noexcept;

protected:
    /// List of Input Points Pointers.  Note: Initially the point IDs are stored instead of pointers
    Fxt::Point::Bool**  m_inputRefs;

    /// List of Output Points Pointers. Note: Initially the point IDs are stored instead of pointers
    Fxt::Point::Bool**  m_outputRefs;

    /// List of Negate qualifier for the output points
    bool*               m_outputNegated;

    /// Number of Input points
    uint8_t             m_numInputs;

    /// Number of Output points
    uint8_t             m_numOutputs;
};



};      // end namespaces
};
};
#endif  // end header latch