#ifndef Fxt_Card_HW_Mock_Analog8Factory_h_
#define Fxt_Card_HW_Mock_Analog8Factory_h_
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
#include "Fxt/Card/HW/Mock/AnalogIn8.h"

///
namespace Fxt {
///
namespace Card {
///
namespace HW {
///
namespace Mock {


/** This concrete class implements the Factory API to create a Mock Analog8
    card.


 */
class AnalogIn8Factory : public Fxt::Card::FactoryCommon_
{
public:
    /// Constructor
    AnalogIn8Factory( FactoryDatabaseApi&                 factoryDatabase,
                      Cpl::Memory::ContiguousAllocator&   generalAllocator,
                      Cpl::Memory::ContiguousAllocator&   statefulDataAllocator,
                      Fxt::Point::DatabaseApi&            dbForPoints );

    /// Destructor
    ~AnalogIn8Factory();

public:
    /// See Fxt::Card::FactoryApi
    Api* create( JsonVariant& cardObject, uint32_t& cardErrorCode ) noexcept;

    /// See Fxt::Card::FactoryApi
    const char* getGuid() const noexcept { return AnalogIn8::GUID_STRING; }
};


};      // end namespaces
};
};
};
#endif  // end header latch
