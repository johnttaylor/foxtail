#ifndef Fxt_Card_HW_Mock_DigitalFactory_h_
#define Fxt_Card_HW_Mock_DigitalFactory_h_
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

#include "Fxt/Card/FactoryCommon_.h"
#include "Fxt/Card/HW/Mock/Digital.h"

///
namespace Fxt {
///
namespace Card {
///
namespace HW {
///
namespace Mock {


/** This concrete class implements the Factory API to create a Mock Digital
    card.

    \code

    JSON Definition
    --------------------
    {
      "name": "My Digital Card",                        // Text label for the card
      "id": 0,                                          // ID assigned to the card
      "type": "59d33888-62c7-45b2-a4d4-9dbc55914ed3",   // Identifies the card type.  Value comes from the Supported/Available-card-list
      "typename": "Fxt::Card::HW::Mock::Digital",       // Human readable type name
      "slot": 0,                                        // Physical identifier, e.g. its the card position in the Node's physical chassis
      "scannerIdRef: 0                                  // ID Reference of the scanner instance that scans this card
      "initialInputValueMask":  0,                      // unsigned 32bit long mask used to set the initial input values.  LSb == channel 0. 0=Signal Low, 1=Signal High
      "points": {
        "inputs": [                                     // Inputs. The card supports 0 to 32 input points
          {
            "channel": 0,                               // Physical identifier, e.g. terminal/wiring connection number on the card
            "id": 0,                                    // ID assigned to the Virtual Point that represents the input value
            "ioRegId": 0,                               // The ID of the Point's IO register.
            "internalId": 0,                            // The ID of the Point's internal register.
            "name": "My input point0 name"              // Text label for the input signal
          },
          ...
        ],
        "outputs": [                                    // Outputs. The card supports 0 to 32 output points
          {
            "channel": 0,                               // Physical identifier, e.g. terminal/wiring connection number on the card
            "id": 0,                                    // ID assigned to the Virtual Point that represents the output value
            "ioRegId": 0,                               // The ID of the Point's IO register.
            "internalId": 0,                            // The ID of the Point's internal register.
            "name": "My output point0 name"             // Text label for the output signal
          },
          ...
        ]
      }
    }

    \endcode

 */
class DigitalFactory : public Fxt::Card::FactoryCommon_
{
public:
    /// Constructor
    DigitalFactory( Fxt::Card::DatabaseApi&            cardDb,
                    Fxt::Point::DatabaseApi&           pointDatabase,
                    PointAllocators_T&                 pointAllocators,
                    Cpl::Memory::ContiguousAllocator&  allocatorForCard );

    /// Destructor
    ~DigitalFactory();

public:
    /// See Fxt::Card::FactoryApi
    bool create( JsonVariant& cardObject ) noexcept;

    /// See Fxt::Card::FactoryApi
    const char* getGuid() const noexcept { return Digital::GUID_STRING; }
};


};      // end namespaces
};
};
};
#endif  // end header latch
