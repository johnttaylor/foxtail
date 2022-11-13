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

FactoryCommon_::FactoryCommon_( FactoryDatabaseApi&                 factoryDatabase, 
                                Cpl::Memory::ContiguousAllocator&   generalAllocator,
                                Cpl::Memory::ContiguousAllocator&   statefulDataAllocator,
                                Fxt::Point::DatabaseApi&            m_pointDb )
    : m_pointDb( m_pointDb )
    , m_generalAllocator( generalAllocator )
    , m_statefulDataAllocator( statefulDataAllocator )
{
    // Auto register with factory database
    factoryDatabase.insert_( *this );
}

FactoryCommon_::~FactoryCommon_()
{
}


void FactoryCommon_::destroy( Api& cardToDestory ) noexcept
{
    // Call the card's destructor. Note: The Application is still responsible for 'freeing/reseting' the card's memory allocator
    cardToDestory.~Api();
}

const char* FactoryCommon_::parseBasicFields( JsonVariant& obj,
                                              uint16_t&    cardId,
                                              uint16_t&    slotNumber,
                                              uint32_t&    errorCode ) noexcept
{
    // Ensure that a Id has been assigned
    if ( obj["id"].isNull() )
    {
        errorCode = FXT_CARD_ERR_CARD_MISSING_ID;
        return nullptr;
    }
    cardId = obj["id"];

    // Ensure that a Slot ID has been assigned
    if ( obj["slot"].isNull() )
    {
        errorCode = FXT_CARD_ERR_CARD_MISSING_SLOT_ID;
        return nullptr;
    }
    slotNumber = obj["slot"];

    // Get the Text label
    const char* name = obj["name"];
    if ( name == nullptr )
    {
        errorCode = FXT_CARD_ERR_CARD_MISSING_NAME;
        return nullptr;
    }
    char* memory4Name = (char*) m_generalAllocator.allocate( strlen( name ) + 1 );
    if ( memory4Name == nullptr )
    {
        errorCode = FXT_CARD_ERR_MEMORY_CARD_NAME;
        return nullptr;
    }
    strcpy( memory4Name, name );
    return memory4Name;
}