#ifndef Fxt_Card_HW_Mock_Digital8Factory_h_
#define Fxt_Card_HW_Mock_Digital8Factory_h_
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
#include "Fxt/Card/HW/Mock/Digital8.h"

///
namespace Fxt {
///
namespace Card {
///
namespace HW {
///
namespace Mock {


/** This concrete class implements the Factory API to create a Mock Digital8
    card.

    \code

    JSON Definition
    --------------------
    {
      "name": "My Digital Card",                        // Text label for the card
      "id": 0,                                          // ID assigned to the card
      "type": "59d33888-62c7-45b2-a4d4-9dbc55914ed3",   // Identifies the card type.  Value comes from the Supported/Available-card-list
      "typename": "Fxt::Card::HW::Mock::Digital8",      // Human readable type name
      "slot": 0,                                        // Physical identifier, e.g. its the card position in the Node's physical chassis
      "points": {
        "inputs": [                                     // Inputs. The card supports 8 input points that is exposed a single Byte
          {
            "channel": 1                                // Always set to 1
            "id": 0,                                    // ID assigned to the Virtual Point that represents the input value
            "ioRegId": 0,                               // The ID of the Point's IO register.
            "name": "My input name"                     // Text label for the 8 input signals
            "initial": {                                // OPTIONAL.  If not used, the IO Register point defaults to the invalid state.
              "valid": true|false,                      // Initial valid state for the IO Register point
              "val":  <num>                             // Initial value for the input point. Only required when 'valid' is true
              "id": 0                                   // The ID of the internal point that is used store the initial value in binary form
            }
          }
        ],
        "outputs": [                                    // Outputs. The card supports 0 to 32 output points
          {
            "channel": 1                                // Always set to 1
            "id": 0,                                    // ID assigned to the Virtual Point that represents the output value
            "ioRegId": 0,                               // The ID of the Point's IO register.
            "name": "My output name"                    // Text label for the 8 output signals
            "initial": {                                // OPTIONAL.  If not used, the IO Register point defaults to the invalid state.
              "valid": true|false,                      // Initial valid state for the IO Register point
              "val":  <num>                             // Initial value for the input point. Only required when 'valid' is true
              "id": 0                                   // The ID of the internal point that is used store the initial value in binary form
            }
          }
        ]
      }
    }

    \endcode

 */
class Digital8Factory : public Fxt::Card::FactoryCommon_
{
public:
    /// Constructor
    Digital8Factory( Fxt::Card::DatabaseApi&             cardDb,
                     Cpl::Memory::ContiguousAllocator&   generalAllocator,
                     Cpl::Memory::ContiguousAllocator&   statefulDataAllocator,
                     Fxt::Point::DatabaseApi&            dbForPoints );

    /// Destructor
    ~Digital8Factory();

public:
    /// See Fxt::Card::FactoryApi
    bool create( JsonVariant& cardObject ) noexcept;

    /// See Fxt::Card::FactoryApi
    const char* getGuid() const noexcept { return Digital8::GUID_STRING; }
};


};      // end namespaces
};
};
};
#endif  // end header latch
