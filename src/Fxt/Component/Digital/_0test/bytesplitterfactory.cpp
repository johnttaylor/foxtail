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
#include "Fxt/Component/Digital/ByteSplitter.h"
#include "Fxt/Component/Digital/ByteSplitterFactory.h"
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
                           "          \"bit\": 1," \
                           "          \"name\": \"/bit1\"," \
                           "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                           "          \"typeName\": \"Fxt::Point::Bool\"," \
                           "          \"idRef\": 1," \
                           "          \"negate\": true" \
                           "      }," \
                           "      {" \
                           "          \"bit\": 4," \
                           "          \"name\": \"bit4\"," \
                           "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                           "          \"typeName\": \"Fxt::Point::Bool\"," \
                           "          \"idRef\": 0" \
                           "      }," \
                           "      {" \
                           "          \"bit\": 4," \
                           "          \"name\": \"/bit4\"," \
                           "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                           "          \"typeName\": \"Fxt::Point::Bool\"," \
                           "          \"idRef\": 4," \
                           "          \"negate\": true" \
                           "      }," \
                           "      {" \
                           "          \"bit\": 5," \
                           "          \"name\": \"/bit5\"," \
                           "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                           "          \"typeName\": \"Fxt::Point::Bool\"," \
                           "          \"idRef\": 5," \
                           "          \"negate\": true" \
                           "      }" \
                          "    ]" \
                           "  }" \
                           "]}"

static size_t generalHeap_[10000];
static size_t statefulHeap_[10000];

#define MAX_COMPONENTS  2

#define MAX_POINTS              6

#define POINT_ID__IN_SIGNAL_1   2

#define POINT_ID__BIT1_OUT      3
#define POINT_ID__BIT1_NEGATED  1
#define POINT_ID__BIT4_OUT      0
#define POINT_ID__BIT4_NEGATED  4
#define POINT_ID__BIT5_NEGATED  5




////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "ByteSplitterFactory" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap                       generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap                       statefulAllocator( statefulHeap_, sizeof( statefulHeap_ ) );
    Fxt::Point::Database<MAX_POINTS>            pointDb;
    Fxt::Component::FactoryDatabase             componentFactoryDb;
    Fxt::Point::FactoryDatabase                 pointFactoryDb;
    ByteSplitterFactory                         uut( componentFactoryDb );
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

        REQUIRE( strcmp( component->getTypeName(), ByteSplitter::TYPE_NAME ) == 0 );
        REQUIRE( strcmp( component->getTypeGuid(), ByteSplitter::GUID_STRING ) == 0 );

        REQUIRE( component->resolveReferences( pointDb ) == Fxt::Component::fullErr( Fxt::Component::Err_T::UNRESOLVED_INPUT_REFRENCE ) );
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

        REQUIRE( strcmp( component->getTypeName(), ByteSplitter::TYPE_NAME ) == 0 );
        REQUIRE( strcmp( component->getTypeGuid(), ByteSplitter::GUID_STRING ) == 0 );

        new(std::nothrow) Fxt::Point::Uint8( pointDb, POINT_ID__IN_SIGNAL_1, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT1_OUT, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT1_NEGATED, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT4_OUT, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT4_NEGATED, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT5_NEGATED, statefulAllocator );
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
        REQUIRE( component );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("error Code=%s", Fxt::Type::Error::toText( componentErrorCode, buf )) );
        REQUIRE( componentErrorCode == Fxt::Type::Error::SUCCESS() );

        REQUIRE( strcmp( component->getTypeName(), ByteSplitter::TYPE_NAME ) == 0 );
        REQUIRE( strcmp( component->getTypeGuid(), ByteSplitter::GUID_STRING ) == 0 );

        new(std::nothrow) Fxt::Point::Uint8( pointDb, POINT_ID__IN_SIGNAL_1, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT1_OUT, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT1_NEGATED, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT4_OUT, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT4_NEGATED, statefulAllocator );
        new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT5_NEGATED, statefulAllocator );
        REQUIRE( component->resolveReferences( pointDb ) == Fxt::Type::Error::SUCCESS() );

        uut.destroy( *component );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}