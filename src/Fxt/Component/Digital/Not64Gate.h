#ifndef Fxt_Component_Digital_Not64Gate_h_
#define Fxt_Component_Digital_Not64Gate_h_
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


/** This concrete class implements a Component that functions as NOT gate
    for boolean inputs.  Up to 16 inputs (with corresponding outputs) are 
    supported. In addition, the output can be configured as logical complement
    of the NOT operation, i.e. simple pass through of the input boolean.

    IF the Input signal is invalid, THEN its corresponding output signal will
    be invalid.

    The component has NO stateful data

    \code

    JSON Definition
    --------------------
    {
       "name": "NOT Gate#1"                                 // *Text label for the component
       "type": "31d8a613-bc99-4d0d-a96f-4b4dc9b0cc6f",      // Identifies the card type.  Value comes from the Supported/Available-card-list
       "typeName": "Fxt::Component::Digital::Not64Gate"     // *OPTIONAL: Human readable type name
       "inputs": [                                          // Array of Point references that supply the Component's input values.  Number of elements: 1-16
          {                                                 // NOTE: Order of the Inputs MUST MATCH the Order of the Outputs
            "name": "Signal A",                             // *human readable name for the input value
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // REQUIRED Type for the input signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the input signal
            "idRef": 4294967295                             // Point ID Reference to the point to read the input value from
          },
          ...
       ],
       "outputs": [                                         // Array of Point reference where the Component's writes it output values to.  Number of elements: 1-16
          {                                                 // NOTE: order is IMPORTANT (see comments on the Inputs)
            "name":"/Signal A"                              // *Human readable name for the output signal
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // REQUIRED Type for the output signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the output signal
            "idRef":4294967295,                             // Point ID reference to the point that is updated with the output value
            "negate":false                                  // OPTIONAL: (defaults to false) When set to false, the output:= !input. When set to true, the output:= input
          },
          ...
       ]
    }

    *The field is NOT parsed/used by the firmware

    \endcode
 */
class Not64Gate : public Fxt::Component::Common_
{
public:
    /// Type ID for the card
    static constexpr const char*    GUID_STRING = "31d8a613-bc99-4d0d-a96f-4b4dc9b0cc6f";

    /// Type name for the card
    static constexpr const char*    TYPE_NAME   = "Fxt::Component::Digital::Not64Gate";

    /// Size (in bytes) of Stateful data that will be allocated on the HA Heap
    static constexpr const size_t   HA_STATEFUL_HEAP_SIZE = 0;

public:
    /// Maximum number of Input signals
    static constexpr unsigned MAX_INPUTS = 64;

    /// Maximum number of Output signals
    static constexpr unsigned MAX_OUTPUTS = MAX_INPUTS;

public:
    /// Constructor
    Not64Gate( JsonVariant&                       componentObject,
               Cpl::Memory::ContiguousAllocator&  generalAllocator,
               Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
               Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
               Fxt::Point::DatabaseApi&           dbForPoints );

    /// Destructor
    ~Not64Gate();

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
    bool parseConfiguration( Cpl::Memory::ContiguousAllocator& generalAllocator,
                             JsonVariant&                      obj,
                             unsigned                          minInputs,
                             unsigned                          maxInputs,
                             unsigned                          minOutputs,
                             unsigned                          maxOutputs ) noexcept;

protected:
    /// List of Negate qualifier for the output points
    bool*  m_outputPassthrough;
};



};      // end namespaces
};
};
#endif  // end header latch