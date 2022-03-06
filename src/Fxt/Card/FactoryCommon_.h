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
#include "Fxt/Card/Database.h"
#include "Cpl/Container/Dictionary.h"
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
    FactoryCommon_( Fxt::Card::DatabaseApi&                             cardDb,
                    Cpl::Container::Dictionary<Fxt::Point::Descriptor>& descriptorDatabase,
                    Fxt::Point::Database&                               pointDatabase,
                    PointAllocators_T&                                  pointAllocators,
                    Cpl::Memory::ContiguousAllocator&                   allocatorForCard );

    /// Destructor
    ~FactoryCommon_();

public:
    /// See Fxt::Card::FactoryApi
    void destroy( Api& cardToDestory ) noexcept;


protected:
    /// Card database
    Fxt::Card::DatabaseApi&                             m_cardDb;

    /// Point Descriptor Database
    Cpl::Container::Dictionary<Fxt::Point::Descriptor>& m_descriptorDatabase;
    
    /// Point instance database
    Fxt::Point::Database&                               m_pointDatabase;

    /// Point Allocators
    PointAllocators_T&                                  m_pointAllocators;
    
    /// General pupose allocator (i.e. everything BUT points)
    Cpl::Memory::ContiguousAllocator&                   m_allocatorForCard;
};



};      // end namespaces
};
#endif  // end header latch
