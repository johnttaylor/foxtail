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
#include "Fxt/Point/Database.h"
#include "Fxt/Point/Int32.h"
#include "Fxt/Point/Setter.h"
#include "Cpl/System/Trace.h"
#include <string.h>
#include "Cpl/Memory/HPool.h"
#include "Cpl/Memory/LeanHeap.h"
#include <new>

#define SECT_   "_0test"

/// 
using namespace Fxt::Point;

#define MAX_POINTS      (2+1)


#define ORANGE_INIT_VAL 7

#define APPLE_ID        0
#define APPLE_LABEL     "APPLE"

#define ORANGE_ID       1
#define ORANGE_LABEL    "ORANGE"

#define SETTER_ID       1
#define SETTER_LABEL    "SETTER"

#define ELEM_SIZE_AS_SIZET(elemSize)    (((elemSize)+sizeof( int32_t ) - 1) / sizeof(int32_t))
#define MEM_SIZE_STATIC_POINT_DATA      128
static size_t stateHeapMemory_[ELEM_SIZE_AS_SIZET( sizeof( Int32::StateBlock_T ) ) * MAX_POINTS];

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Setter" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Database<MAX_POINTS>     db;
    Cpl::Memory::LeanHeap    stateHeap( stateHeapMemory_, sizeof( stateHeapMemory_ ) );
    bool                     valid;
    int32_t                  value;

    Int32* apple = new(std::nothrow) Int32( db, APPLE_ID, APPLE_LABEL, stateHeap );
    REQUIRE( apple );
    Int32* orange = new(std::nothrow) Int32( db, ORANGE_ID, ORANGE_LABEL, stateHeap, ORANGE_INIT_VAL );
    REQUIRE( orange );


    SECTION( "set" )
    {
        valid = orange->read( value );
        REQUIRE( valid == true );
        REQUIRE( value == ORANGE_INIT_VAL );

        valid = apple->read( value );
        REQUIRE( valid == false );

        Setter uut( orange );
        uut.setValue( *apple );
        valid = apple->read( value );
        REQUIRE( valid == true );
        REQUIRE( value == ORANGE_INIT_VAL );
    }

    SECTION( "create" )
    {
        valid = orange->read( value );
        REQUIRE( valid == true );
        REQUIRE( value == ORANGE_INIT_VAL );

        size_t                  heapMemory_[MEM_SIZE_STATIC_POINT_DATA];
        Cpl::Memory::LeanHeap   heapAllocator( heapMemory_, sizeof( heapMemory_ ) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, "{val:32}" );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["val"] == 32 );

        JsonObject valObject = doc.as<JsonObject>();
        Setter* uut = Setter::create( db, Int32::create, SETTER_ID, SETTER_LABEL, heapAllocator, stateHeap, valObject );
        REQUIRE( uut );
        uut->setValue( *orange );
        valid = orange->read( value );
        REQUIRE( valid == true );
        REQUIRE( value == 32 );
    }

    SECTION( "create#2" )
    {
        valid = orange->read( value );
        REQUIRE( valid == true );
        REQUIRE( value == ORANGE_INIT_VAL );

        size_t                  heapMemory_[MEM_SIZE_STATIC_POINT_DATA];
        Cpl::Memory::LeanHeap   heapAllocator( heapMemory_, sizeof( heapMemory_ ) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, "{valid:false,val:32}" );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["val"] == 32 );
        REQUIRE( doc["valid"] == false );
        JsonVariant jsonSrc = doc.as<JsonVariant>();
        Setter*     uut     = Setter::create( db, Int32::create, SETTER_ID, SETTER_LABEL, heapAllocator, stateHeap, jsonSrc );
        uut->setValue( *orange );
        valid = orange->read( value );
        REQUIRE( valid == false );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
