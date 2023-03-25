#ifndef Fxt_Component_Digital_Mux8Uint8_h_
#define Fxt_Component_Digital_Mux8Uint8_h_
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


#include "Fxt/Component/Digital/MuxBase.h"
#include "Fxt/Point/Uint8.h"

///
namespace Fxt {
///
namespace Component {
///
namespace Digital {


/** This concrete class implements a Mux with up to 8 inputs and outputs
    a UINT8 point.
 */
class Mux8Uint8 : public MuxBase
{
public:
    /// Type ID for the card
    static constexpr const char*    GUID_STRING = "d60f2daf-9709-42d6-ba92-b76f641eb930";

    /// Type name for the card
    static constexpr const char*    TYPE_NAME   = "Fxt::Component::Digital::Mux8Uint8";

public:
    /// Maximum number of Input signals
    static constexpr unsigned MAX_INPUTS = 8;

public:
    /// Constructor
    Mux8Uint8( JsonVariant&                       componentObject,
               Cpl::Memory::ContiguousAllocator&  generalAllocator,
               Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
               Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
               Fxt::Point::DatabaseApi&           dbForPoints )
        :MuxBase()
    {
        parseConfiguration( generalAllocator, componentObject, MAX_INPUTS );
    }

public:
    /// See Fxt::Component::Api
    Fxt::Type::Error execute( int64_t currentTickUsec ) noexcept
    {
        return doExecute<Fxt::Point::Uint8, uint8_t>( currentTickUsec );
    }

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

protected:
    /// See Fxt::Component::Digital::MuxBase
    bool validateOutputType() noexcept
    {
        return Fxt::Point::Api::validatePointTypes( m_outputRefs, 1, Fxt::Point::Uint8::GUID_STRING );
    }


public:
};



};      // end namespaces
};
};
#endif  // end header latch
