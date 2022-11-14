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
#include "Cpl/Memory/LeanHeap.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Card::HW::Mock;


#define CARD_DEFINTION     "\"cards\":[" \
                           "{" \
                           "  \"name\": \"My Digital Card\"," \
                           "  \"id\": 0," \
                           "  \"type\": \"59d33888-62c7-45b2-a4d4-9dbc55914ed3\"," \
                           "  \"typename\": \"Fxt::Card::HW::Mock::Digital8\"," \
                           "  \"slot\": 0," \
                           "  \"points\": {" \
                           "  \"inputs\": [" \
                           "  {" \
                           "      \"channel\": 1," \
                           "          \"id\": 100," \
                           "          \"ioRegId\": 101," \
                           "          \"name\": \"InputPt\"," \
                           "          \"initial\": {" \
                           "          \"valid\": true," \
                           "          \"val\": 128," \
                           "          \"id\": 0" \
                           "  }" \
                           "        }" \
                           "      ]," \
                           "      \"outputs\": [" \
                           "      {" \
                           "          \"channel\": 1," \
                           "              \"id\": 200," \
                           "              \"ioRegId\": 201," \
                           "              \"name\": \"OutputPt\"," \
                           "              \"initial\": {" \
                           "              \"valid\": true," \
                           "              \"val\": 1," \
                           "              \"id\": 0" \
                           "      }" \
                           "        }" \
                           "      ]" \
                           "    }" \
                           "  }" \
                           "]"

static size_t generalHeap_[10000];
static size_t statefulHeap_[10000];

#define MAX_POINTS      100

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Digitl8" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap statefulAllocator( statefulHeap_, sizeof( statefulHeap_ ) );
    Fxt::Point::Database<MAX_POINTS> pointDb;


    SECTION( "create card" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, CARD_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant cardObj = doc["cards"][0];
        Digital8 uut( generalAllocator,
                      statefulAllocator,
                      pointDb,
                      11,
                      22,
                      "bob",
                      cardObj );

        REQUIRE( uut.getErrorCode() == FXT_CARD_ERR_NO_ERROR );
        REQUIRE( uut.getId() == 11 );
        REQUIRE( uut.getSlot() == 22 );
        REQUIRE( strcmp( uut.getName(), "bob" ) == 0 );

        Fxt::Point::Uint8* pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 100 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "InputPt" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 101 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "InputPt" ) == 0 );
        uint8_t pointVal = 0;
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 200 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "OutputPt" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 201 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "OutputPt" ) == 0 );
        uint8_t pointVal = 0;
        REQUIRE( pointPtr->isNotValid() );

        REQUIRE( uut.start() );
        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 101 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( pointVal == 128 );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 201 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( pointVal == 1 );
    }


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}