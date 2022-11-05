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
#include "Fxt/Point/Float.h"
#include "Cpl/System/Trace.h"
#include <string.h>
#include "Cpl/Memory/HPool.h"
#include "Cpl/Memory/LeanHeap.h"
#include "Cpl/Math/real.h"
#include <new>

#define SECT_   "_0test"

/// 
using namespace Fxt::Point;

#define MAX_POINTS  2


#define ORANGE_INIT_VAL 7.5

#define APPLE_ID        0
#define APPLE_LABEL     "APPLE"

#define ORANGE_ID       1
#define ORANGE_LABEL    "ORANGE"

#define ELEM_SIZE_AS_SIZET(elemSize)    (((elemSize)+sizeof( size_t ) - 1) / sizeof(size_t))
static size_t stateHeapMemory_[ELEM_SIZE_AS_SIZET( sizeof( Float::StateBlock_T ) ) * MAX_POINTS];

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Float" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Database<MAX_POINTS>     db;
    Cpl::Memory::LeanHeap    stateHeap( stateHeapMemory_, sizeof( stateHeapMemory_ ) );
    bool                     valid;
    float                    value;

    Float* apple = new(std::nothrow) Float( db, APPLE_ID, APPLE_LABEL, stateHeap );
    REQUIRE( apple );
    Float* orange = new(std::nothrow) Float( db, ORANGE_ID, ORANGE_LABEL, stateHeap, ORANGE_INIT_VAL );
    REQUIRE( orange );


    SECTION( "read" )
    {
        valid = orange->read( value );
        REQUIRE( valid == true );
        REQUIRE( value == ORANGE_INIT_VAL );

        valid = apple->read( value );
        REQUIRE( valid == false );
        apple->write( 1.1F );
        valid = apple->read( value );
        REQUIRE( valid );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 1.1F ) );

        apple->setInvalid();
        valid = apple->read( value );
        REQUIRE( valid == false );

        REQUIRE( apple->getStatefulMemorySize() >= (sizeof( float ) + sizeof( bool ) * 2) );
    }

    SECTION( "write" )
    {
        apple->write( 2.0F );
        REQUIRE( apple->read( value ) );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 2.0F ) );

        apple->increment();
        REQUIRE( apple->read( value ) );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 3.0F ) );

        apple->increment( 3.0F );
        REQUIRE( apple->read( value ) );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 3.0F + 3.0F ) );

        apple->decrement();
        REQUIRE( apple->read( value ) );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 3.0F + 3.0F - 1.0F ) );

        apple->decrement( 3.0F );
        REQUIRE( apple->read( value ) );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 2.0F ) );
    }

    SECTION( "write2" )
    {
        apple->write( *orange, Api::eLOCK );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == ORANGE_INIT_VAL );
        REQUIRE( apple->isLocked() );

        apple->write( *orange );
        REQUIRE( apple->isLocked() );
        apple->write( *orange, Api::eUNLOCK );
        REQUIRE( apple->isLocked() == false );
        orange->setInvalid();
        apple->write( *orange );
        REQUIRE( apple->isNotValid() );
    }


    SECTION( "json" )
    {
        char buffer[256];
        bool truncated;
        Cpl::Text::FString<100> errMsg;
        apple->write( 127.3F );

        bool result = db.toJSON( APPLE_ID, buffer, sizeof( buffer ), truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( result );
        REQUIRE( truncated == false );
        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, buffer );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( Cpl::Math::areFloatsEqual( doc["val"], 127.3F ) );

        orange->setInvalid();
        result = db.toJSON( ORANGE_ID, buffer, sizeof( buffer ), truncated, false );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( result );
        REQUIRE( truncated == false );

        result = db.fromJSON( "{\"id\":0,\"val\":3.14}" );
        REQUIRE( apple->read( value ) );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 3.14F ) );


        result = db.fromJSON( "{\"id\":0,\"val\":21,locked:true}" );
        REQUIRE( result );
        REQUIRE( apple->read( value ) );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 21.0F ) );
        REQUIRE( apple->isLocked() );

        result = db.fromJSON( "{\"id\":0,\"val\":22,locked:false}" );
        REQUIRE( result );
        REQUIRE( apple->read( value ) );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 22.0F ) );
        REQUIRE( !apple->isLocked() );

        result = db.fromJSON( "{\"id\":0,locked:true}" );
        REQUIRE( result );
        REQUIRE( apple->read( value ) );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 22.0F ) );
        REQUIRE( apple->isLocked() );

        result = db.fromJSON( "{\"id\":0,locked:false,valid:false}" );
        REQUIRE( result );
        REQUIRE( apple->isNotValid() );
        REQUIRE( !apple->isLocked() );

        // ERROR
        result = db.fromJSON( "{\"id\":100,\"val\":\"0x20\",locked:true}" );
        REQUIRE( result == false );

        // ERROR
        errMsg = "NOERROR";
        result = db.fromJSON( "{\"id\":100,\"val\":\"0x20\",locked:true}", &errMsg );
        REQUIRE( result == false );
        REQUIRE( errMsg != "NOERROR" );

        // ERROR
        errMsg = "NOERROR";
        result = db.fromJSON( "{\"id\":0,\"val\":true,locked:true}", &errMsg );
        REQUIRE( result == false );
        REQUIRE( errMsg != "NOERROR" );

        // ERROR
        result = db.fromJSON( "{\"id\":0,\"val\":false,locked:true}" );
        REQUIRE( result == false );

        // ERROR
        result = db.toJSON( ORANGE_ID + 2, buffer, sizeof( buffer ), truncated, false );
        REQUIRE( result == false );

        // ERROR
        errMsg = "NOERROR";
        result = db.fromJSON( "{\"id\"=100,\"val\":3.14,locked:true}", &errMsg );
        REQUIRE( result == false );
        REQUIRE( errMsg != "NOERROR" );

        // ERROR
        result = db.fromJSON( "{\"id\"=100,\"val\":3.14,locked:true}" );
        REQUIRE( result == false );

        // ERROR
        errMsg = "NOERROR";
        result = db.fromJSON( "{\"val\":3.14,locked:true}", &errMsg );
        REQUIRE( result == false );
        REQUIRE( errMsg != "NOERROR" );

        // ERROR
        result = db.fromJSON( "{\"val\":3.14,locked:true}" );
        REQUIRE( result == false );

        // ERROR
        errMsg = "NOERROR";
        result = db.fromJSON( "{\"id\":0}", &errMsg );
        REQUIRE( result == false );
        REQUIRE( errMsg != "NOERROR" );

        // ERROR
        result = db.fromJSON( "{\"id\":0}" );
        REQUIRE( result == false );
    }


    SECTION( "database" )
    {
        REQUIRE( db.add( *orange ) == false );

        REQUIRE( db.lookupById( ORANGE_ID ) == orange );
        REQUIRE( db.lookupById( APPLE_ID ) == apple );
        REQUIRE( db.lookupById( ORANGE_ID + 2 ) == nullptr );

        REQUIRE( db.first() == apple );
        REQUIRE( db.next( *apple ) == orange );
        REQUIRE( db.next( *orange ) == nullptr );
    }

    delete apple;
    delete orange;
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
