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
#include "Fxt/Point/Double.h"
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

#define ORANGE_ID       1

static size_t stateHeapMemory_[(sizeof( Double::StateBlock_T )  * MAX_POINTS + sizeof( size_t ) - 1) / sizeof( size_t )];

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Double" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Database<MAX_POINTS>     db;
    Cpl::Memory::LeanHeap    stateHeap( stateHeapMemory_, sizeof( stateHeapMemory_ ) );
    bool                     valid;
    double                   value;

    Double* apple = new(std::nothrow) Double( db, APPLE_ID, stateHeap );
    REQUIRE( apple );
    Double* orange = new(std::nothrow) Double( db, ORANGE_ID, stateHeap );
    REQUIRE( orange );


    SECTION( "read" )
    {
        valid = orange->read( value );
        REQUIRE( valid == false );

        valid = apple->read( value );
        REQUIRE( valid == false );
        apple->write( 1.1 );
        valid = apple->read( value );
        REQUIRE( valid );
        REQUIRE( Cpl::Math::areDoublesEqual( value, 1.1 ) );

        apple->setInvalid();
        valid = apple->read( value );
        REQUIRE( valid == false );

        REQUIRE( apple->getStatefulMemorySize() >= (sizeof( double ) + sizeof( bool ) * 2) );
    }

    SECTION( "write" )
    {
        apple->write( 2 );
        REQUIRE( apple->read( value ) );
        REQUIRE( Cpl::Math::areDoublesEqual( value, 2 ) );

        apple->increment();
        REQUIRE( apple->read( value ) );
        REQUIRE( Cpl::Math::areDoublesEqual( value, 3 ) );

        apple->increment( 3 );
        REQUIRE( apple->read( value ) );
        REQUIRE( Cpl::Math::areDoublesEqual( value, 3 + 3 ) );

        apple->decrement();
        REQUIRE( apple->read( value ) );
        REQUIRE( Cpl::Math::areDoublesEqual( value, 3 + 3 - 1 ) );

        apple->decrement( 3 );
        REQUIRE( apple->read( value ) );
        REQUIRE( Cpl::Math::areDoublesEqual( value, 2 ) );
    }

    SECTION( "write2" )
    {
        orange->write( ORANGE_INIT_VAL );
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

    SECTION( "setter" )
    {
        orange->write( ORANGE_INIT_VAL, Api::eLOCK );
        apple->setInvalid( Api::eUNLOCK );
        orange->updateFromSetter( *apple );
        REQUIRE( orange->isNotValid() == false );
        REQUIRE( orange->isLocked() == true );

        orange->updateFromSetter( *apple, Api::eUNLOCK );
        REQUIRE( orange->isNotValid() );
        REQUIRE( orange->isLocked() == false );

        apple->write( ORANGE_INIT_VAL );
        orange->updateFromSetter( *apple );
        REQUIRE( orange->isNotValid() == false );
        REQUIRE( orange->isLocked() == false );
    }

    SECTION( "json" )
    {
        char buffer[256];
        bool truncated;
        Cpl::Text::FString<100> errMsg;
        apple->write( 127.3 );

        bool result = db.toJSON( APPLE_ID, buffer, sizeof( buffer ), truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( result );
        REQUIRE( truncated == false );
        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, buffer );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( Cpl::Math::areDoublesEqual( doc["val"], 127.3 ) );

        orange->setInvalid();
        result = db.toJSON( ORANGE_ID, buffer, sizeof( buffer ), truncated, false );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( result );
        REQUIRE( truncated == false );

        result = db.fromJSON( "{\"id\":0,\"val\":3.14}" );
        REQUIRE( apple->read( value ) );
        REQUIRE( Cpl::Math::areDoublesEqual( value, 3.14 ) );


        result = db.fromJSON( "{\"id\":0,\"val\":21,locked:true}" );
        REQUIRE( result );
        REQUIRE( apple->read( value ) );
        REQUIRE( Cpl::Math::areDoublesEqual( value, 21 ) );
        REQUIRE( apple->isLocked() );

        result = db.fromJSON( "{\"id\":0,\"val\":22,locked:false}" );
        REQUIRE( result );
        REQUIRE( apple->read( value ) );
        REQUIRE( Cpl::Math::areDoublesEqual( value, 22 ) );
        REQUIRE( !apple->isLocked() );

        result = db.fromJSON( "{\"id\":0,locked:true}" );
        REQUIRE( result );
        REQUIRE( apple->read( value ) );
        REQUIRE( Cpl::Math::areDoublesEqual( value, 22 ) );
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
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
