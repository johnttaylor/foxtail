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


#include "Database.h"
#include "Cpl/System/Assert.h"

///
using namespace Fxt::Card;

///////////////////////////////////////////////////////////////////////////////
Database::Database() noexcept
    : m_cards()
    , m_factories()
{
}

Database::Database( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
    : m_cards( ignoreThisParameter_usedToCreateAUniqueConstructor )
    , m_factories( ignoreThisParameter_usedToCreateAUniqueConstructor )
{
}


///////////////////////////////////////////////////////////////////////////////
Api* Database::lookupCard( uint16_t cardLocalId ) noexcept
{
    Api* item  = m_cards.first();
    while ( item )
    {
        if ( item->getLocalId() == cardLocalId )
        {
            return item;
        }
        item = m_cards.next( *item );
    }

    return nullptr;
}

Api* Database::getFirstCard() noexcept
{
    return m_cards.first();
}

Api* Database::getNextCard( Api& currentCard ) noexcept
{
    return m_cards.next( currentCard );
}

FactoryApi* Database::lookupFactory( Cpl::Type::Guid_T cardTypeId ) noexcept
{
    FactoryApi* item  = m_factories.first();
    while ( item )
    {
        if ( item->getGuid() == cardTypeId )
        {
            return item;
        }
        item = m_factories.next( *item );
    }

    return nullptr;
}

FactoryApi* Database::getFirstFactory() noexcept
{
    return m_factories.first();
}

FactoryApi* Database::getNextFactory( FactoryApi& currentFactory ) noexcept
{
    return m_factories.next( currentFactory );
}



///////////////////////////////////////////////////////////////////////////////
void Database::insert_( Api& cardToAdd ) noexcept
{
    m_cards.put( cardToAdd );
}

void Database::remove_( Api& cardToRemove ) noexcept
{
    m_cards.remove( cardToRemove );
}

void Database::insert_( FactoryApi& cardFactoryToAdd ) noexcept
{
    m_factories.put( cardFactoryToAdd );
}
