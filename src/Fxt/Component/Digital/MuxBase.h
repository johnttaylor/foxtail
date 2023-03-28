#ifndef Fxt_Component_Digital_MuxBase_h_
#define Fxt_Component_Digital_MuxBase_h_
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


/** This partially concrete class implements a Component that combines up to N 
    boolean inputs into a single integer.  The are two possible outputs for
    each input bit. 
    
    The integer type (i.e. the number of bits) is defined by concrete child class. 

    Bit ordering: Bit0 is the LEAST significant bit

    IF any input signal is invalid, THEN the output signal is invalid.

    The component has NO stateful data

    \code

    JSON Definition
    --------------------
    {
       "name": "Mux #1"                                     // *Text label for the component
       "type": "d60f2daf-9709-42d6-ba92-b76f641eb930",      // Identifies the card type.  Value comes from the Supported/Available-card-list
       "typeName": "Fxt::Component::Digital::Mux8u"         // *OPTIONAL: Human readable type name
       "inputs": [                                          // Array of Point references that supply the Component's input values.  Number of elements is CHILD class specific!
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
            "type": "918cff9e-8007-4666-99ac-384b9624329c", // *REQUIRED Type for the output signal.  The type is CHILD class specific!
            "typeName": "Fxt::Point::Uint8",                // *OPTIONAL: Human readable Type name for the output signal. The type is CHILD class specific!
            "idRef":4294967295,                             // Point ID reference to the point that is updated with the output value
          }
       ]
    }

    *The field is NOT parsed/used by the firmware

    \endcode
 */
class MuxBase : public Fxt::Component::Common_
{
public:
    /// Size (in bytes) of Stateful data that will be allocated on the HA Heap
    static constexpr const size_t   HA_STATEFUL_HEAP_SIZE = 0;

public:
    /// Maximum number of Output signals
    static constexpr unsigned MAX_OUTPUTS = 1;

protected:
    /// Constructor
    MuxBase();

    /// Destructor
    ~MuxBase();

public:
    /// See Fxt::Component::Api
    Fxt::Type::Error resolveReferences( Fxt::Point::DatabaseApi& pointDb ) noexcept;


protected:
    /// Helper method to parse the card'sJSON config
    bool parseConfiguration( Cpl::Memory::ContiguousAllocator& generalAllocator,
                             JsonVariant&                      obj,
                             unsigned                          minInputs,
                             unsigned                          maxInputs,
                             unsigned                          minOutputs,
                             unsigned                          maxOutputs ) noexcept;

    /// Returns (via 'dstBitValue' the specified input's bit value.  Return false if the input bit is invalid
    bool getInputBit( unsigned refIdx, bool& dstBitValue );

    /// Helper method that returns the Output point type's GUID
    virtual const char* getOutputPointTypeGuid() const noexcept = 0;

protected:
    /// Helper method that is type specific for the execute() calls
    template<typename PT_TYPE, typename WORD_TYPE>
    Fxt::Type::Error doExecute( int64_t currentTickUsec ) noexcept
    {
        // NOTE: The method NEVER fails

        // Walk all of my inputs
        WORD_TYPE outValue = 0;
        for ( unsigned i=0; i < m_numInputs; i++ )
        {
            bool bitValue;
            if ( !getInputBit( i, bitValue ) )
            {
                // Input invalid - can exit early
                return Fxt::Type::Error::SUCCESS();
            }

            WORD_TYPE bitMask = ((WORD_TYPE) bitValue) << m_bitOffsets[i];
            outValue         |= bitMask;
        }

        // Update the output value
        PT_TYPE* pt = (PT_TYPE*) m_outputRefs[0];
        pt->write( outValue);
        return Fxt::Type::Error::SUCCESS();
    }

protected:
    /// List of Negate qualifier for the input points
    bool*                                   m_inputNegated;

    /// List of Bit offset for the input points
    uint8_t*                                m_bitOffsets;
};



};      // end namespaces
};
};
#endif  // end header latch
