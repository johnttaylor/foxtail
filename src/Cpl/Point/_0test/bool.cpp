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
#include "Cpl/Point/Bool.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Memory/HPool.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Cpl::Point;

#define MAX_POINTS  2

constexpr Bool::Id_T  appleId  = 0;
constexpr Bool::Id_T  orangeId = 1;

#define ORANGE_INIT_VAL true

static Bool apple_( appleId );
static Bool orange_( orangeId, ORANGE_INIT_VAL );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Bool" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Database db( MAX_POINTS );
    Info_T info ={ &apple_, "APPLE" };
    REQUIRE( db.add( appleId, info ) );
    info ={ &orange_, "ORANGE" };
    REQUIRE( db.add( orangeId, info ) );
    bool     valid;
    bool value;

    SECTION( "create" )
    {
        Cpl::Memory::HPool<Bool> heap( 1 );
        Api* pt = Bool::create( heap, 0 );
        REQUIRE( pt );
        REQUIRE( strcmp( pt->getTypeAsText(), apple_.getTypeAsText() ) == 0 );
    }
    
    SECTION( "read" )
    {
        valid = orange_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value == ORANGE_INIT_VAL );

        valid = apple_.read( value );
        REQUIRE( valid == false );
        apple_.set();
        valid = apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == true );

        apple_.setInvalid();
        valid = apple_.read( value );
        REQUIRE( valid == false );

        REQUIRE( apple_.getSize() == sizeof( bool ) );
    }

    SECTION( "write" )
    {
        value = apple_.clear();
        REQUIRE( value == false );
        value = apple_.set();
        REQUIRE( value == true );
        value = apple_.clear();
        REQUIRE( value == false );
    }


    SECTION( "json" )
    {
        char buffer[256];
        bool truncated;
        apple_.write( true );
        bool result = db.toJSON( appleId, buffer, sizeof( buffer ), truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( result );
        REQUIRE( truncated == false );
        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, buffer );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["val"] == true );

        result = db.fromJSON( "{\"id\":0,\"val\":false}" );
        valid  = apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == false );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
