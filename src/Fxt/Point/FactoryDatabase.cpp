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
using namespace Fxt::Point;

///////////////////////////////////////////////////////////////////////////////
FactoryDatabase::FactoryDatabase() noexcept
    : m_factories()
{
}

FactoryDatabase::FactoryDatabase( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
    : m_factories( ignoreThisParameter_usedToCreateAUniqueConstructor )
{
}

///////////////////////////////////////////////////////////////////////////////
Api* FactoryDatabase::createPointfromJSON( JsonObject&                        pointObject,
                                           Fxt::Type::Error&                  pointErrorCode,
                                           Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                           Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                                           Fxt::Point::DatabaseApi&           dbForPoints,
                                           const char*                        pointIdKeyName,
                                           bool                               createSetter  ) noexcept
{
    // Ensure that a Id has been assigned
    const char* typeGuid = pointObject["type"];
    if ( typeGuid == nullptr )
    {
        pointErrorCode = fullErr( Err_T::UNKNOWN_GUID );
        return nullptr;
    }

    FactoryApi* factory = lookup( typeGuid );
    if ( factory == nullptr )
    {
        pointErrorCode = fullErr( Err_T::UNKNOWN_GUID );
        return nullptr;
    }

    return factory->create( pointObject, pointErrorCode, generalAllocator, statefulDataAllocator, dbForPoints, pointIdKeyName, createSetter );
}


///////////////////////////////////////////////////////////////////////////////
FactoryApi* FactoryDatabase::lookup( const char* guidPointTypeId ) noexcept
{
    if ( guidPointTypeId )
    {
        FactoryApi* item  = m_factories.first();
        while ( item )
        {
            if ( strcmp( item->getGuid(), guidPointTypeId ) == 0 )
            {
                return item;
            }
            item = m_factories.next( *item );
        }
    }

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
FactoryApi* FactoryDatabase::first() noexcept
{
    return m_factories.first();
}

FactoryApi* FactoryDatabase::next( FactoryApi& currentFactory ) noexcept
{
    return m_factories.next( currentFactory );
}

void FactoryDatabase::insert_( FactoryApi& pointFactoryToAdd ) noexcept
{
    m_factories.put( pointFactoryToAdd );
}

void FactoryDatabase::remove_( FactoryApi& pointFactoryToRemove ) noexcept
{
    m_factories.remove( pointFactoryToRemove );
}
