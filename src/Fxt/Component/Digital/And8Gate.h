#ifndef Fxt_Component_Digital_And8Gate_h_
#define Fxt_Component_Digital_And8Gate_h_
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


#include "Fxt/Component/Digital/AndGateBase.h"
#include "Cpl/Json/Arduino.h"
#include "Fxt/Point/Bool.h"
#include "Fxt/Point/BankApi.h"

///
namespace Fxt {
///
namespace Component {
///
namespace Digital {


/** This concrete class implements an AND Gate with up to 8 inputs
 */
class And8Gate : public AndGateBase
{
public:
    /// Type ID for the card
    static constexpr const char*    GUID_STRING = "e62e395c-d27a-4821-bba9-aa1e6de42a05";

    /// Type name for the card
    static constexpr const char*    TYPE_NAME   = "Fxt::Component::Digital::And8Gate";

public:
    /// Maximum number of Input signals
    static constexpr unsigned MAX_INPUTS = 8;


public:
    /// Constructor
    And8Gate( JsonVariant&                       componentObject,
              Cpl::Memory::ContiguousAllocator&  generalAllocator,
              Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
              Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
              Fxt::Point::DatabaseApi&           dbForPoints )
        :AndGateBase()
    {
        parseConfiguration( generalAllocator, componentObject, MAX_INPUTS );
    }

public:
    /// See Fxt::Component::Api
    const char* getTypeGuid() const noexcept
    {
        return GUID_STRING;
    }

    /// See Fxt::Component::Api
    const char* getTypeName() const noexcept
    {
        return TYPE_NAME;
    }
};



};      // end namespaces
};
};
#endif  // end header latch