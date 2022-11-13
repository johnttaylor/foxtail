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
#include "Fxt/Card/HW/Mock/Digital8.h"
#include "Fxt/Card/HW/Mock/Digital8Factory.h"
#include "Fxt/Point/Database.h"
#include "Fxt/Card/Database.h"
#include "Cpl/Memory/LeanHeap.h"
#include "Cpl/System/Trace.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Card::HW::Mock;


#define CARD_DEFINTION     "{\"cards\":[" \
                           "{" \
                           "  \"name\": \"bob\"," \
                           "  \"id\": 11," \
                           "  \"type\": \"59d33888-62c7-45b2-a4d4-9dbc55914ed3\"," \
                           "  \"typename\": \"Fxt::Card::HW::Mock::Digital8\"," \
                           "  \"slot\": 22," \
                           "    \"points\": {" \
                           "       \"inputs\": [" \
                           "           {" \
                           "               \"channel\": 1," \
                           "                   \"id\": 1," \
                           "                   \"ioRegId\": 2," \
                           "                   \"name\": \"InputPt\"," \
                           "                   \"initial\": {" \
                           "                     \"valid\": true," \
                           "                     \"val\": 128," \
                           "                     \"id\": 0" \
                           "                   }" \
                           "           }" \
                           "       ]," \
                           "       \"outputs\": [" \
                           "           {" \
                           "              \"channel\": 1," \
                           "              \"id\": 3," \
                           "              \"ioRegId\": 4," \
                           "              \"name\": \"OutputPt\"," \
                           "              \"initial\": {" \
                           "                 \"valid\": true," \
                           "                 \"val\": 1," \
                           "                 \"id\": 0" \
                           "              }" \
                           "           }" \
                           "       ]" \
                           "    }" \
                           "}" \
                           "]}"

static size_t generalHeap_[10000];
static size_t statefulHeap_[10000];

#define MAX_POINTS      100

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Digitl8Factory" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap            generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap            statefulAllocator( statefulHeap_, sizeof( statefulHeap_ ) );
    Fxt::Point::Database<MAX_POINTS> pointDb;
    Fxt::Card::Database              cardDb;
    Digital8Factory                  uut( cardDb, generalAllocator, statefulAllocator, pointDb );
    uint32_t                         cardErrorCode;

    SECTION( "create/destroy card" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, CARD_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant cardObj = doc["cards"][0];
        Fxt::Card::Api* card = uut.create( cardObj, cardErrorCode );
        REQUIRE( card != nullptr );
        REQUIRE( cardErrorCode == FXT_CARD_ERR_NO_ERROR );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("error Code=%s", Fxt::Card::Api::getErrorText( cardErrorCode )) );

        REQUIRE( cardDb.lookupCard( 11 ) == card );
        REQUIRE( strcmp( card->getTypeName(), Digital8::TYPE_NAME ) == 0 );
        REQUIRE( strcmp( card->getTypeGuid(), Digital8::GUID_STRING ) == 0 );

        REQUIRE( card->getId() == 11 );
        REQUIRE( card->getSlot() == 22 );
        REQUIRE( strcmp( card->getName(), "bob" ) == 0 );

        Fxt::Point::Uint8* pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 1 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "InputPt" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 2 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "InputPt" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 3 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "OutputPt" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 4 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "OutputPt" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        uut.destroy( *card );
    }


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}