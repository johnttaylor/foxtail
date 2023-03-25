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
#include "Fxt/Component/Digital/Mux8Uint8Factory.h"
#include "Fxt/Component/FactoryDatabase.h"
#include "Fxt/Point/Database.h"
#include "Fxt/Point/FactoryDatabase.h"
#include "Fxt/Point/Int16.h"
#include "Cpl/Memory/LeanHeap.h"
#include "Cpl/Text/FString.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Component::Digital;


#define COMP_DEFINTION     "{\"components\":[" \
                           "{" \
                           "  \"name\": \"Mux #1\"," \
                           "  \"type\": \"d60f2daf-9709-42d6-ba92-b76f641eb930\"," \
                           "  \"typeName\": \"Fxt::Component::Digital::Mux8Uint8\"," \
                           "  \"inputs\": [" \
                           "      {" \
                           "          \"bit\": 4," \
                           "          \"name\": \"bit4\"," \
                           "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                           "          \"typeName\": \"Fxt::Point::Bool\"," \
                           "          \"idRef\": 0," \
                           "          \"negate\": false" \
                           "      }," \
                           "      {" \
                           "          \"bit\": 0," \
                           "          \"name\": \"/bit1\"," \
                           "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                           "          \"typeName\": \"Fxt::Point::Bool\"," \
                           "          \"idRef\": 1," \
                           "          \"negate\": true" \
                           "      }," \
                           "      {" \
                           "          \"bit\": 1," \
                           "          \"name\": \"bit1\"," \
                           "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                           "          \"typeName\": \"Fxt::Point::Bool\"," \
                           "          \"idRef\": 2" \
                           "      }" \
                           "    ]," \
                           "  \"outputs\": [" \
                           "      {" \
                           "          \"name\": \"output word\"," \
                           "          \"type\": \"918cff9e-8007-4666-99ac-384b9624329c\"," \
                           "          \"typeName\": \"Fxt::Point::Uint8\"," \
                           "          \"idRef\": 3" \
                           "      }" \
                          "    ]" \
                           "  }" \
                           "]}"

static size_t generalHeap_[10000];
static size_t statefulHeap_[10000];

#define MAX_POINTS              4

#define POINT_ID__OUT_SIGNAL    3

#define POINT_ID__BIT4_IN       0
#define POINT_ID__BIT0_IN       1
#define POINT_ID__BIT1_IN       2



////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Mux8Uint8Factory" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap                       generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap                       statefulAllocator( statefulHeap_, sizeof( statefulHeap_ ) );
    Fxt::Point::Database<MAX_POINTS>            pointDb;
    Fxt::Component::FactoryDatabase             componentFactoryDb;
    Fxt::Point::FactoryDatabase                 pointFactoryDb;
    Mux8Uint8Factory                            uut( componentFactoryDb );
    Fxt::Type::Error                            componentErrorCode;
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

        REQUIRE( strcmp( component->getTypeName(), Mux8Uint8::TYPE_NAME ) == 0 );
        REQUIRE( strcmp( component->getTypeGuid(), Mux8Uint8::GUID_STRING ) == 0 );

        REQUIRE( component->resolveReferences( pointDb ) == Fxt::Component::fullErr( Fxt::Component::Err_T::UNRESOLVED_INPUT_REFRENCE ) );
        uut.destroy( *component );
    }

    SECTION( "create - resolve references" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, COMP_DEFINTION );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("json error=%s", err.c_str()) );
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

        REQUIRE( strcmp( component->getTypeName(), Mux8Uint8::TYPE_NAME ) == 0 );
        REQUIRE( strcmp( component->getTypeGuid(), Mux8Uint8::GUID_STRING ) == 0 );

        new(std::nothrow) Fxt::Point::Uint8( pointDb, POINT_ID__OUT_SIGNAL, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT0_IN, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT1_IN, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT4_IN, statefulAllocator );

        componentErrorCode = component->resolveReferences( pointDb );
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
        CPL_SYSTEM_TRACE_MSG( SECT_, ("error Code=%s", Fxt::Type::Error::toText( componentErrorCode, buf )) );
        REQUIRE( component );
        REQUIRE( componentErrorCode == Fxt::Type::Error::SUCCESS() );

        REQUIRE( strcmp( component->getTypeName(), Mux8Uint8::TYPE_NAME ) == 0 );
        REQUIRE( strcmp( component->getTypeGuid(), Mux8Uint8::GUID_STRING ) == 0 );

        new(std::nothrow) Fxt::Point::Uint8( pointDb, POINT_ID__OUT_SIGNAL, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT0_IN, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT1_IN, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT4_IN, statefulAllocator );

        REQUIRE( component->resolveReferences( pointDb ) == Fxt::Type::Error::SUCCESS() );

        uut.destroy( *component );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}