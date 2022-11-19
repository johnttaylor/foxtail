#ifndef Fxt_Card_Mock_AnalogIn8_h_
#define Fxt_Card_Mock_AnalogIn8_h_
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
    supports 8 Analog inputs.  The analog values are of type 'float' and
    have units of 'Volts'

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
      "name": "My AnalogIn8 Card",                      // Text label for the card
      "id": 0,                                          // ID assigned to the card
      "type": "1968f533-e323-4ae4-8493-9a572f3bd195",   // Identifies the card type.  Value comes from the Supported/Available-card-list
      "typename": "Fxt::Card::Mock::AnalogIn8",         // Human readable type name
      "slot": 0,                                        // Physical identifier, e.g. its the card position in the Node's physical chassis
      "points": {
        "inputs": [                                     // Inputs. The card supports 8 input values. Each input is exposed a Float point
          {
            "channel": 1                                // Range: 1 to 8.  (For a physical card this would map back to connector/terminal identifiers)
            "id": 0,                                    // ID assigned to the Virtual Point that represents the input value
            "ioRegId": 0,                               // The ID of the Point's IO register.
            "internalId": 0,                            // The ID of the Point's internal register.
            "name": "My input#1 name"                   // Text label for the input signal
            "initial": {
              "valid": true|false                       // Initial valid state for the IO Register point
              "val": <float>                            // Initial value for the input point. Only required when 'valid' is true
              "id": 0                                   // The ID of the internal point that is used store the initial value in binary form
            }
          }
        ]
      }
    }

    \endcode
 */
class AnalogIn8 : public Fxt::Card::Common_
{
public:
    /// Type ID for the card
    static constexpr const char* GUID_STRING = "1968f533-e323-4ae4-8493-9a572f3bd195";

    /// Type name for the card
    static constexpr const char* TYPE_NAME   = "Fxt::Card::Mock::AnalogIn8";

public:

public:
    /// Maximum number of descriptors per Input and Output
    static constexpr unsigned MAX_DESCRIPTORS = 8;


public:
    /// Constructor
    AnalogIn8( DatabaseApi&                       cardDb,
               Cpl::Memory::ContiguousAllocator&  generalAllocator,
               Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
               Fxt::Point::DatabaseApi&           dbForPoints,
               uint16_t                           cardId,
               JsonVariant&                       cardObject );

    /// Destructor
    ~AnalogIn8();

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
    /** Provide the Application the ability to set the inputs. The range of
        'channelNumber' is 1...8 (as defined in the Card's JSON object). This 
        method is thread safe
     */
    void setInputs( uint8_t channelNumber, float newValue );


protected:
    /// Helper method to parse the card's JSON config
    virtual bool parseConfiguration( JsonVariant& obj ) noexcept;

    /// Helper method to create the point instances
    virtual void createPoints() noexcept;


protected:
    /// Mutex to provide thread safety for the application driving/reading the mocked IO
    Cpl::System::Mutex                  m_lock;

    /// List of Virtual Point Input Descriptors (allocate space for max IO plus a list-terminator)
    Fxt::Point::Descriptor*             m_virtualInDescriptors[MAX_DESCRIPTORS + 1];

    /// List of IO Register Input Descriptors (allocate space for max IO plus a list-terminator)
    Fxt::Point::Descriptor*             m_ioRegInDescriptors[MAX_DESCRIPTORS + 1];

    /// Channel Numbers
    uint16_t                            m_channelNumbers[MAX_DESCRIPTORS];
};



};      // end namespaces
};
};
#endif  // end header latch
