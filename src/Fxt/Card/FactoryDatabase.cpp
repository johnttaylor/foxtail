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


#include "FactoryDatabase.h"
#include "Error.h"
#include "Cpl/System/Assert.h"

///
using namespace Fxt::Card;

///////////////////////////////////////////////////////////////////////////////
FactoryDatabase::FactoryDatabase() noexcept
    : FactoryDatabaseApi()
{
}

FactoryDatabase::FactoryDatabase( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
    : FactoryDatabaseApi( ignoreThisParameter_usedToCreateAUniqueConstructor )
{
}

///////////////////////////////////////////////////////////////////////////////
Api* FactoryDatabase::createCardfromJSON( JsonVariant                        cardObj,
                                          Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                          Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                                          Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                                          Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                                          Fxt::Point::DatabaseApi&           dbForPoints,
                                          Fxt::Type::Error&                  cardErrorCode ) noexcept
{
    // Ensure that a Id has been assigned
    const char* typeGuid = cardObj["type"];
    if ( typeGuid == nullptr )
    {
        cardErrorCode = fullErr( Err_T::UNKNOWN_GUID );
        cardErrorCode.logIt();
        return nullptr;
    }

    FactoryApi* factory = lookup( typeGuid );
    if ( factory == nullptr )
    {
        cardErrorCode = fullErr( Err_T::UNKNOWN_GUID );
        cardErrorCode.logIt( typeGuid );
        return nullptr;
    }

    return factory->create( cardObj, cardErrorCode, generalAllocator, cardStatefulDataAllocator, haStatefulDataAllocator, pointFactoryDb, dbForPoints );
}


///////////////////////////////////////////////////////////////////////////////
FactoryApi* FactoryDatabase::lookup( const char* guidCardTypeId ) noexcept
{
    if ( guidCardTypeId )
    {
        FactoryApi* item  = first();
        while ( item )
        {
            if ( strcmp( item->getGuid(), guidCardTypeId ) == 0 )
            {
                return item;
            }
            item = next( *item );
        }
    }

    return nullptr;
}
