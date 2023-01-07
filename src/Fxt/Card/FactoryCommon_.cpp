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


#include "FactoryCommon_.h"


///
using namespace Fxt::Card;

FactoryCommon_::FactoryCommon_( FactoryDatabaseApi& factoryDatabase )
{
    // Auto register with factory database
    factoryDatabase.put( *this );
}

FactoryCommon_::~FactoryCommon_()
{
}


void FactoryCommon_::destroy( Api& cardToDestory ) noexcept
{
    // Call the card's destructor. Note: The Application is still responsible for 'freeing/reseting' the card's memory allocator
    cardToDestory.~Api();
}

Fxt::Type::Error FactoryCommon_::allocateAndParse( JsonVariant&                       obj,
                                                   Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                                   size_t                             cardSizeInBytes,
                                                   void*&                             memoryForCard ) noexcept
{
    // Allocate memory for the card
    memoryForCard = generalAllocator.allocate( cardSizeInBytes );
    if ( memoryForCard == nullptr )
    {
        return fullErr( Err_T::MEMORY_CARD );
    }

    return Fxt::Type::Error::SUCCESS();
}