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
#include "Fxt/Point/Uint32.h"
#include "Fxt/Point/String.h"
#include "Fxt/Point/FactoryDatabase.h"
#include "Fxt/Point/Factory.h"
#include "Fxt/Point/Database.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Memory/LeanHeap.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Point;

#define MAX_POINTS          3

static size_t stateHeapMemory_[100];
static size_t generalHeapMemory_[200];

#define JSON_UINT32_NO_VAL  "{" \
                            "  id:0,"\
                            "  type:\"99f91433-b8e4-4480-9203-8fcab5d9f20f\""\
                            "}"

#define JSON_UINT32_VAL     "{" \
                            "  id:1,"\
                            "  type:\"99f91433-b8e4-4480-9203-8fcab5d9f20f\"," \
                            "  initial: {" \
                            "    val:42, "\
                            "    id:2" \
                            "  }"\
                            "}"

#define JSON_STRING_NO_VAL  "{" \
                            "  id:0,"\
                            "  type:\"99f91433-b8e4-4480-9203-8fcab5d9f20f\","\
                            "  typeCfg: {"\
                            "    numElems:17" \
                            "  }"\
                            "}"

#define JSON_STRING_VAL     "{" \
                            "  id:1,"\
                            "  type:\"e2c8172c-fca3-4023-bb9f-79f92dec7c34\"," \
                            "  typeCfg: {"\
                            "    numElems:17" \
                            "  },"\
                            "  initial: {" \
                            "    val: {" \
                            "      text:\"bob's your uncle!\" "\
                            "    }," \
                            "    id:2" \
                            "  }"\
                            "}"

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Factory" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Database<MAX_POINTS>     db;
    Cpl::Memory::LeanHeap    stateHeap( stateHeapMemory_, sizeof( stateHeapMemory_ ) );
    Cpl::Memory::LeanHeap    generalHeap( generalHeapMemory_, sizeof( generalHeapMemory_ ) );
    FactoryDatabase          uutFactoryDb;
    Factory<Uint32>          uutFactoryUint32( uutFactoryDb );
    Factory1DArray<String>   uutFactoryString( uutFactoryDb );
    Fxt::Type::Error         err;
    bool                     valid;
    uint32_t                 value;
    Cpl::Text::FString<64>   bufString;

    SECTION( "db" )
    {
        FactoryApi* factory = uutFactoryDb.first();
        REQUIRE( factory == &uutFactoryUint32 );
        factory = uutFactoryDb.next( *factory );
        REQUIRE( factory == &uutFactoryString );
        factory = uutFactoryDb.next( *factory );
        REQUIRE( factory == nullptr );

        factory = uutFactoryDb.lookup( String::GUID_STRING );
        REQUIRE( factory == &uutFactoryString );
        factory = uutFactoryDb.lookup( Uint32::GUID_STRING );
        REQUIRE( factory == &uutFactoryUint32 );
        factory = uutFactoryDb.lookup( "bob" );
        REQUIRE( factory == nullptr );
    }

    SECTION( "factory create - uint32" )
    {
        StaticJsonDocument<1024> doc;
        DeserializationError errJson = deserializeJson( doc, JSON_UINT32_NO_VAL );
        REQUIRE( errJson == DeserializationError::Ok );
        JsonObject  jsonObj = doc.as<JsonObject>();

        Uint32* pointU32 = uutFactoryUint32.createTypeSafe( jsonObj, err, generalHeap, stateHeap, db );
        REQUIRE( pointU32 != nullptr );
        REQUIRE( pointU32->isNotValid() );


        doc.clear();
        errJson = deserializeJson( doc, JSON_UINT32_VAL );
        REQUIRE( errJson == DeserializationError::Ok );
        jsonObj = doc.as<JsonObject>();

        pointU32 = uutFactoryUint32.createTypeSafe( jsonObj, err, generalHeap, stateHeap, db );
        REQUIRE( pointU32 != nullptr );
        valid = pointU32->read( value );
        REQUIRE( valid );
        REQUIRE( value == 42 );
    }

    SECTION( "factory create - String" )
    {
        StaticJsonDocument<1024> doc;
        DeserializationError errJson = deserializeJson( doc, JSON_STRING_NO_VAL );
        REQUIRE( errJson == DeserializationError::Ok );
        JsonObject  jsonObj = doc.as<JsonObject>();

        String* pointString = uutFactoryString.createTypeSafe( jsonObj, err, generalHeap, stateHeap, db );
        REQUIRE( pointString != nullptr );
        REQUIRE( pointString->isNotValid() );
        REQUIRE( pointString->getMaxLength() == 17 );


        doc.clear();
        errJson = deserializeJson( doc, JSON_STRING_VAL );
        REQUIRE( errJson == DeserializationError::Ok );
        jsonObj = doc.as<JsonObject>();

        pointString = uutFactoryString.createTypeSafe( jsonObj, err, generalHeap, stateHeap, db );
        REQUIRE( pointString != nullptr );
        REQUIRE( pointString->getMaxLength() == 17 );
        valid = pointString->read( bufString );
        REQUIRE( valid );
        REQUIRE( bufString == "bob's your uncle!" );
    }

    SECTION( "db create - uint32" )
    {
        StaticJsonDocument<1024> doc;
        DeserializationError errJson = deserializeJson( doc, JSON_UINT32_NO_VAL );
        REQUIRE( errJson == DeserializationError::Ok );
        JsonObject  jsonObj = doc.as<JsonObject>();

        Api* pointPtr = uutFactoryDb.createPointfromJSON( jsonObj, err, generalHeap, stateHeap, db );
        REQUIRE( pointPtr != nullptr );
        REQUIRE( pointPtr->isNotValid() );


        doc.clear();
        errJson = deserializeJson( doc, JSON_UINT32_VAL );
        REQUIRE( errJson == DeserializationError::Ok );
        jsonObj = doc.as<JsonObject>();

        pointPtr = uutFactoryDb.createPointfromJSON( jsonObj, err, generalHeap, stateHeap, db );
        Uint32* u32Ptr = (Uint32*) pointPtr;
        REQUIRE( u32Ptr != nullptr );
        valid = u32Ptr->read( value );
        REQUIRE( valid );
        REQUIRE( value == 42 );
    }

    SECTION( "db create - String" )
    {
        StaticJsonDocument<1024> doc;
        DeserializationError errJson = deserializeJson( doc, JSON_STRING_NO_VAL );
        REQUIRE( errJson == DeserializationError::Ok );
        JsonObject  jsonObj = doc.as<JsonObject>();

        Api* pointPtr = uutFactoryString.createTypeSafe( jsonObj, err, generalHeap, stateHeap, db );
        REQUIRE( pointPtr != nullptr );
        REQUIRE( pointPtr->isNotValid() );
        String* pointString = (String*) pointPtr;
        REQUIRE( pointString->getMaxLength() == 17 );


        doc.clear();
        errJson = deserializeJson( doc, JSON_STRING_VAL );
        REQUIRE( errJson == DeserializationError::Ok );
        jsonObj = doc.as<JsonObject>();

        pointString = (String*) uutFactoryString.createTypeSafe( jsonObj, err, generalHeap, stateHeap, db );
        REQUIRE( pointString != nullptr );
        REQUIRE( pointString->getMaxLength() == 17 );
        valid = pointString->read( bufString );
        REQUIRE( valid );
        REQUIRE( bufString == "bob's your uncle!" );
    }

    SECTION( "factory create - no setter - uint32" )
    {
        StaticJsonDocument<1024> doc;
        DeserializationError errJson = deserializeJson( doc, JSON_UINT32_VAL );
        REQUIRE( errJson == DeserializationError::Ok );
        JsonObject  jsonObj = doc.as<JsonObject>();

        Uint32* pointU32 = uutFactoryUint32.createTypeSafe( jsonObj, err, generalHeap, stateHeap, db, "id", false );
        REQUIRE( pointU32 != nullptr );
        REQUIRE( pointU32->isNotValid() );
    }

    SECTION( "factory create - no setter - string" )
    {
        StaticJsonDocument<1024> doc;
        DeserializationError errJson = deserializeJson( doc, JSON_STRING_VAL );
        REQUIRE( errJson == DeserializationError::Ok );
        JsonObject  jsonObj = doc.as<JsonObject>();

        String* pointString = (String*) uutFactoryString.createTypeSafe( jsonObj, err, generalHeap, stateHeap, db, "id", false );
        REQUIRE( pointString != nullptr );
        REQUIRE( pointString->getMaxLength() == 17 );
        REQUIRE( pointString->isNotValid() );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
