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
#include "Fxt/Component/Basic/Wire64Bool.h"
#include "Fxt/Point/Database.h"
#include "Fxt/Point/FactoryDatabase.h"
#include "Cpl/Memory/LeanHeap.h"
#include "Cpl/Math/real.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Component::Basic;

static const char* COMP_DEFINTION = R"literalString(
{
  "components": [
    {
      "name": "test",
      "type": "d2253d23-c1cd-428c-817e-32748f96c45a",
      "typeName": "Fxt::Component::Basic::Wire64Bool",
      "inputs": [
        {
          "name": "Signal A",
          "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0",
          "typeName": "Fxt::Point::Bool",
          "idRef": 0
        },
        {
          "name": "Signal B",
          "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0",
          "typeName": "Fxt::Point::Bool",
          "idRef": 1
        }
      ],
      "outputs": [
        {
          "name": "Signal AA",
          "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0",
          "typeName": "Fxt::Point::Bool",
          "idRef": 2
        },
        {
          "name": "Signal BB",
          "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0",
          "typeName": "Fxt::Point::Bool",
          "idRef": 3
        }
      ]
    }
  ]
}
)literalString";


static size_t generalHeap_[10000];
static size_t statefulHeap_[10000];

#define MAX_POINTS              4

#define POINT_ID__IN_SIGNAL_1   0
#define POINT_ID__IN_SIGNAL_2   1

#define POINT_ID__OUT_SIGNAL_1  2
#define POINT_ID__OUT_SIGNAL_2  3



////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Wire64Bool" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap statefulAllocator( statefulHeap_, sizeof( statefulHeap_ ) );
    Fxt::Point::Database<MAX_POINTS>                   pointDb;
    Fxt::Point::FactoryDatabase                        pointFactoryDb;
    Cpl::Text::FString<Fxt::Type::Error::MAX_TEXT_LEN> buf;

    SECTION( "create component" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, COMP_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant componentObj = doc["components"][0];
        Wire64Bool uut( componentObj,
                           generalAllocator,
                           statefulAllocator,
                           pointFactoryDb,
                           pointDb );

        REQUIRE( uut.getErrorCode() == Fxt::Type::Error::SUCCESS() );

        REQUIRE( strcmp( uut.getTypeGuid(), Wire64Bool::GUID_STRING ) == 0 );
        REQUIRE( strcmp( uut.getTypeName(), Wire64Bool::TYPE_NAME ) == 0 );

        REQUIRE( uut.isStarted() == false );
    }

    SECTION( "start component" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, COMP_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant componentObj = doc["components"][0];
        Wire64Bool uut( componentObj,
                           generalAllocator,
                           statefulAllocator,
                           pointFactoryDb,
                           pointDb );

        REQUIRE( uut.getErrorCode() == Fxt::Type::Error::SUCCESS() );

        uint64_t nowUsec = Cpl::System::ElapsedTime::milliseconds() * 1000;

        REQUIRE( uut.start( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( uut.isStarted() == true );
    }

    SECTION( "execute component - happy path" )
    {
        bool value1, value2;
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, COMP_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant componentObj = doc["components"][0];
        Wire64Bool uut( componentObj,
                           generalAllocator,
                           statefulAllocator,
                           pointFactoryDb,
                           pointDb );

        REQUIRE( uut.getErrorCode() == Fxt::Type::Error::SUCCESS() );

        Fxt::Point::Bool* ptIn1  = new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__IN_SIGNAL_1, statefulAllocator );
        Fxt::Point::Bool* ptIn2  = new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__IN_SIGNAL_2, statefulAllocator );
        Fxt::Point::Bool* ptOut1 = new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__OUT_SIGNAL_1, statefulAllocator );
        Fxt::Point::Bool* ptOut2 = new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__OUT_SIGNAL_2, statefulAllocator );

        Fxt::Type::Error errCode = uut.resolveReferences( pointDb );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("error Code=%s", Fxt::Type::Error::toText( errCode, buf )) );
        REQUIRE( errCode == Fxt::Type::Error::SUCCESS() );

        uint64_t nowUsec = Cpl::System::ElapsedTime::milliseconds() * 1000;
        REQUIRE( uut.start( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( uut.isStarted() == true );

        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut1->isNotValid() );
        REQUIRE( ptOut2->isNotValid() );

        ptIn1->write( false );
        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut1->read( value1 ) );
        REQUIRE( value1 == false );
        REQUIRE( ptOut2->isNotValid() );

        ptIn1->write( true );
        ptIn2->write( false );
        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut1->read( value1 ) );
        REQUIRE( value1 == true );
        REQUIRE( ptOut2->read( value2 ) );
        REQUIRE( value2 == false );


        ptIn1->setInvalid();
        ptIn2->write( true );
        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut1->isNotValid() );
        REQUIRE( ptOut2->read( value2 ) );
        REQUIRE( value2 == true );

        ptIn2->setInvalid();
        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut1->isNotValid() );
        REQUIRE( ptOut2->isNotValid() );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}