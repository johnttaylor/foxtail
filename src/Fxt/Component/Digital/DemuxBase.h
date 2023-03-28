#ifndef Fxt_Component_Digital_DemuxBase_h_
#define Fxt_Component_Digital_DemuxBase_h_
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


/** This partially concrete class implements a Component that breaks-out the
    each individual bit in an integer into 2 Boolean outputs (1 pt for
    the logical value, an 1 pt for its complement).

    The concrete child class determines the maximum number of output points

    Bit ordering: Bit0 is the LEAST significant bit

    IF the input signal is invalid, ALL output signals are invalid.

    The component has NO stateful data

    \code

    JSON Definition
    --------------------
    {
       "name": "Byte DemuxBase #1"                              // *Text label for the component
       "type": "8c55aa52-3bc8-4b8a-ad73-c434a0bbd4b4",      // Identifies the card type.  Value comes from the Supported/Available-card-list
       "typeName": "Fxt::Component::Digital::Demux8u"       // *OPTIONAL: Human readable type name
       "inputs": [                                          // Array of Point references that supply the Component's input values.
          {
            "name": "input word",                           // human readable name for the input value
            "type": "918cff9e-8007-4666-99ac-384b9624329c", // *REQUIRED Type for the input signal. The type is CHILD class specific!
            "typeName": "Fxt::Point::Uint8",                // *OPTIONAL: Human readable Type name for the input signal. The type is CHILD class specific!
            "idRef": 4294967295                             // Point ID Reference to the point to read the input value from
          },
          ...
       ],
       "outputs": [                                         // Array of Point reference where the Component's writes it output values to.  Number of output is CHILD class specific!
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
class DemuxBase : public Fxt::Component::Common_
{
public:
    /// Size (in bytes) of Stateful data that will be allocated on the HA Heap
    static constexpr const size_t   HA_STATEFUL_HEAP_SIZE = 0;

public:
    /// Maximum number of Input signals
    static constexpr unsigned MAX_INPUTS = 1;

protected:
    /// Constructor
    DemuxBase();

    /// Destructor
    ~DemuxBase();

public:
    /// See Fxt::Component::Api
    Fxt::Type::Error resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept;

protected:
    /// Helper method to parse the card's JSON config
    bool parseConfiguration( Cpl::Memory::ContiguousAllocator& generalAllocator,
                             JsonVariant&                      obj,
                             unsigned                          minInputs,
                             unsigned                          maxInputs,
                             unsigned                          minOutputs,
                             unsigned                          maxOutputs ) noexcept;

    /// Helper method that returns the Input point type's GUID
    virtual const char* getInputPointTypeGuid() const noexcept = 0;

    /// Helper method that invalidates ALL outputs
    void invalidateAllOutputs() noexcept;

protected:
    /// Helper method that is type specific for the execute() calls
    template<typename PT_TYPE, typename WORD_TYPE>
    Fxt::Type::Error doExecute( int64_t currentTickUsec ) noexcept
    {
        // NOTE: The method NEVER fails

        // Get my input
        WORD_TYPE inValue;
        PT_TYPE* pt = (PT_TYPE*) m_inputRefs[0];
        if ( !pt->read( inValue ) )
        {
            // Set the outputs to invalid if at least one input is invalid
            invalidateAllOutputs();
            return Fxt::Type::Error::SUCCESS();
        }

        // Derive my outputs
        for ( unsigned i=0; i < m_numOutputs; i++ )
        {
            WORD_TYPE bitMask   = ((WORD_TYPE) 1) << m_bitOffsets[i];
            bool      outputVal = bitMask & inValue;
            bool      finalOut  = m_outputNegated[i] ? !outputVal : outputVal;
            Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_outputRefs[i];
            pt->write( finalOut );
        }

        return Fxt::Type::Error::SUCCESS();
    }

protected:
    /// List of Negate qualifier for the output points
    bool*               m_outputNegated;

    /// List of Bit offset for the output points
    uint8_t*            m_bitOffsets;
};



};      // end namespaces
};
};
#endif  // end header latch
