#ifndef Fxt_Component_Digital_Mux_h_
#define Fxt_Component_Digital_Mux_h_
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
#include "Fxt/Point/Api.h"
#include "Fxt/Point/Bool.h"
#include "Fxt/Point/BankApi.h"


///
namespace Fxt {
///
namespace Component {
///
namespace Digital {


/** This concrete class implements a Component that combines up to 64 boolean
    inputs into a single integer, where the type of integer (byte, word, dword,
    qword) is specified. Only 1 input signal is required.

    Bit ordering: Bit0 is the LEAST significant bit

    IF any input signal is invalid, THEN the output signal is invalid.

    The component has NO stateful data

    \code

    JSON Definition
    --------------------
    {
       "name": "Mux #1"                                     // *Text label for the component
       "type": "d60f2daf-9709-42d6-ba92-b76f641eb930",      // Identifies the card type.  Value comes from the Supported/Available-card-list
       "typeName": "Fxt::Component::Digital::Mux"           // *OPTIONAL: Human readable type name
       "inputs": [                                          // Array of Point references that supply the Component's input values.  Number of elements: 1-16
          {
            "name": "input bit0",                           // human readable name for the input value
            "bit: 0                                         // The bit offset (zero based, bit=LSb) in the mux'd output byte
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // *REQUIRED Type for the input signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the input signal
            "idRef": 4294967295                             // Point ID Reference to the point to read the input value from
            "negate": false                                 // Optional: (defaults to false) When set to true - the input signal is inverted/negated before being muxed
          },
          ...
       ],
       "outputs": [                                         // Array of Point reference where the Component's writes it output values to.  Number of elements: MUST be 1
          {
            "name":"output"                                 // Human readable name for the output signal
            "type": "918cff9e-8007-4666-99ac-384b9624329c", // *REQUIRED Type for the output signal.  Can be any integer type: Int8, Uint8, ... Int64, Uint64.
            "typeName": "Fxt::Point::Uint8",                // *OPTIONAL: Human readable Type name for the output signal
            "idRef":4294967295,                             // Point ID reference to the point that is updated with the output value
          }
       ]
    }

    *The field is NOT parsed/used by the firmware

    \endcode
 */
class Mux : public Fxt::Component::Common_
{
public:
    /// Type ID for the card
    static constexpr const char*    GUID_STRING = "d60f2daf-9709-42d6-ba92-b76f641eb930";

    /// Type name for the card
    static constexpr const char*    TYPE_NAME   = "Fxt::Component::Digital::Mux";

    /// Size (in bytes) of Stateful data that will be allocated on the HA Heap
    static constexpr const size_t   HA_STATEFUL_HEAP_SIZE = 0;

public:
    /// Maximum number of Input signals
    static constexpr unsigned MAX_INPUTS = 64;

    /// Maximum number of Output signals
    static constexpr unsigned MAX_OUTPUTS = 1;

public:
    /// Constructor
    Mux( JsonVariant&                       componentObject,
         Cpl::Memory::ContiguousAllocator&  generalAllocator,
         Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
         Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
         Fxt::Point::DatabaseApi&           dbForPoints );

    /// Destructor
    ~Mux();

public:
    /// See Fxt::Component::Api
    Fxt::Type::Error resolveReferences( Fxt::Point::DatabaseApi& pointDb ) noexcept;

    /// See Fxt::Component::Api
    Fxt::Type::Error execute( int64_t currentTickUsec ) noexcept;

public:
    /// See Fxt::Component::Api
    const char* getTypeGuid() const noexcept;

    /// See Fxt::Component::Api
    const char* getTypeName() const noexcept;

protected:
    /// Helper method to parse the card's JSON config
    bool parseConfiguration( Cpl::Memory::ContiguousAllocator& generalAllocator, JsonVariant& obj ) noexcept;

protected:
    /// Function signature for updating the output value
    typedef void (*WriteFunc_T)(Fxt::Point::Api* genericPt, uint64_t value);

protected:
    /// List of Input Points.  Note: Initially the point IDs are stored instead of pointers
    Fxt::Point::Bool**  m_inputRefs;

    /// List of Output Points. Note: Initially the point IDs are stored instead of pointers
    Fxt::Point::Api**   m_outputRefs;

    /// List of Negate qualifier for the input points
    bool*               m_inputNegated;

    /// List of Bit offset for the input points
    uint8_t*            m_bitOffsets;

    /// Type specific function that performs the update to the output value
    WriteFunc_T         m_writeFunc;

    /// Number of Input points
    uint8_t             m_numInputs;

    /// Number of Output points
    uint8_t             m_numOutputs;
};



};      // end namespaces
};
};
#endif  // end header latch
