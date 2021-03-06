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
 */
class DigitalFactory : public Fxt::Card::FactoryCommon_
{
public:
    /// Constructor
    DigitalFactory( Fxt::Card::DatabaseApi&                             cardDb,
                    Cpl::Container::Dictionary<Fxt::Point::Descriptor>& descriptorDatabase,
                    Fxt::Point::Database&                               pointDatabase,
                    PointAllocators_T&                                  pointAllocators,
                    Cpl::Memory::ContiguousAllocator&                   allocatorForCard );

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
