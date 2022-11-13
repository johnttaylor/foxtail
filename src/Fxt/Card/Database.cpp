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
Api* Database::createCardfromJSON( JsonVariant cardObj, uint32_t cardErrorCode ) noexcept
{
    // Ensure that a Id has been assigned
    const char* typeGuid = cardObj["type"];
    if ( typeGuid == nullptr )
    {
        cardErrorCode = FXT_CARD_ERR_UNKNOWN_GUID;
        return nullptr;
    }
    
    FactoryApi* factory = lookupFactory( typeGuid );
    if ( factory == nullptr )
    {
        cardErrorCode = FXT_CARD_ERR_UNKNOWN_GUID;
        return nullptr;
    }

    return factory->create( cardObj, cardErrorCode );
}


///////////////////////////////////////////////////////////////////////////////
Api* Database::lookupCard( uint16_t cardId ) noexcept
{
    Api* item  = m_cards.first();
    while ( item )
    {
        if ( item->getId() == cardId )
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

FactoryApi* Database::lookupFactory( const char* guidCardTypeId ) noexcept
{
    if ( guidCardTypeId )
    {
        FactoryApi* item  = m_factories.first();
        while ( item )
        {
            if ( strcmp( item->getGuid(), guidCardTypeId ) == 0 )
            {
                return item;
            }
            item = m_factories.next( *item );
        }
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


void Database::clearCards() noexcept
{
    // Drain the list 
    Api* item = m_cards.get();
    while ( item )
    {
        // Call the Card destructor (but do not free the memory)
        item->~Api();
        item = m_cards.get();
    }
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

void Database::remove_( FactoryApi& cardFactoryToRemove ) noexcept
{
    m_factories.remove( cardFactoryToRemove );
}
