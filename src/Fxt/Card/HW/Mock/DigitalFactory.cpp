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

#include "DigitalFactory.h"
#include "Cpl/System/Assert.h"
#include <new>


using namespace Fxt::Card::HW::Mock;


DigitalFactory::DigitalFactory( Fxt::Card::DatabaseApi&                             cardDb,
                                Cpl::Container::Dictionary<Fxt::Point::Descriptor>& descriptorDatabase,
                                Fxt::Point::Database&                               pointDatabase,
                                PointAllocators_T&                                  pointAllocators,
                                Cpl::Memory::ContiguousAllocator&                   allocatorForCard )
    :FactoryCommon_( cardDb, descriptorDatabase, pointDatabase, pointAllocators, allocatorForCard )
{
}

DigitalFactory::~DigitalFactory()
{
}

bool DigitalFactory::create( JsonVariant& cardObject ) noexcept
{
    void* memCardInstance = m_allocatorForCard.allocate( sizeof( Digital ) );
    if ( memCardInstance == nullptr )
    {
        return false;
    }

    Digital* card = new(memCardInstance) Digital( cardObject, m_pointAllocators, m_pointDatabase, m_descriptorDatabase, m_allocatorForCard );
    m_cardDb.insert_( *card );
    return true;
}
