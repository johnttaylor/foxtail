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
#include "Fxt/Point/Int8.h"
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
#define APPLE_LABEL     "APPLE"

#define ORANGE_ID       0
#define ORANGE_LABEL    "ORANGE"

static size_t stateHeapMemory_[((sizeof( int8_t ) + sizeof( size_t )) * MAX_POINTS + sizeof( size_t ) - 1) / sizeof( size_t )];

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Int8" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Database<MAX_POINTS>     db;
    Cpl::Memory::LeanHeap    stateHeap( stateHeapMemory_, sizeof( stateHeapMemory_ ) );
    bool                     valid;
    int8_t                   value;

    Int8* apple = new(std::nothrow) Int8( APPLE_ID, APPLE_LABEL, stateHeap );
    REQUIRE( apple );
    Int8* orange = new(std::nothrow) Int8( ORANGE_ID, ORANGE_LABEL, stateHeap, ORANGE_INIT_VAL );
    REQUIRE( orange );


    //SECTION( "create" )
    //{
    //    Cpl::Memory::HPool<Int8> heap( 1 );
    //    Api* pt = Int8::create( heap, 0 );
    //    REQUIRE( pt );
    //    REQUIRE( strcmp( pt->getTypeAsText(), apple->getTypeAsText() ) == 0 );

    //    REQUIRE( apple->getTotalSize() == sizeof( apple-> ) );
    //}

    SECTION( "read" )
    {
        valid = orange->read( value );
        REQUIRE( valid == true );
        REQUIRE( value == ORANGE_INIT_VAL );

        valid = apple->read( value );
        REQUIRE( valid == false );
        apple->write( 1 );
        valid = apple->read( value );
        REQUIRE( valid );
        REQUIRE( value == 1 );

        apple->setInvalid();
        valid = apple->read( value );
        REQUIRE( valid == false );

        REQUIRE( apple->getStatefulMemorySize() >= (sizeof( int8_t ) + sizeof(bool)*2) );
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

    SECTION( "bitOpertions" )
    {
        apple->setInvalid();
        apple->maskOr( (int8_t) 0xA5 );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == (int8_t) 0xA5 );
        
        apple->maskAnd( (int8_t) 0xF0 );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == (int8_t) 0xA0 );

        apple->maskXor( (int8_t) 0x8A );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == (int8_t) 0x2A );

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
        db.add( *apple );
        db.add( *orange );

        bool result = db.toJSON( APPLE_ID, buffer, sizeof( buffer ), truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( result );
        REQUIRE( truncated == false );
        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, buffer );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( strcmp( doc["val"], "0x7F" ) == 0 );

        result = db.fromJSON( "{\"id\":0,\"val\":\"0x20\"}" );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == 0x20 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
