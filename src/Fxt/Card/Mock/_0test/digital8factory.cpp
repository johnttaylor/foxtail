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

#include "Catch/catch.hpp"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Fxt/Card/Mock/Digital8.h"
#include "Fxt/Card/Mock/Digital8Factory.h"
#include "Fxt/Point/Uint8.h"
#include "Fxt/Point/Database.h"
#include "Fxt/Point/FactoryDatabase.h"
#include "Fxt/Point/Factory.h"
#include "Fxt/Card/FactoryDatabase.h"
#include "Cpl/Memory/LeanHeap.h"
#include "Cpl/System/Trace.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Card::Mock;


#define CARD_DEFINTION     "{\"cards\":[" \
                           "{" \
                           "  \"name\": \"My Digital8 Card\"," \
                           "  \"type\": \"59d33888-62c7-45b2-a4d4-9dbc55914ed3\"," \
                           "  \"typename\": \"Fxt::Card::HW::Mock::Digital8\"," \
                           "  \"slot\": 0," \
                           "    \"points\": {" \
                           "       \"inputs\": [" \
                           "           {" \
                           "               \"channel\": 1," \
                           "                \"id\": 1," \
                           "                \"ioRegId\": 2," \
                           "                \"type\": \"918cff9e-8007-4666-99ac-384b9624329c\"," \
                           "                \"typeName\": \"Fxt::Point::Uint8\"," \
                           "                \"name\": \"InputPt\"," \
                           "                \"initial\": {" \
                           "                  \"valid\": true," \
                           "                  \"val\": 128," \
                           "                  \"id\": 0" \
                           "                }" \
                           "           }" \
                           "       ]," \
                           "       \"outputs\": [" \
                           "           {" \
                           "              \"channel\": 1," \
                           "              \"id\": 3," \
                           "              \"ioRegId\": 4," \
                           "              \"type\": \"918cff9e-8007-4666-99ac-384b9624329c\"," \
                           "              \"typeName\": \"Fxt::Point::Uint8\"," \
                           "              \"name\": \"OutputPt\"," \
                           "              \"initial\": {" \
                           "                 \"valid\": true," \
                           "                 \"val\": 1," \
                           "                 \"id\": 5" \
                           "              }" \
                           "           }" \
                           "       ]" \
                           "    }" \
                           "}" \
                           "]}"

static size_t generalHeap_[10000];
static size_t statefulHeap_[10000];

#define MAX_POINTS      100
#define MAX_CARDS       3

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Digital8Factory" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap                              generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap                              statefulAllocator( statefulHeap_, sizeof( statefulHeap_ ) );
    Fxt::Point::Database<MAX_POINTS>                   pointDb;
    Fxt::Point::FactoryDatabase                        pointFactoryDb;
    Fxt::Point::Factory<Fxt::Point::Uint8>             factoryUint8( pointFactoryDb );
    Fxt::Card::FactoryDatabase                         cardFactoryDb;
    Digital8Factory                                    uut( cardFactoryDb );
    Fxt::Type::Error                                   cardErrorCode;
    Cpl::Text::FString<Fxt::Type::Error::MAX_TEXT_LEN> errText;


    SECTION( "create/destroy card" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, CARD_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant cardObj = doc["cards"][0];
        Fxt::Card::Api* card = uut.create( cardObj, cardErrorCode, generalAllocator, statefulAllocator, pointFactoryDb, pointDb );
        REQUIRE( card != nullptr );
        REQUIRE( cardErrorCode == Fxt::Type::Error::SUCCESS()  );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("error Code=%s", Fxt::Type::Error::toText( cardErrorCode, errText )) );

        REQUIRE( strcmp( uut.getGuid(), card->getTypeGuid() ) == 0 );

        REQUIRE( strcmp( card->getTypeName(), Digital8::TYPE_NAME ) == 0 );
        REQUIRE( strcmp( card->getTypeGuid(), Digital8::GUID_STRING ) == 0 );
        REQUIRE( card->getSlotNumber() == 0 );

        Fxt::Point::Uint8* pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 1 );
        REQUIRE( pointPtr );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 2 );
        REQUIRE( pointPtr );
        REQUIRE( pointPtr->isNotValid() == false );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 3 );
        REQUIRE( pointPtr );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 4 );
        REQUIRE( pointPtr );
        REQUIRE( pointPtr->isNotValid() == false );

        uut.destroy( *card );
    }

    SECTION( "create from factory db" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, CARD_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant cardObj = doc["cards"][0];
        Fxt::Card::Api* card = cardFactoryDb.createCardfromJSON( cardObj, generalAllocator, statefulAllocator, pointFactoryDb, pointDb, cardErrorCode );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("error Code=%s", Fxt::Type::Error::toText( cardErrorCode, errText )) );
        REQUIRE( card != nullptr );
        REQUIRE( cardErrorCode == Fxt::Type::Error::SUCCESS()  );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}