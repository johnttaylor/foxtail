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

#include "AnalogIn8.h"
#include "AnalogIn8Factory.h"
#include "Cpl/System/Assert.h"
#include <new>


using namespace Fxt::Card::HW::Mock;


AnalogIn8Factory::AnalogIn8Factory( FactoryDatabaseApi&                 factoryDatabase,
                                    Cpl::Memory::ContiguousAllocator&   generalAllocator,
                                    Cpl::Memory::ContiguousAllocator&   statefulDataAllocator,
                                    Fxt::Point::DatabaseApi&            dbForPoints )
    :FactoryCommon_( factoryDatabase, generalAllocator, statefulDataAllocator, dbForPoints )
{
}

AnalogIn8Factory::~AnalogIn8Factory()
{
}

Fxt::Card::Api* AnalogIn8Factory::create( DatabaseApi& cardDb,
                                          JsonVariant& cardObject,
                                          Api::Err_T&  cardErrorCode ) noexcept
{
    // Allocate memory for the card
    void* memCardInstance = m_generalAllocator.allocate( sizeof( AnalogIn8 ) );
    if ( memCardInstance == nullptr )
    {
        cardErrorCode = FXT_CARD_ERR_CARD_MEMORY;
        return nullptr;
    }

    // Get basic info about the card
    uint16_t cardId;
    cardErrorCode = parseBasicFields( cardObject, cardId );
    if ( cardErrorCode == FXT_CARD_ERR_NO_ERROR )
    {
        // Create the card
        AnalogIn8* card = new(memCardInstance) AnalogIn8( cardDb,
                                                          m_generalAllocator,
                                                          m_statefulDataAllocator,
                                                          m_pointDb,
                                                          cardId,
                                                          cardObject );

        cardErrorCode = card->getErrorCode();
        return card;
    }

    return nullptr;
}
