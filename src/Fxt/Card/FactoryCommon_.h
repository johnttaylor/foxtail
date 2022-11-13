#ifndef Fxt_Card_FactoryCommon_h_
#define Fxt_Card_FactoryCommon_h_
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


#include "Fxt/Card/FactoryApi.h"
#include "Fxt/Card/FactoryDatabaseApi.h"
#include "Cpl/Memory/ContiguousAllocator.h"

///
namespace Fxt {
///
namespace Card {


/** This partially concrete class provide common infrastructure for a Card
    Factory.
 */
class FactoryCommon_ : public Fxt::Card::FactoryApi
{
public:
    /// Constructor
    FactoryCommon_( FactoryDatabaseApi&                 factoryDatabase,
                    Cpl::Memory::ContiguousAllocator&   generalAllocator,
                    Cpl::Memory::ContiguousAllocator&   statefulDataAllocator,
                    Fxt::Point::DatabaseApi&            dbForPoints );

    /// Destructor
    ~FactoryCommon_();

public:
    /// See Fxt::Card::FactoryApi
    void destroy( Api& cardToDestory ) noexcept;

protected:
    /** Helper method to parse basic/common fields for a card.  Returns the
        card name if successful; else nullptr is returned.  Other fields are
        returned via the function arguments.
     */
    const char* parseBasicFields( JsonVariant& obj,
                                  uint16_t&    cardId,
                                  uint16_t&    slotNumber,
                                  uint32_t&    errorCode ) noexcept;

protected:
    /// Point instance database
    Fxt::Point::DatabaseApi&            m_pointDb;

    /// General Allocator
    Cpl::Memory::ContiguousAllocator&   m_generalAllocator;

    /// Stateful data allocator
    Cpl::Memory::ContiguousAllocator&   m_statefulDataAllocator;
};



};      // end namespaces
};
#endif  // end header latch
