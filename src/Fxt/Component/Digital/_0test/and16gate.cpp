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
#include "Fxt/Component/Digital/And16Gate.h"
#include "Fxt/Component/Digital/Error.h"
#include "Fxt/Point/Database.h"
#include "Fxt/Point/FactoryDatabase.h"
#include "Fxt/Point/Bank.h"
#include "Cpl/Memory/LeanHeap.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Component::Digital;


#define COMP_DEFINTION     "{\"components\":[" \
                           "{" \
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
                           "      }," \
                           "      {" \
                           "          \"name\": \"Signal#3\"," \
                           "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                           "          \"typeName\": \"Fxt::Point::Bool\"," \
                           "          \"idRef\": 4" \
                           "      }" \
                           "    ]," \
                           "  \"outputs\": [" \
                           "      {" \
                           "          \"name\": \"out\"," \
                           "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                           "          \"typeName\": \"Fxt::Point::Bool\"," \
                           "          \"idRef\": 3" \
                           "      }," \
                           "      {" \
                           "          \"name\": \"/out\"," \
                           "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                           "          \"typeName\": \"Fxt::Point::Bool\"," \
                           "          \"idRef\": 1," \
                           "          \"negate\": true" \
                           "      }" \
                           "    ]" \
                           "  }" \
                           "]}"

static size_t generalHeap_[10000];
static size_t statefulHeap_[10000];

#define MAX_POINTS      5

#define POINT_ID__IN_SIGNAL_1   2
#define POINT_ID__IN_SIGNAL_2   0
#define POINT_ID__IN_SIGNAL_3   4

#define POINT_ID__OUT           3
#define POINT_ID__OUT_NEGATED   1



////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "And16Gate" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap statefulAllocator( statefulHeap_, sizeof( statefulHeap_ ) );
    Fxt::Point::Database<MAX_POINTS> pointDb;
    Fxt::Point::Bank                 pointBank;
    Fxt::Point::FactoryDatabase      pointFactoryDb;

    SECTION( "create component" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, COMP_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant componentObj = doc["components"][0];
        And16Gate uut( componentObj,
                       generalAllocator,
                       statefulAllocator,
                       pointBank,
                       pointFactoryDb,
                       pointDb );

        REQUIRE( uut.getErrorCode() == Fxt::Type::Error::SUCCESS() );

        REQUIRE( strcmp( uut.getTypeGuid(), And16Gate::GUID_STRING ) == 0 );
        REQUIRE( strcmp( uut.getTypeName(), And16Gate::TYPE_NAME ) == 0 );

        REQUIRE( uut.isStarted() == false );
    }

    SECTION( "start component" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, COMP_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant componentObj = doc["components"][0];
        And16Gate uut( componentObj,
                       generalAllocator,
                       statefulAllocator,
                       pointBank,
                       pointFactoryDb,
                       pointDb );

        REQUIRE( uut.getErrorCode() == Fxt::Type::Error::SUCCESS() );

        uint64_t nowUsec = Cpl::System::ElapsedTime::milliseconds() * 1000;

        REQUIRE( uut.start( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( uut.isStarted() == true );
    }

    SECTION( "execute component - happy path" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, COMP_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant componentObj = doc["components"][0];
        And16Gate uut( componentObj,
                       generalAllocator,
                       statefulAllocator,
                       pointBank,
                       pointFactoryDb,
                       pointDb );

        REQUIRE( uut.getErrorCode() == Fxt::Type::Error::SUCCESS() );

        Fxt::Point::Bool* ptIn1  = new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__IN_SIGNAL_1, statefulAllocator );
        Fxt::Point::Bool* ptIn2  = new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__IN_SIGNAL_2, statefulAllocator );
        Fxt::Point::Bool* ptIn3  = new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__IN_SIGNAL_3, statefulAllocator );
        Fxt::Point::Bool* ptOut1 = new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__OUT, statefulAllocator );
        Fxt::Point::Bool* ptOut2 = new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__OUT_NEGATED, statefulAllocator );
        REQUIRE( uut.resolveReferences( pointDb ) == Fxt::Type::Error::SUCCESS() );

        uint64_t nowUsec = Cpl::System::ElapsedTime::milliseconds() * 1000;
        REQUIRE( uut.start( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( uut.isStarted() == true );

        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut1->isNotValid() );
        REQUIRE( ptOut2->isNotValid() );

        ptIn1->write( true );
        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut1->isNotValid() );
        REQUIRE( ptOut2->isNotValid() );

        ptIn2->write( false );
        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut1->isNotValid() );
        REQUIRE( ptOut2->isNotValid() );

        ptIn3->write( false );
        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        bool val;
        REQUIRE( ptOut1->read( val ) );
        REQUIRE( val == false );
        REQUIRE( ptOut2->read( val ) );
        REQUIRE( val == true );

        ptIn3->write( true );
        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut1->read( val ) );
        REQUIRE( val == false );
        REQUIRE( ptOut2->read( val ) );
        REQUIRE( val == true );

        ptIn2->write( true );
        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut1->read( val ) );
        REQUIRE( val == true );
        REQUIRE( ptOut2->read( val ) );
        REQUIRE( val == false );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}