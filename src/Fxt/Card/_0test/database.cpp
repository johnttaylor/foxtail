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
#include "Fxt/Card/Database.h"
#include "Fxt/Point/Database.h"
#include "Fxt/Point/Uint8.h"
#include "Cpl/Memory/LeanHeap.h"
#include "Cpl/System/Trace.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Card;


#define CARD_DEFINTION     "{\"cards\":[" \
                           "{" \
                           "  \"name\": \"My Digital8 Card\"," \
                           "  \"id\": 0," \
                           "  \"type\": \"59d33888-62c7-45b2-a4d4-9dbc55914ed3\"," \
                           "  \"typename\": \"Fxt::Card::HW::Mock::Digital8\"," \
                           "  \"slot\": 0," \
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
#define MAX_CARDS       2

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Database" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap statefulAllocator( statefulHeap_, sizeof( statefulHeap_ ) );
    Fxt::Point::Database<MAX_POINTS> pointDb;
    Fxt::Card::Database<MAX_CARDS>   uut;


    SECTION( "empty db" )
    {
        REQUIRE( uut.getMaxNumPoints() == MAX_CARDS );

        for ( unsigned i=0; i < MAX_CARDS; i++ )
        {
            REQUIRE( uut.lookupById( i ) == nullptr );
        }
    }


    SECTION( "add cards" )
    {

        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, CARD_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        REQUIRE( uut.lookupById( 0 ) == nullptr );

        JsonVariant cardObj = doc["cards"][0];
        Fxt::Card::Mock::Digital8* card1 = new Fxt::Card::Mock::Digital8( uut,
                                                                         generalAllocator,
                                                                         statefulAllocator,
                                                                         pointDb,
                                                                         0,
                                                                         cardObj );
        REQUIRE( card1 != nullptr );
        REQUIRE( uut.lookupById( 0 ) == card1 );
        REQUIRE( card1->getErrorCode() == FXT_CARD_ERR_NO_ERROR );

        Fxt::Card::Mock::Digital8* card2 = new Fxt::Card::Mock::Digital8( uut,
                                              generalAllocator,
                                              statefulAllocator,
                                              pointDb,
                                              0,
                                              cardObj );
        REQUIRE( card2 != nullptr );
        REQUIRE( uut.lookupById( 0 ) == card1 );
        REQUIRE( card2->getErrorCode() == FXT_CARD_ERR_CARD_INVALID_ID );

        uut.clearCards();
        REQUIRE( uut.lookupById( 0 ) == nullptr );
        card2 = new Fxt::Card::Mock::Digital8( uut,
                                              generalAllocator,
                                              statefulAllocator,
                                              pointDb,
                                              0,
                                              cardObj );
        REQUIRE( card2 != nullptr );
        REQUIRE( uut.lookupById( 0 ) == card2 );
        REQUIRE( card2->getErrorCode() == FXT_CARD_ERR_NO_ERROR );

        Fxt::Card::Mock::Digital8* card3 = new Fxt::Card::Mock::Digital8( uut,
                                                                          generalAllocator,
                                                                          statefulAllocator,
                                                                          pointDb,
                                                                          1,
                                                                          cardObj );
        REQUIRE( card3 != nullptr );
        REQUIRE( uut.lookupById( 1 ) == card3 );
        REQUIRE( card3->getErrorCode() == FXT_CARD_ERR_NO_ERROR );

        Fxt::Card::Mock::Digital8* card4 = new Fxt::Card::Mock::Digital8( uut,
                                                                          generalAllocator,
                                                                          statefulAllocator,
                                                                          pointDb,
                                                                          2,
                                                                          cardObj );
        REQUIRE( card4 != nullptr );
        REQUIRE( uut.lookupById( 2 ) == nullptr );
        REQUIRE( card4->getErrorCode() == FXT_CARD_ERR_CARD_INVALID_ID );
    }


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}