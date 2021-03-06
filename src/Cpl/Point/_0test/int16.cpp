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
#include "Cpl/Point/Database.h"
#include "Cpl/Point/Int16.h"
#include "Cpl/System/Trace.h"
#include <string.h>
#include "Cpl/Memory/HPool.h"

#define SECT_   "_0test"

/// 
using namespace Cpl::Point;

#define MAX_POINTS  2

constexpr Int16::Id_T  appleId  = 0;
constexpr Int16::Id_T  orangeId = 1;

#define ORANGE_INIT_VAL 7

static Int16 apple_( appleId );
static Int16 orange_( orangeId, ORANGE_INIT_VAL );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Int16" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Database db( MAX_POINTS );
    Info_T info ={ &apple_, "APPLE" };
    REQUIRE( db.add( appleId, info ) );
    info ={ &orange_, "ORANGE" };
    REQUIRE( db.add( orangeId, info ) );
    bool     valid;
    int16_t value;

    SECTION( "create" )
    {
        Cpl::Memory::HPool<Int16> heap( 1 );
        Api* pt = Int16::create( heap, 0 );
        REQUIRE( pt );
        REQUIRE( strcmp( pt->getTypeAsText(), apple_.getTypeAsText() ) == 0 );

        REQUIRE( apple_.getTotalSize() == sizeof( apple_ ) );
    }

    SECTION( "read" )
    {
        valid = orange_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value == ORANGE_INIT_VAL );

        valid = apple_.read( value );
        REQUIRE( valid == false );
        apple_.write( 1 );
        valid = apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 1 );

        apple_.setInvalid();
        valid = apple_.read( value );
        REQUIRE( valid == false );

        REQUIRE( apple_.getSize() == sizeof( int16_t ) );
    }

    SECTION( "write" )
    {
        value = apple_.write( 2 );
        REQUIRE( value == 2 );
        value = apple_.increment();
        REQUIRE( value == 3 );
        value = apple_.increment( 3 );
        REQUIRE( value == 3 + 3 );
        value = apple_.decrement();
        REQUIRE( value == 3 + 3 - 1 );
        value = apple_.decrement( 3 );
        REQUIRE( value == 2 );
    }

    SECTION( "write2" )
    {
        apple_.write( orange_, Api::eLOCK );
        valid = apple_.read( value );
        REQUIRE( value == ORANGE_INIT_VAL );
        REQUIRE( apple_.isLocked() );
        apple_.write( orange_ );
        REQUIRE( apple_.isLocked() );
        apple_.write( orange_, Api::eUNLOCK );
        REQUIRE( apple_.isLocked() == false );
        orange_.setInvalid();
        apple_.write( orange_ );
        REQUIRE( apple_.isNotValid() );
    }
    
    SECTION( "bitOpertions" )
    {
        apple_.setInvalid();
        value = apple_.maskOr( 0xA5 );
        REQUIRE( value == 0xA5 );
        value = apple_.maskAnd( 0xF0 );
        REQUIRE( value == 0xA0 );
        value = apple_.maskXor( 0x8A );
        REQUIRE( value == 0x2A );

        value = apple_.bitClear( 5 );
        REQUIRE( value == 0x0A );
        value = apple_.bitToggle( 3 );
        REQUIRE( value == 0x02 );
        value = apple_.bitSet( 4 );
        REQUIRE( value == 0x12 );
    }

    SECTION( "json" )
    {
        char buffer[256];
        bool truncated;
        apple_.write( 127 );
        bool result = db.toJSON( appleId, buffer, sizeof( buffer ), truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( result );
        REQUIRE( truncated == false );
        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, buffer );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( strcmp( doc["val"], "0x7F" ) == 0  );

        result = db.fromJSON( "{\"id\":0,\"val\":\"0xA\"}" );
        valid  = apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 0xA );
    }
    
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
