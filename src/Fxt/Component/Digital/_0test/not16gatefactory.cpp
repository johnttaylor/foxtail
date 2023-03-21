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
#include "Fxt/Component/Digital/Error.h"
#include "Fxt/Component/Digital/Not16Gate.h"
#include "Fxt/Component/Digital/Not16GateFactory.h"
#include "Fxt/Component/FactoryDatabase.h"
#include "Fxt/Point/Database.h"
#include "Fxt/Point/FactoryDatabase.h"
#include "Cpl/Memory/LeanHeap.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Component::Digital;


static const char* COMP_DEFINTION = R"literalString(
{
  "components": [
    {
      "name": "NOT16 Gate",
      "type": "1d8a613-bc99-4d0d-a96f-4b4dc9b0cc6f",
      "typeName": "Fxt::Component::Digital::Not16Gate",
      "inputs": [
        {
          "name": "IN Signal A",
          "type": "708745fa-cef6-4364-abad-063a40f35cbc",
          "typeName": "Fxt::Point::Bool",
          "idRef": 0
        },
        {
          "name": "IN Signal B",
          "type": "708745fa-cef6-4364-abad-063a40f35cbc",
          "typeName": "Fxt::Point::Bool",
          "idRef": 1
        }
      ],
      "outputs": [
        {
          "name": "OUT /Signal A",
          "type": "708745fa-cef6-4364-abad-063a40f35cbc",
          "typeName": "Fxt::Point::Bool",
          "idRef": 2
        },
        {
          "name": "OUT Signal B",
          "type": "708745fa-cef6-4364-abad-063a40f35cbc",
          "typeName": "Fxt::Point::Bool",
          "negate": true,
          "idRef": 3
        }
      ]
    }
  ]
}
)literalString";




static size_t generalHeap_[10000];
static size_t statefulHeap_[10000];

#define MAX_COMPONENTS  1

#define MAX_POINTS      4

#define POINT_ID__IN_SIGNAL_1   0
#define POINT_ID__IN_SIGNAL_2   1

#define POINT_ID__OUT_SIGNAL_1  2
#define POINT_ID__OUT_SIGNAL_2  3



////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Not16GateFactory" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap               generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap               statefulAllocator( statefulHeap_, sizeof( statefulHeap_ ) );
    Fxt::Point::Database<MAX_POINTS>    pointDb;
    Fxt::Component::FactoryDatabase     componentFactoryDb;
    Fxt::Point::FactoryDatabase         pointFactoryDb;
    Not16GateFactory                    uut( componentFactoryDb );
    Fxt::Type::Error                    componentErrorCode;
    Cpl::Text::FString<Fxt::Type::Error::MAX_TEXT_LEN> buf;

    SECTION( "create/destroy card" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, COMP_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant componentObj = doc["components"][0];
        Fxt::Component::Api* component = uut.create( componentObj,
                                                     componentErrorCode,
                                                     generalAllocator,
                                                     statefulAllocator,
                                                     pointFactoryDb,
                                                     pointDb );
        REQUIRE( component != nullptr );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("error Code=%s", Fxt::Type::Error::toText( componentErrorCode, buf )) );
        REQUIRE( componentErrorCode == Fxt::Type::Error::SUCCESS() );

        REQUIRE( strcmp( component->getTypeName(), Not16Gate::TYPE_NAME ) == 0 );
        REQUIRE( strcmp( component->getTypeGuid(), Not16Gate::GUID_STRING ) == 0 );

        REQUIRE( component->resolveReferences( pointDb ) == Fxt::Component::fullErr( Fxt::Component::Err_T::UNRESOLVED_INPUT_REFRENCE) );

        uut.destroy( *component );
    }

    SECTION( "create - resolve references" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, COMP_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant componentObj = doc["components"][0];
        Fxt::Component::Api* component = uut.create( componentObj,
                                                     componentErrorCode,
                                                     generalAllocator,
                                                     statefulAllocator,
                                                     pointFactoryDb,
                                                     pointDb );
        REQUIRE( component != nullptr );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("error Code=%s", Fxt::Type::Error::toText( componentErrorCode, buf )) );
        REQUIRE( componentErrorCode == Fxt::Type::Error::SUCCESS() );

        REQUIRE( strcmp( component->getTypeName(), Not16Gate::TYPE_NAME ) == 0 );
        REQUIRE( strcmp( component->getTypeGuid(), Not16Gate::GUID_STRING ) == 0 );

        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__IN_SIGNAL_1, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__IN_SIGNAL_2, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__OUT_SIGNAL_1, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__OUT_SIGNAL_2, statefulAllocator );
        componentErrorCode = component->resolveReferences( pointDb );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("error Code=%s", Fxt::Type::Error::toText( componentErrorCode, buf )) );

        REQUIRE( componentErrorCode == Fxt::Type::Error::SUCCESS() );

        uut.destroy( *component );
    }
    SECTION( "create from factory db" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, COMP_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant componentObj = doc["components"][0];
        Fxt::Component::Api* component = componentFactoryDb.createComponentfromJSON( componentObj,
                                                                                     generalAllocator,
                                                                                     statefulAllocator,
                                                                                     pointFactoryDb,
                                                                                     pointDb,
                                                                                     componentErrorCode );
        REQUIRE( component  );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("error Code=%s", Fxt::Type::Error::toText( componentErrorCode, buf )) );
        REQUIRE( componentErrorCode == Fxt::Type::Error::SUCCESS() );
        
        REQUIRE( strcmp( component->getTypeName(), Not16Gate::TYPE_NAME ) == 0 );
        REQUIRE( strcmp( component->getTypeGuid(), Not16Gate::GUID_STRING ) == 0 );

        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__IN_SIGNAL_1, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__IN_SIGNAL_2, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__OUT_SIGNAL_1, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__OUT_SIGNAL_2, statefulAllocator );
        REQUIRE( component->resolveReferences( pointDb ) == Fxt::Type::Error::SUCCESS() );

        uut.destroy( *component );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}