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
#include "Fxt/Component/Basic/Wire64Float.h"
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
      "type": "e6759a22-06c1-4aad-8190-67bf36425903",
      "typeName": "Fxt::Component::Basic::Wire64Float",
      "inputs": [
        {
          "name": "Signal A",
          "type": "708745fa-cef6-4364-abad-063a40f35cbc",
          "typeName": "Fxt::Point::Float",
          "idRef": 0
        },
        {
          "name": "Signal B",
          "type": "708745fa-cef6-4364-abad-063a40f35cbc",
          "typeName": "Fxt::Point::Float",
          "idRef": 1
        }
      ],
      "outputs": [
        {
          "name": "Signal AA",
          "type": "708745fa-cef6-4364-abad-063a40f35cbc",
          "typeName": "Fxt::Point::Bool",
          "idRef": 2
        },
        {
          "name": "Signal BB",
          "type": "708745fa-cef6-4364-abad-063a40f35cbc",
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
TEST_CASE( "Wire64Float" )
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
        Wire64Float uut( componentObj,
                           generalAllocator,
                           statefulAllocator,
                           pointFactoryDb,
                           pointDb );

        REQUIRE( uut.getErrorCode() == Fxt::Type::Error::SUCCESS() );

        REQUIRE( strcmp( uut.getTypeGuid(), Wire64Float::GUID_STRING ) == 0 );
        REQUIRE( strcmp( uut.getTypeName(), Wire64Float::TYPE_NAME ) == 0 );

        REQUIRE( uut.isStarted() == false );
    }

    SECTION( "start component" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, COMP_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant componentObj = doc["components"][0];
        Wire64Float uut( componentObj,
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
        float value1, value2;
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, COMP_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant componentObj = doc["components"][0];
        Wire64Float uut( componentObj,
                           generalAllocator,
                           statefulAllocator,
                           pointFactoryDb,
                           pointDb );

        REQUIRE( uut.getErrorCode() == Fxt::Type::Error::SUCCESS() );

        Fxt::Point::Float* ptIn1  = new(std::nothrow) Fxt::Point::Float( pointDb, POINT_ID__IN_SIGNAL_1, statefulAllocator );
        Fxt::Point::Float* ptIn2  = new(std::nothrow) Fxt::Point::Float( pointDb, POINT_ID__IN_SIGNAL_2, statefulAllocator );
        Fxt::Point::Float* ptOut1 = new(std::nothrow) Fxt::Point::Float( pointDb, POINT_ID__OUT_SIGNAL_1, statefulAllocator );
        Fxt::Point::Float* ptOut2 = new(std::nothrow) Fxt::Point::Float( pointDb, POINT_ID__OUT_SIGNAL_2, statefulAllocator );

        Fxt::Type::Error errCode = uut.resolveReferences( pointDb );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("error Code=%s", Fxt::Type::Error::toText( errCode, buf )) );
        REQUIRE( errCode == Fxt::Type::Error::SUCCESS() );

        uint64_t nowUsec = Cpl::System::ElapsedTime::milliseconds() * 1000;
        REQUIRE( uut.start( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( uut.isStarted() == true );

        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut1->isNotValid() );
        REQUIRE( ptOut2->isNotValid() );

        ptIn1->write( 0.0F );
        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut1->read( value1 ) );
        REQUIRE( Cpl::Math::areFloatsEqual( value1, 0.0F ) );
        REQUIRE( ptOut2->isNotValid() );

        ptIn1->write( 1.1F );
        ptIn2->write( 2.2F );
        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut1->read( value1 ) );
        REQUIRE( Cpl::Math::areFloatsEqual( value1, 1.1F ) );
        REQUIRE( ptOut2->read( value2 ) );
        REQUIRE( Cpl::Math::areFloatsEqual( value2, 2.2F) );


        ptIn1->setInvalid();
        ptIn2->write( -1.0F );
        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut1->isNotValid() );
        REQUIRE( ptOut2->read( value2 ) );
        REQUIRE( Cpl::Math::areFloatsEqual( value2, -1.0F ) );

        ptIn2->setInvalid();
        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut1->isNotValid() );
        REQUIRE( ptOut2->isNotValid() );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}