#ifndef Fxt_Component_Digital_And16Gate_h_
#define Fxt_Component_Digital_And16Gate_h_
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

///
namespace Fxt {
///
namespace Component {
///
namespace Digital {


/** This concrete class implements a Component that functions as AND gate
    for boolean inputs.  Up to 16 inputs are support, and has two outputs. The
    first output is result of the AND operation and the second output is the
    logical complement of the AND output

    IF one or more of the Input signals are invalid, THEN the output signals
    are invalid.

    The component has NOT stateful data

    \code

    JSON Definition
    --------------------
    {
       "name": "AND Gate#1"                                 // Text label for the component
       "type": "e62e395c-d27a-4821-bba9-aa1e6de42a05",      // Identifies the card type.  Value comes from the Supported/Available-card-list
       "typeName": "16 Input AND Gate"                      // Human readable type name
       "exeOrder": 0                                        // Execution order within the Logic Chain. Range: 0-64K
       "inputs": [                                          // Array of Point references that supply the Component's input values.  Number of elements: 1-16
          {
            "name": "Signal A",                             // human readable name for the input value
            "type": "Fxt::Point::Bool",                     // REQUIRED Type for the input signal
            "idRef": 4294967295                             // Point ID Reference to the point to read the input value from
          },
          ...
       ],
       "outputs": [                                         // Array of Point reference where the Component's writes it output values to.  Number of elements: 1-2
            {                                               
              "name":"Product"                              // Human readable name for the output signal
              "type":"Fxt::Point::Bool",                    // REQUIRED Type for the output signal,
              "idRef":4294967295,                           // Point ID reference to the point that is updated with the output value
              "negate":false                                // Optional: (defaults to false) When set to false - the output is result of ANDing the input signals.  When set to true the output is logical complement of ANDing result
            },
            {                                       
              "name":"/Product"                             // Human readable name for the output signal
              "type":"Fxt::Point::Bool",                    // REQUIRED Type for the output signal,
              "idRef":4294967295,                           // Point ID reference to the point that is updated with the output value
              "negate":true                                 // Optional: (defaults to false) When set to false - the output is result of ANDing the input signals.  When set to true the output is logical complement of ANDing result
            }
       ]
    }


    \endcode
 */
class And16Gate : public Fxt::Component::Common_
{
public:
    /// Type ID for the card
    static constexpr const char* GUID_STRING = "e62e395c-d27a-4821-bba9-aa1e6de42a05";

    /// Type name for the card
    static constexpr const char* TYPE_NAME   = "Fxt::Component::Digital::And16Gate";

public:
    /// Maximum number of Input signals
    static constexpr unsigned MAX_INPUTS = 16;

    /// Maximum number of Output signals
    static constexpr unsigned MAX_OUTPUTS = 2;

public:
    /// Constructor
    And16Gate( uint16_t     exeOrder,
               JsonVariant& componentObject );

    /// Destructor
    ~And16Gate();

public:
    /// See Fxt::Component::Api
    bool resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept;

    /// See Fxt::Component::Api
    bool execute( int64_t currentTickUsec, Err_T& componentErrorCode ) noexcept;

public:
    /// See Fxt::Component::Api
    const char* getTypeGuid() const noexcept;

    /// See Fxt::Component::Api
    const char* getTypeName() const noexcept;


protected:
    /// Helper method to parse the card's JSON config
    virtual bool parseConfiguration( JsonVariant& obj ) noexcept;


protected:
    /// List of Input Points.  Note: Initially the point IDs are stored instead of pointers
    Fxt::Point::Bool*   m_inputRefs[MAX_INPUTS];

    /// List of Output Points. Note: Initially the point IDs are stored instead of pointers
    Fxt::Point::Bool*   m_outputRefs[MAX_OUTPUTS];

    /// Negate qualifier for the output points
    bool                m_outputNegated[MAX_OUTPUTS];

    /// Number of Input points
    uint8_t             m_numInputs;

    /// Number of Output points
    uint8_t             m_numOutputs;
};



};      // end namespaces
};
};
#endif  // end header latch
