#ifndef Fxt_Card_Hw_Mock_Digital_h_
#define Fxt_Card_Hw_Mock_Digital_h_
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


#include "Fxt/Card/Api.h"
#include "Cpl/Json/Arduino.h"
#include "Fxt/Point/RegisterBank.h"

///
namespace Fxt {
///
namespace Card {
///
namespace HW {
///
namespace Mock {


/** This concrete class implements a mocked/emulated IO card that
    supports N digital inputs, and M digital outputs.

    In general the implementation is NOT thread safe, however the get/set methods
    ARE thread safe to allow the application/test/console to drive/access the
    inputs/outputs.

    \code

    Static Configuration
    --------------------
    "staticConfig": {
        "numInputs": "n",                   // Number of inputs.   Range 0 to 32
        "numOutputs": "m",                  // Number of outputs.  Range 0 to 32
        "initialOutputVals": [ 0, 1, ...],  // Each output signal must have value. 0=signal low, 1=signal high.  
        "initialInputVals": [ 1, 1, ...]    // Each input signal must have value.  0=signal low, 1=signal high. 
    }

    Runtime Configuration
    --------------------
    "runtimeConfig": {
        "descriptors": {    // Minimum required info
            "inputs": [ // One entry per input. Signals are the same order as the staticConfig order
                {
                    "id": 0,
                    "name": "symbolicName1"
                },
                {
                    "id": 1,
                    "name": "symbolicName2"
                }
            ],
            "outputs": [ // One entry per output. Signals are the same order as the staticConfig order
                {
                    "id": 2,
                    "name": "symbolicName3"
                },
                {
                    "id": 3,
                    "name": "symbolicName4"
                }
            ]
        }
    }

    \endcode
 */
class Digital : public Fxt::Card::Api
{
public:
    /// Type ID for the card
    static constexpr const char* guidString = "59d33888-62c7-45b2-a4d4-9dbc55914ed3";

public:
    /// Constructor
    Digital( const char*                cardName,
             uint16_t                   cardLocalId,
             JsonVariant&               staticConfig,
             JsonVariant&               runtimeConfig,
             Fxt::Point::BankApi&       internalInputsBank,
             Fxt::Point::RegisterBank&  registerInputsBank,
             Fxt::Point::BankApi&       internalOutputsBank,
             Fxt::Point::RegisterBank&  registerOutputsBank,
             uint32_t&                  startEndPointIdValue );

    /// Destructor
    ~Digital();

public:
    /// See Fxt::Card::Api
    bool start() noexcept;

    /// See Fxt::Card::Api
    bool stop() noexcept;

    /// See Fxt::Card::Api
    bool isStarted() const noexcept;

    /// See Fxt::Card::Api
    uint16_t getLocalId() const noexcept;

    /// See Fxt::Card::Api
    const char* getName() const noexcept;

    /// See Fxt::Card::Api
    const Cpl::Type::Guid_T& getGuid() const noexcept;

    /// See Fxt::Card::Api
    const char* getTypeName() const noexcept;

public:
    /// Provide the Application the ability to set the inputs. This method is thread safe
    void setInputs( uint32_t bitMask );

    /// Provide the Application the ability to clear the inputs. This method is thread safe
    void clearInputs( uint32_t bitMask );

    /// Provide the Application the ability to toggle the inputs. This method is thread safe
    void toggleInputs( uint32_t bitMask );

    /// Provide the Application the ability to read the inputs. 
    uint32_t getOutputs();

    /// Set input by bit position (zero based bit position)
    inline void setInputBit( uint8_t bitPosition ) { setInputs( ((uint32_t) 1) << bitPosition ); }

    /// Clear input by bit position (zero based bit position)
    inline void clearInputBit( uint8_t bitPosition ) { clearInputs( ~(((uint32_t) 1) << bitPosition ) ); }

    /// Toggle input by bit position (zero based bit position)
    inline void toggleInputBit( uint8_t bitPosition ) { toggleInputs( ((uint32_t) 1) << bitPosition ); }


protected:
    /// The card's Type/GUID (is the same for all instances)
    static Cpl::Type::Guid_T g_guid;

    /// The card's runtime name
    const char* m_cardName;

    /// The card's 'User facing local ID'
    uint16_t    m_localId;

    /// Number of Inputs
    uint8_t     m_numInputs;

    /// Number of Outputs
    uint8_t     m_numInputs;

    /// Initial (when start() is called) input values
    uint32_t    m_initInputVals;

    /// Initial (when start() is called) output values
    uint32_t    m_initOutputVals;

    /// Internal Input Descriptors (allocate space for max IO)
    Fxt::Point::Descriptor m_internalInDescriptors[32];

    /// Visible/Register Input Descriptors (allocate space for max IO)
    Fxt::Point::Descriptor m_registerInDescriptors[32];

    /// Internal Output Descriptors (allocate space for max IO)
    Fxt::Point::Descriptor m_internalOutDescriptors[32];

    /// Visible/Register Output Descriptors (allocate space for max IO)
    Fxt::Point::Descriptor m_registerOutDescriptors[32];
};



};      // end namespaces
};
};
};
#endif  // end header latch
