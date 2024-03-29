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
#include "Fxt/Component/Digital/And64Gate.h"
#include "Fxt/Component/Digital/And64GateFactory.h"
#include "Fxt/Component/FactoryDatabase.h"
#include "Fxt/Point/Database.h"
#include "Fxt/Point/FactoryDatabase.h"
#include "Cpl/Memory/LeanHeap.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Component::Digital;


#define COMP_DEFINTION     "{\"components\":[" \
                           "{" \
                           "  \"name\": \"AND Gate#1\"," \
                           "  \"type\": \"e62e395c-d27a-4821-bba9-aa1e6de42a05\"," \
                           "  \"typeName\": \"Fxt::Component::Digital::And64Gate\"," \
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

#define MAX_COMPONENTS  2

#define MAX_POINTS      5

#define POINT_ID__IN_SIGNAL_1   2
#define POINT_ID__IN_SIGNAL_2   0
#define POINT_ID__IN_SIGNAL_3   4

#define POINT_ID__OUT           3
#define POINT_ID__OUT_NEGATED   1



////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "And64GateFactory" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap               generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap               statefulAllocator( statefulHeap_, sizeof( statefulHeap_ ) );
    Fxt::Point::Database<MAX_POINTS>    pointDb;
    Fxt::Component::FactoryDatabase     componentFactoryDb;
    Fxt::Point::FactoryDatabase         pointFactoryDb;
    And64GateFactory                    uut( componentFactoryDb );
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

        REQUIRE( strcmp( component->getTypeName(), And64Gate::TYPE_NAME ) == 0 );
        REQUIRE( strcmp( component->getTypeGuid(), And64Gate::GUID_STRING ) == 0 );

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

        REQUIRE( strcmp( component->getTypeName(), And64Gate::TYPE_NAME ) == 0 );
        REQUIRE( strcmp( component->getTypeGuid(), And64Gate::GUID_STRING ) == 0 );

        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__IN_SIGNAL_1, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__IN_SIGNAL_2, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__IN_SIGNAL_3, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__OUT, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__OUT_NEGATED, statefulAllocator );
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
        
        REQUIRE( strcmp( component->getTypeName(), And64Gate::TYPE_NAME ) == 0 );
        REQUIRE( strcmp( component->getTypeGuid(), And64Gate::GUID_STRING ) == 0 );

        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__IN_SIGNAL_1, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__IN_SIGNAL_2, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__IN_SIGNAL_3, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__OUT, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__OUT_NEGATED, statefulAllocator );
        REQUIRE( component->resolveReferences( pointDb ) == Fxt::Type::Error::SUCCESS() );

        uut.destroy( *component );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}