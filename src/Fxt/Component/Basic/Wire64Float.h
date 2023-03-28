#ifndef Fxt_Component_Basic_Wire64Float_h_
#define Fxt_Component_Basic_Wire64Float_h_
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


#include "Fxt/Component/Basic/Wire64Base.h"
#include "Fxt/Point/Float.h"

///
namespace Fxt {
///
namespace Component {
///
namespace Basic {


/** This concrete class implements a Component that connects two Floatean
    points. Up to 64 inputs/output pairs are supported.
 */
class Wire64Float : public Wire64Base
{
public:
    /// Type ID for the card
    static constexpr const char*    GUID_STRING = "e6759a22-06c1-4aad-8190-67bf36425903";

    /// Type name for the card
    static constexpr const char*    TYPE_NAME   = "Fxt::Component::Basic::Wire64Float";

    /// Size (in bytes) of Stateful data that will be allocated on the HA Heap
    static constexpr const size_t   HA_STATEFUL_HEAP_SIZE = 0;

public:
    /// Constructor
    Wire64Float( JsonVariant&                       componentObject,
                 Cpl::Memory::ContiguousAllocator&  generalAllocator,
                 Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                 Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                 Fxt::Point::DatabaseApi&           dbForPoints )
        :Wire64Base()
    {
        parseConfiguration( generalAllocator, componentObject, 1, MAX_INPUTS, 1, MAX_OUTPUTS );
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
    /// See Fxt::Component::Basic::Wire64
    const char* getPointTypeGuid() const noexcept
    {
        return Fxt::Point::Float::GUID_STRING;
    }

    /// See Fxt::Component::Basic::Wire64
    void copyPoint( unsigned refIndex ) noexcept
    {
        Fxt::Point::Float* inPt  = (Fxt::Point::Float*) m_inputRefs[refIndex];
        Fxt::Point::Float* outPt = (Fxt::Point::Float*) m_outputRefs[refIndex];
        outPt->write( *inPt );
    }
};



};      // end namespaces
};
};
#endif  // end header latch