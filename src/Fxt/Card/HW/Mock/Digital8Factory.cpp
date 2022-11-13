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

#include "Digital8.h"
#include "Digital8Factory.h"
#include "Cpl/System/Assert.h"
#include <new>


using namespace Fxt::Card::HW::Mock;


Digital8Factory::Digital8Factory( Fxt::Card::DatabaseApi&             cardDb,
                                  Cpl::Memory::ContiguousAllocator&   generalAllocator,
                                  Cpl::Memory::ContiguousAllocator&   statefulDataAllocator,
                                  Fxt::Point::DatabaseApi&            dbForPoints )
    :FactoryCommon_( cardDb, generalAllocator, statefulDataAllocator, dbForPoints )
{
}

Digital8Factory::~Digital8Factory()
{
}

Fxt::Card::Api* Digital8Factory::create( JsonVariant& cardObject, uint32_t& cardErrorCode ) noexcept
{
    // Allocate memory for the card
    void* memCardInstance = m_generalAllocator.allocate( sizeof( Digital8 ) );
    if ( memCardInstance == nullptr )
    {
        cardErrorCode = FXT_CARD_ERR_CARD_MEMORY;
        return nullptr;
    }

    // Get basic info about the card
    uint16_t    cardId;
    uint16_t    slotNumber;
    const char* cardName = parseBasicFields( cardObject, cardId, slotNumber, cardErrorCode );
    if ( cardName == nullptr )
    {
        return nullptr;
    }
    
    // Create the card
    Digital8* card = new(memCardInstance) Digital8( m_generalAllocator,
                                                    m_statefulDataAllocator,
                                                    m_pointDb,
                                                    cardId,
                                                    slotNumber,
                                                    cardName,
                                                    cardObject );

    m_cardDb.insert_( *card );
    cardErrorCode = FXT_CARD_ERR_NO_ERROR;
    return card;
}
