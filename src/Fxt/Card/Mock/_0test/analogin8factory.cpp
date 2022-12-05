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
#include "Fxt/Card/Mock/AnalogIn8.h"
#include "Fxt/Card/Mock/AnalogIn8Factory.h"
#include "Fxt/Point/Uint8.h"
#include "Fxt/Point/Database.h"
#include "Fxt/Card/FactoryDatabase.h"
#include "Cpl/Memory/LeanHeap.h"
#include "Cpl/System/Trace.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Card::Mock;


#define CARD_DEFINTION     "{\"cards\":[" \
                           "{" \
                           "  \"name\": \"bob\"," \
                           "  \"id\": 1," \
                           "  \"type\": \"1968f533-e323-4ae4-8493-9a572f3bd195\"," \
                           "  \"typename\": \"Fxt::Card::HW::Mock::AnalogIn8\"," \
                           "  \"slot\": 22," \
                           "    \"points\": {" \
                           "       \"inputs\": [" \
                           "           {" \
                           "               \"channel\": 2," \
                           "                   \"id\": 1," \
                           "                   \"ioRegId\": 2," \
                           "                   \"name\": \"Motor Temperature\"," \
                           "                   \"initial\": {" \
                           "                     \"valid\": true," \
                           "                     \"val\": 1.2," \
                           "                     \"id\": 0" \
                           "                   }" \
                           "           }," \
                           "           {" \
                           "               \"channel\": 5," \
                           "                   \"id\": 3," \
                           "                   \"ioRegId\": 4," \
                           "                   \"name\": \"Motor Voltage\"," \
                           "                   \"initial\": {" \
                           "                     \"valid\": true," \
                           "                     \"val\": 3.5," \
                           "                     \"id\": 0" \
                           "                   }" \
                           "           }," \
                           "           {" \
                           "               \"channel\": 6," \
                           "                   \"id\": 5," \
                           "                   \"ioRegId\": 6," \
                           "                   \"name\": \"Motor Current\"," \
                           "                   \"initial\": {" \
                           "                     \"valid\": true," \
                           "                     \"val\": 0.0," \
                           "                     \"id\": 0" \
                           "                   }" \
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
TEST_CASE( "AnalogIn8Factory" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap            generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap            statefulAllocator( statefulHeap_, sizeof( statefulHeap_ ) );
    Fxt::Point::Database<MAX_POINTS> pointDb;
    Fxt::Card::FactoryDatabase       cardFactoryDb;
    AnalogIn8Factory                 uut( cardFactoryDb );
    Fxt::Type::Error                 componentErrorCode;
    Cpl::Text::FString<Fxt::Type::Error::MAX_TEXT_LEN> errText;

    SECTION( "create/destroy card" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, CARD_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant cardObj = doc["cards"][0];
        Fxt::Card::Api* card = uut.create( cardObj, componentErrorCode, generalAllocator, statefulAllocator, pointDb );
        REQUIRE( card != nullptr );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("error Code=%s", componentErrorCode.toText( errText )) );
        REQUIRE( componentErrorCode == Fxt::Type::Error::SUCCESS()  );

        REQUIRE( strcmp( uut.getGuid(), card->getTypeGuid() ) == 0 );

        REQUIRE( strcmp( card->getTypeName(), AnalogIn8::TYPE_NAME ) == 0 );
        REQUIRE( strcmp( card->getTypeGuid(), AnalogIn8::GUID_STRING ) == 0 );

        Fxt::Point::Uint8* pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 1 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "Motor Temperature" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 2 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "Motor Temperature" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 3 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "Motor Voltage" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 4 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "Motor Voltage" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 5 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "Motor Current" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 6 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "Motor Current" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        uut.destroy( *card );
    }

    SECTION( "create from factory db" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, CARD_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant cardObj = doc["cards"][0];
        Fxt::Card::Api* card = cardFactoryDb.createCardfromJSON( cardObj, generalAllocator, statefulAllocator, pointDb, componentErrorCode );
        REQUIRE( card != nullptr );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("error Code=%s", componentErrorCode.toText( errText )) );
        REQUIRE( componentErrorCode == Fxt::Type::Error::SUCCESS()  );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}