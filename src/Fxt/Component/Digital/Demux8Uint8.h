#ifndef Fxt_Component_Digital_Demux8u_h_
#define Fxt_Component_Digital_Demux8u_h_
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


#include "Fxt/Component/Digital/DemuxBase.h"
#include "Fxt/Point/Uint8.h"


///
namespace Fxt {
///
namespace Component {
///
namespace Digital {


/** This concrete class implements a Demux8Uint8 that has Uin8t as it input
    type
 */
class Demux8Uint8 : public DemuxBase
{
public:
    /// Type ID for the card
    static constexpr const char*    GUID_STRING = "8c55aa52-3bc8-4b8a-ad73-c434a0bbd4b4";

    /// Type name for the card
    static constexpr const char*    TYPE_NAME   = "Fxt::Component::Digital::Demux8Uint8";

public:

    /// Maximum number of Output signals
    static constexpr unsigned MAX_OUTPUTS = (8 * 2);      // Each bit has two possible outputs

public:
    /// Constructor
    Demux8Uint8( JsonVariant&                       componentObject,
             Cpl::Memory::ContiguousAllocator&  generalAllocator,
             Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
             Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
             Fxt::Point::DatabaseApi&           dbForPoints )
        : DemuxBase()
    {
        parseConfiguration( generalAllocator, componentObject, MAX_OUTPUTS );
    }


public:
    /// See Fxt::Component::Api
    Fxt::Type::Error execute( int64_t currentTickUsec ) noexcept
    {
        return doExecute<Fxt::Point::Uint8, uint8_t>( currentTickUsec );
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

protected:
    /// See Fxt::Component::Digital::DemuxBase
    bool validateInputType() noexcept
    {
        return Fxt::Point::Api::validatePointTypes( m_inputRefs, 1, Fxt::Point::Uint8::GUID_STRING );
    }
};



};      // end namespaces
};
};
#endif  // end header latch
