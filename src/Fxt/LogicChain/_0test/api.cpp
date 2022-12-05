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
#include "Fxt/LogicChain/Chain.h"
#include "Fxt/Point/Database.h"
#include "Fxt/Point/Uint8.h"
#include "Fxt/Component/Digital/And16GateFactory.h
#include "Fxt/Component/Digital/ByteSplitterFactory.h
#include "Cpl/Memory/LeanHeap.h"
#include "Cpl/System/Trace.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::LogicChain;


#define LC_DEFINTION        "{\"name\":\"my logic chain\", \
                            " \"id\":1 \
                            " \"components\":[" \
                            "  {" \
                            "  \"name\": \"AND Gate#1\"," \
                            "  \"type\": \"e62e395c-d27a-4821-bba9-aa1e6de42a05\"," \
                            "  \"typeName\": \"Fxt::Component::Digital::And16Gate\"," \
                            "  \"inputs\": [" \
                            "      {" \
                            "          \"name\": \"Signal#1\"," \
                            "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                            "          \"typeName\": \"Fxt::Point::Bool\"," \
                            "          \"idRef\": 2" \
                            "      }," \
                            "      {" \
                            "          \"name\": \"Signal#2\"," \
                            "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                            "          \"typeName\": \"Fxt::Point::Bool\"," \
                            "          \"idRef\": 0" \
                            "      }" \
                            "    ]," \
                            "  \"outputs\": [" \
                            "      {" \
                            "          \"name\": \"out\"," \
                            "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                            "          \"typeName\": \"Fxt::Point::Bool\"," \
                            "          \"idRef\": 3" \
                            "      }" \
                            "    ]" \
                            "  }," \
                            "  {" \
                            "  \"name\": \"ByteSplitter #1\"," \
                            "  \"type\": \"8c55aa52-3bc8-4b8a-ad73-c434a0bbd4b4\"," \
                            "  \"typeName\": \"Fxt::Component::Digital::ByteSplitter\"," \
                            "  \"inputs\": [" \
                            "      {" \
                            "          \"name\": \"input byte\"," \
                            "          \"type\": \"918cff9e-8007-4666-99ac-384b9624329c\"," \
                            "          \"typeName\": \"Fxt::Point::Uint8\"," \
                            "          \"idRef\": 2" \
                            "      }" \
                            "    ]," \
                            "  \"outputs\": [" \
                            "      {" \
                            "          \"bit\": 1," \
                            "          \"name\": \"bit1\"," \
                            "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                            "          \"typeName\": \"Fxt::Point::Bool\"," \
                            "          \"idRef\": 3" \
                            "      }," \
                            "      {" \
                            "          \"bit\": 4," \
                            "          \"name\": \"/bit4\"," \
                            "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                            "          \"typeName\": \"Fxt::Point::Bool\"," \
                            "          \"idRef\": 4," \
                            "          \"negate\": true" \
                            "      }" \
                            "    ]" \
                            "  }" \
                            "]}"

static size_t generalHeap_[10000];
static size_t statefulHeap_[10000];

#define MAX_POINTS      100
#define MAX_CARDS       2

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "LogicChain" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap statefulAllocator( statefulHeap_, sizeof( statefulHeap_ ) );
    Fxt::Point::Database<MAX_POINTS> pointDb;


    SECTION( "create" )
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
        REQUIRE( card1->getErrorCode() == Fxt::Type::Error::SUCCESS()  );

        Fxt::Card::Mock::Digital8* card2 = new Fxt::Card::Mock::Digital8( uut,
                                              generalAllocator,
                                              statefulAllocator,
                                              pointDb,
                                              0,
                                              cardObj );
        REQUIRE( card2 != nullptr );
        REQUIRE( uut.lookupById( 0 ) == card1 );
        REQUIRE( card2->getErrorCode() == Fxt::Type::Error( Fxt::Type::Err_T::CARD, Err_T::CARD_INVALID_ID ) );

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
        REQUIRE( card2->getErrorCode() == Fxt::Type::Error::SUCCESS()  );

        Fxt::Card::Mock::Digital8* card3 = new Fxt::Card::Mock::Digital8( uut,
                                                                          generalAllocator,
                                                                          statefulAllocator,
                                                                          pointDb,
                                                                          1,
                                                                          cardObj );
        REQUIRE( card3 != nullptr );
        REQUIRE( uut.lookupById( 1 ) == card3 );
        REQUIRE( card3->getErrorCode() == Fxt::Type::Error::SUCCESS()  );

        Fxt::Card::Mock::Digital8* card4 = new Fxt::Card::Mock::Digital8( uut,
                                                                          generalAllocator,
                                                                          statefulAllocator,
                                                                          pointDb,
                                                                          2,
                                                                          cardObj );
        REQUIRE( card4 != nullptr );
        REQUIRE( uut.lookupById( 2 ) == nullptr );
        REQUIRE( card4->getErrorCode() == Fxt::Type::Error( Fxt::Type::Err_T::CARD, Err_T::CARD_INVALID_ID ) );
    }


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}