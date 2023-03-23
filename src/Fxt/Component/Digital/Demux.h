#ifndef Fxt_Component_Digital_Demux_h_
#define Fxt_Component_Digital_Demux_h_
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
#include "Fxt/Point/Uint8.h"
#include "Fxt/Point/Bool.h"
#include "Fxt/Point/BankApi.h"


///
namespace Fxt {
///
namespace Component {
///
namespace Digital {


/** This concrete class implements a Component that breaks-out the each
    individual bit in an integer into 2 Boolean outputs (1 pt for
    the logical value, an 1 pt for its complement).  There are total of
    possible boolean outputs is dependent on the input type, e.g. Uint8
    input has 8 outputs, an Int64 has 64 outputs.

    Bit ordering: Bit0 is the LEAST significant bit

    IF the input signal is invalid, ALL output signals are invalid.

    The component has NO stateful data

    \code

    JSON Definition
    --------------------
    {
       "name": "Byte Demux #1"                           // *Text label for the component
       "type": "8c55aa52-3bc8-4b8a-ad73-c434a0bbd4b4",      // Identifies the card type.  Value comes from the Supported/Available-card-list
       "typeName": "Fxt::Component::Digital::Demux"  // *OPTIONAL: Human readable type name
       "inputs": [                                          // Array of Point references that supply the Component's input values.  Number of elements: 1-16
          {
            "name": "input word",                           // human readable name for the input value
            "type": "918cff9e-8007-4666-99ac-384b9624329c", // *REQUIRED Type for the input signal. Can be any integer type: Int8, Uint8, ... Int64, Uint64.
            "typeName": "Fxt::Point::Uint8",                // *OPTIONAL: Human readable Type name for the input signal
            "idRef": 4294967295                             // Point ID Reference to the point to read the input value from
          },
          ...
       ],
       "outputs": [                                         // Array of Point reference where the Component's writes it output values to.  Number of elements: 1-2
          {
            "bit: 0                                         // The bit offset (zero based, bit=LSb)
            "name":"Bit0"                                   // Human readable name for the output signal
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // *REQUIRED Type for the output signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the output signal
            "idRef":4294967295,                             // Point ID reference to the point that is updated with the output value
            "negate":false                                  // Optional: (defaults to false) When set to false - the output is result of ANDing the input signals.  When set to true the output is logical complement of ANDing result
          },
          {
            "bit: 0                                         // The bit offset (zero based, bit=LSb)
            "name":"/Bit0"                                  // Human readable name for the output signal
            "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0", // *REQUIRED Type for the output signal
            "typeName": "Fxt::Point::Bool",                 // *OPTIONAL: Human readable Type name for the output signal
            "idRef":4294967295,                             // Point ID reference to the point that is updated with the output value
            "negate":true                                   // Optional: (defaults to false) When set to false - the output is result of ANDing the input signals.  When set to true the output is logical complement of ANDing result
          },
          ...
       ]
    }

    *The field is NOT parsed/used by the firmware

    \endcode
 */
class Demux : public Fxt::Component::Common_
{
public:
    /// Type ID for the card
    static constexpr const char*    GUID_STRING = "8c55aa52-3bc8-4b8a-ad73-c434a0bbd4b4";

    /// Type name for the card
    static constexpr const char*    TYPE_NAME   = "Fxt::Component::Digital::Demux";

    /// Size (in bytes) of Stateful data that will be allocated on the HA Heap
    static constexpr const size_t   HA_STATEFUL_HEAP_SIZE = 0;

public:
    /// Maximum number of Input signals
    static constexpr unsigned MAX_INPUTS = 1;

    /// Maximum number of Output signals
    static constexpr unsigned MAX_OUTPUTS = 64;

public:
    /// Constructor
    Demux( JsonVariant&                       componentObject,
           Cpl::Memory::ContiguousAllocator&  generalAllocator,
           Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
           Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
           Fxt::Point::DatabaseApi&           dbForPoints );

    /// Destructor
    ~Demux();

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
    bool parseConfiguration( Cpl::Memory::ContiguousAllocator& generalAllocator, JsonVariant& obj ) noexcept;

protected:
    /// Function signature for reading the input value
    typedef bool (*ReadFunc_T)(Fxt::Point::Api* genericPt, uint64_t& dstValue );

protected:
    /// List of Input Points.  Note: Initially the point IDs are stored instead of pointers
    Fxt::Point::Api**   m_inputRefs;

    /// List of Output Points. Note: Initially the point IDs are stored instead of pointers
    Fxt::Point::Bool**  m_outputRefs;

    /// List of Negate qualifier for the output points
    bool*               m_outputNegated;

    /// List of Bit offset for the output points
    uint8_t*            m_bitOffsets;

    /// Type specific function to read the input value
    ReadFunc_T          m_readFunc;

    /// Number of Input points
    uint8_t             m_numInputs;

    /// Number of Output points
    uint8_t             m_numOutputs;
};



};      // end namespaces
};
};
#endif  // end header latch
