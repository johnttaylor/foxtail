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
#include "Cpl/System/Trace.h"
#include <string.h>
#include "Cpl/Memory/HPool.h"
#include "Cpl/Memory/LeanHeap.h"
#include <new>

#define SECT_   "_0test"

/// 
using namespace Fxt::Point;

#define MAX_POINTS  2


#define ORANGE_INIT_VAL 7

#define APPLE_ID        0

#define ORANGE_ID       1

#define ELEM_SIZE_AS_SIZET(elemSize)    (((elemSize)+sizeof( size_t ) - 1) / sizeof(size_t))
static size_t stateHeapMemory_[ELEM_SIZE_AS_SIZET( sizeof( Int32::StateBlock_T ) ) * MAX_POINTS];

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Int32" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Database<MAX_POINTS>     db;
    Cpl::Memory::LeanHeap    stateHeap( stateHeapMemory_, sizeof( stateHeapMemory_ ) );
    bool                     valid;
    int32_t                  value;

    Int32* apple = new(std::nothrow) Int32( db, APPLE_ID, stateHeap );
    REQUIRE( apple );
    Int32* orange = new(std::nothrow) Int32( db, ORANGE_ID, stateHeap );
    REQUIRE( orange );


    SECTION( "read" )
    {
        valid = orange->read( value );
        REQUIRE( valid == false );

        valid = apple->read( value );
        REQUIRE( valid == false );
        apple->write( 1 );
        valid = apple->read( value );
        REQUIRE( valid );
        REQUIRE( value == 1 );

        apple->setInvalid();
        valid = apple->read( value );
        REQUIRE( valid == false );

        REQUIRE( apple->getStatefulMemorySize() >= (sizeof( int32_t ) + sizeof(bool)*2) );
    }

    SECTION( "write" )
    {
        apple->write( 2 );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == 2 );

        apple->increment();
        REQUIRE( apple->read( value ) );
        REQUIRE( value == 3 );

        apple->increment( 3 );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == 3 + 3 );

        apple->decrement();
        REQUIRE( apple->read( value ) );
        REQUIRE( value == 3 + 3 - 1 );

        apple->decrement( 3 );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == 2 );
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

    SECTION( "bitOpertions" )
    {
        apple->setInvalid();
        apple->maskOr( (int32_t) 0xA5 );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == (int32_t) 0xA5 );
        
        apple->maskAnd( (int32_t) 0xF0 );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == (int32_t) 0xA0 );

        apple->maskXor( (int32_t) 0x8A );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == (int32_t) 0x2A );

        apple->bitClear( 5 );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == 0x0A );
        
        apple->bitToggle( 3 );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == 0x02 );

        apple->bitSet( 4 );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == 0x12 );
    }

    SECTION( "json" )
    {
        char buffer[256];
        bool truncated;
        apple->write( 127 );

        bool result = db.toJSON( APPLE_ID, buffer, sizeof( buffer ), truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( result );
        REQUIRE( truncated == false );
        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, buffer );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( strcmp( doc["val"], "0x7F" ) == 0 );

        orange->setInvalid();
        result = db.toJSON( ORANGE_ID, buffer, sizeof( buffer ), truncated, false );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( result );
        REQUIRE( truncated == false );

        result = db.fromJSON( "{\"id\":0,\"val\":\"0x20\"}" );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == 0x20 );

        result = db.fromJSON( "{\"id\":0,\"val\":\"20\"}" );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == 20 );

        result = db.fromJSON( "{\"id\":0,\"val\":21}" );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == 21 );

        result = db.fromJSON( "{\"id\":0,\"val\":\"0x21\",locked:true}" );
        REQUIRE( result );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == 0x21 );
        REQUIRE( apple->isLocked() );

        result = db.fromJSON( "{\"id\":0,\"val\":\"0x22\",locked:false}" );
        REQUIRE( result );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == 0x22 );
        REQUIRE( !apple->isLocked() );

        result = db.fromJSON( "{\"id\":0,locked:true}" );
        REQUIRE( result );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == 0x22 );
        REQUIRE( apple->isLocked() );

        result = db.fromJSON( "{\"id\":0,locked:false,valid:false}" );
        REQUIRE( result );
        REQUIRE( apple->isNotValid() );
        REQUIRE( !apple->isLocked() );

        // ERROR
        result = db.fromJSON( "{\"id\":100,\"val\":\"0x20\",locked:true}" );
        REQUIRE( result == false );

        // ERROR
        Cpl::Text::FString<100> errMsg = "NOERROR";
        result = db.fromJSON( "{\"id\":100,\"val\":\"0x20\",locked:true}", &errMsg );
        REQUIRE( result == false );
        REQUIRE( errMsg != "NOERROR" );

        // ERROR
        errMsg = "NOERROR";
        result = db.fromJSON( "{\"id\":0,\"val\":20.1,locked:true}", &errMsg );
        REQUIRE( result == false );
        REQUIRE( errMsg != "NOERROR" );

        // ERROR
        result = db.fromJSON( "{\"id\":0,\"val\":20.1,locked:true}" );
        REQUIRE( result == false );

        // ERROR
        result = db.toJSON( ORANGE_ID+2, buffer, sizeof( buffer ), truncated, false );
        REQUIRE( result == false );

        // ERROR
        errMsg = "NOERROR";
        result = db.fromJSON( "{\"id\"=100,\"val\":\"0x20\",locked:true}", &errMsg );
        REQUIRE( result == false );
        REQUIRE( errMsg != "NOERROR" );

        // ERROR
        result = db.fromJSON( "{\"id\"=100,\"val\":\"0x20\",locked:true}" );
        REQUIRE( result == false );

        // ERROR
        errMsg = "NOERROR";
        result = db.fromJSON( "{\"val\":\"0x20\",locked:true}", &errMsg );
        REQUIRE( result == false );
        REQUIRE( errMsg != "NOERROR" );

        // ERROR
        result = db.fromJSON( "{\"val\":\"0x20\",locked:true}" );
        REQUIRE( result == false );

        // ERROR
        errMsg = "NOERROR";
        result = db.fromJSON( "{\"id\":0}", &errMsg );
        REQUIRE( result == false );
        REQUIRE( errMsg != "NOERROR" );

        // ERROR
        result = db.fromJSON( "{\"id\":0}");
        REQUIRE( result == false );
    }


    SECTION( "database" )
    {
        REQUIRE( db.add( *orange ) == false );

        REQUIRE( db.lookupById( ORANGE_ID ) == orange );
        REQUIRE( db.lookupById( APPLE_ID ) == apple );
        REQUIRE( db.lookupById( ORANGE_ID+2 ) == nullptr );
    }
    
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
