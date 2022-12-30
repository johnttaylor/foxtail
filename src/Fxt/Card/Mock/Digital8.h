#ifndef Fxt_Card_Mock_Digital8_h_
#define Fxt_Card_Mock_Digital8_h_
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


#include "Fxt/Point/Uint8.h"
#include "Fxt/Card/Common_.h"
#include "Cpl/Json/Arduino.h"
#include "Cpl/System/Mutex.h"

///
namespace Fxt {
///
namespace Card {
///
namespace Mock {


/** This concrete class implements a mocked/emulated IO card that
    supports 8 digital inputs, and 8 digital outputs.

    In general the implementation is NOT thread safe, however the get/set methods
    ARE thread safe to allow the application/test/console to drive/access the
    inputs/outputs.

    The class dynamically creates numerous thingys and objects when it is created.
    This memory is allocated via a 'Contiguous Allocator'.  This means the this
    class will NOT free the allocated memory in its destructor, it WILL however
    call the destructor on any objects it directly created using the allocator.
    The semantics with the Application is that the Application is RESPONSIBLE for
    freeing/recycling the memory in the Contiguous Allocator when Card(s) are
    deleted.

    \code

    JSON Definition
    --------------------
    {
      "name": "My Digital Card",                            // Text label for the card
      "id": 0,                                              // ID assigned to the card
      "type": "59d33888-62c7-45b2-a4d4-9dbc55914ed3",       // Identifies the card type.  Value comes from the Supported/Available-card-list
      "typename": "Fxt::Card::Mock::Digital8",              // *Human readable type name
      "slot": 0,                                            // *Physical identifier, e.g. its the card position in the Node's physical chassis
      "points": {
        "inputs": [                                         // Inputs. The card supports 8 input points that is exposed a single Byte
          {
            "channel": 1                                    // Always set to 1
            "id": 0,                                        // ID assigned to the Virtual Point that represents the input value
            "ioRegId": 0,                                   // The ID of the Point's IO register.
            "name": "My input name"                         // Text label for the 8 input signals
            "type": "918cff9e-8007-4666-99ac-384b9624329c", // *REQUIRED Type for the input signal
            "typeName": "Fxt::Point::Uint8",                // *OPTIONAL: Human readable Type name for the input signal
            "initial": {
              "valid": true|false                           // Initial valid state for the IO Register point
              "val": <integer>                              // Initial value for the input point. Only required when 'valid' is true
              "id": 0                                       // The ID of the internal point that is used store the initial value in binary form
            }
          }
        ],
        "outputs": [                                        // Outputs. The card supports 8 output points that is exposed a single Byte
          {
            "channel": 1                                    // Always set to 1
            "id": 0,                                        // ID assigned to the Virtual Point that represents the output value
            "ioRegId": 0,                                   // The ID of the Point's IO register.
            "name": "My output name"                        // Text label for the 8 output signals
            "type": "918cff9e-8007-4666-99ac-384b9624329c", // *REQUIRED Type for the output signal
            "typeName": "Fxt::Point::Uint8",                // *OPTIONAL: Human readable Type name for the output signal
            "initial": {
              "valid": true|false                           // Initial valid state for the IO Register point
              "val": <integer>                              // Initial value for the input point. Only required when 'valid' is true
              "id": 0                                       // The ID of the internal point that is used store the initial value in binary form
            }
          }
        ]
      }
    }

    *The field is NOT parsed/used by the firmware

    \endcode
 */
class Digital8 : public Fxt::Card::Common_
{
public:
    /// Type ID for the card
    static constexpr const char*    GUID_STRING = "59d33888-62c7-45b2-a4d4-9dbc55914ed3";

    /// Type name for the card
    static constexpr const char*    TYPE_NAME   = "Fxt::Card::Mock::Digital8";

    /// Size (in bytes) of Stateful data that will be allocated on the Card Heap
    static constexpr const size_t   CARD_STATEFUL_HEAP_SIZE = (1 * 3 * sizeof( Fxt::Point::Uint8::StateBlock_T ));


public:
    /// Constructor
    Digital8( Cpl::Memory::ContiguousAllocator&  generalAllocator,
              Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
              Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
              Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
              Fxt::Point::DatabaseApi&           dbForPoints,
              JsonVariant&                       cardObject );

public:
    /// See Fxt::Card::Api
    bool start() noexcept;

    /// See Fxt::Card::Api
    bool stop() noexcept;

    /// See Fxt::Card::Api
    const char* getTypeGuid() const noexcept;

    /// See Fxt::Card::Api
    const char* getTypeName() const noexcept;

public:
    /// Provide the Application the ability to set the inputs. This method is thread safe
    void setInputs( uint8_t bitMaskToOR );

    /// Provide the Application the ability to clear the inputs. This method is thread safe
    void clearInputs( uint8_t bitMaskToAND );

    /// Provide the Application the ability to toggle the inputs. This method is thread safe
    void toggleInputs( uint8_t bitMaskToXOR );

    /// Provide the Application the ability to read the inputs. Returns false if one or more of the outputs is INVALID; else true is returned
    bool getOutputs( uint8_t& valueMask );

    /// Set input by bit position (zero based bit position)
    inline void setInputBit( uint8_t bitPosition ) { setInputs( ((uint8_t) 1) << bitPosition ); }

    /// Clear input by bit position (zero based bit position)
    inline void clearInputBit( uint8_t bitPosition ) { clearInputs( ~(((uint8_t) 1) << bitPosition) ); }

    /// Toggle input by bit position (zero based bit position)
    inline void toggleInputBit( uint8_t bitPosition ) { toggleInputs( ((uint8_t) 1) << bitPosition ); }


protected:
    /// Helper method to parse the card's JSON config
    void parseConfiguration( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                             Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                             Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                             Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                             Fxt::Point::DatabaseApi&           dbForPoints,
                             JsonVariant&                       cardObject ) noexcept;


protected:
    /// Mutex to provide thread safety for the application driving/reading the mocked IO
    Cpl::System::Mutex                  m_lock;

};



};      // end namespaces
};
};
#endif  // end header latch
