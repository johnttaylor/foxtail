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
#include "Cpl/Point/Uint32.h"
#include "Cpl/System/Trace.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Cpl::Point;

#define MAX_POINTS  3

constexpr Uint32::Id_T  appleId  = 0;
constexpr Uint32::Id_T  orangeId = 1;
constexpr Uint32::Id_T  cherryId = 2;
constexpr Uint32::Id_T  limeId = 3;

static Uint32 apple_( appleId );
static Uint32 orange_( orangeId );
static Uint32 cherry_( cherryId );
static Uint32 lime_( limeId );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Database" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Database uut( MAX_POINTS );

    SECTION( "add" )
    {
        Api* pt = uut.lookupById( appleId );
        REQUIRE( pt == 0 );

        Info_T info ={ &apple_, "APPLE" };
        REQUIRE( uut.add( appleId, info ) );
        info ={ &orange_, "ORANGE" };
        REQUIRE( uut.add( orangeId, info ) );
        pt = uut.lookupById( appleId );
        REQUIRE( pt == &apple_ );
        pt = uut.lookupById( orangeId );
        REQUIRE( pt == &orange_ );
        pt = uut.lookupById( cherryId );
        REQUIRE( pt == 0 );

        info ={ &cherry_, "CHERRY" };
        REQUIRE( uut.add( cherryId, info ) );
        info ={ &lime_, "LIME" };
        REQUIRE( uut.add( limeId, info ) == false );
        pt = uut.lookupById( cherryId );
        REQUIRE( pt == &cherry_ );
        pt = uut.lookupById( limeId );
        REQUIRE( pt == 0 );
    }

    SECTION( "toJson" )
    {
#define EXPECTED1 "{\"id\":0,\"valid\":true,\"type\":\"Cpl::Point::Uint32\",\"locked\":false,\"val\":\"0xA\"}"
#define EXPECTED4 "{\"id\":0,\"valid\":true,\"type\":\"Cpl::Point::Uint32\",\"locked\":true,\"val\":\"0xB\"}"
#define EXPECTED5 "{\"id\":0,\"valid\":true,\"type\":\"Cpl::Point::Uint32\",\"locked\":false,\"val\":\"0x3\"}"
#define EXPECTED3 "{\"id\":0,\"valid\":true,\"val\":\"0xA\"}"
#define EXPECTED2 "{\"id\":0,\"valid\":false,\"type\":\"Cpl::Point::Uint32\",\"locked\":false}"

        Info_T info ={ &apple_, "APPLE" };
        REQUIRE( uut.add( appleId, info ) );

        char buffer[256];
        bool truncated;
        apple_.write( 10 );
        REQUIRE( uut.toJSON( appleId, buffer, sizeof( buffer ), truncated ) );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( truncated == false );
        REQUIRE( strcmp( buffer, EXPECTED1 ) == 0 );

        REQUIRE( uut.toJSON( appleId, buffer, sizeof( buffer ), truncated, false ) );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( truncated == false );
        REQUIRE( strcmp( buffer, EXPECTED3 ) == 0 );

        apple_.setInvalid();
        REQUIRE( uut.toJSON( appleId, buffer, sizeof( buffer ), truncated ) );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( truncated == false );
        REQUIRE( strcmp( buffer, EXPECTED2 ) == 0 );

        apple_.write( 11, Cpl::Point::Api::eLOCK );
        REQUIRE( uut.toJSON( appleId, buffer, sizeof( buffer ), truncated ) );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( truncated == false );
        REQUIRE( strcmp( buffer, EXPECTED4 ) == 0 );

        apple_.write( 111 );
        REQUIRE( uut.toJSON( appleId, buffer, sizeof( buffer ), truncated ) );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( truncated == false );
        REQUIRE( strcmp( buffer, EXPECTED4 ) == 0 );

        apple_.write( 3, Cpl::Point::Api::eUNLOCK );
        REQUIRE( uut.toJSON( appleId, buffer, sizeof( buffer ), truncated ) );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( truncated == false );
        REQUIRE( strcmp( buffer, EXPECTED5 ) == 0 );

        REQUIRE( uut.toJSON( appleId, buffer, 50, truncated ) );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( truncated == true );
        REQUIRE( strncmp( buffer, EXPECTED5, 50-1 ) == 0 );

        REQUIRE( uut.toJSON( orangeId, buffer, sizeof( buffer ), truncated ) == false );
    }

    SECTION( "fromJson" )
    {
        Info_T info ={ &apple_, "APPLE" };
        REQUIRE( uut.add( appleId, info ) );
        apple_.write( 0xFFFFFFFF );
        apple_.setInvalid();

        Cpl::Text::FString<256> errorMsg = "<noerror>";
        bool result = uut.fromJSON( "{\"id\":0,\"val\":\"0xA\"}", &errorMsg );
        REQUIRE( result );
        bool valid;
        bool locked;
        uint32_t val;
        REQUIRE( apple_.get( val, valid, locked ) );
        REQUIRE( val == 10 );
        REQUIRE( valid );
        REQUIRE( locked == false );
        REQUIRE( errorMsg == "<noerror>" );

        result = uut.fromJSON( "{\"id\":0,\"val\":\"0xB\",valid:false}", &errorMsg );
        REQUIRE( result );
        REQUIRE( !apple_.get( val, valid, locked ) );
        REQUIRE( !valid );
        REQUIRE( locked == false );
        REQUIRE( errorMsg == "<noerror>" );

        result = uut.fromJSON( "{\"id\":0,\"val\":\"0xB\",locked:true}", &errorMsg );
        REQUIRE( result );
        REQUIRE( apple_.get( val, valid, locked ) );
        REQUIRE( val == 11 );
        REQUIRE( valid );
        REQUIRE( locked );
        REQUIRE( errorMsg == "<noerror>" );

        result = uut.fromJSON( "{\"id\":0,\"val\":\"0xBB\"}" );
        REQUIRE( result );
        REQUIRE( apple_.get( val, valid, locked ) );
        REQUIRE( val == 11 );
        REQUIRE( valid );
        REQUIRE( locked );

        result = uut.fromJSON( "{\"id\":0,\"val\":\"0xBB\",locked:false}");
        REQUIRE( result );
        REQUIRE( apple_.get( val, valid, locked ) );
        REQUIRE( val == 0xBB );
        REQUIRE( valid );
        REQUIRE( !locked );

        errorMsg = "<noerror>";
        result   = uut.fromJSON( "{\"id\":0,\"val\":true}", &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("errorMsg=[%s]", errorMsg.getString()) );
        REQUIRE( !result );
        REQUIRE( apple_.get( val, valid, locked ) );
        REQUIRE( val == 0xBB );
        REQUIRE( valid );
        REQUIRE( !locked );
        REQUIRE( errorMsg != "<noerror>" );

        errorMsg = "<noerror>";
        result   = uut.fromJSON( "{\"id\":0,\"val\":\"0x10", &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("errorMsg=[%s]", errorMsg.getString()) );
        REQUIRE( !result );
        REQUIRE( apple_.get( val, valid, locked ) );
        REQUIRE( val == 0xBB );
        REQUIRE( valid );
        REQUIRE( !locked );
        REQUIRE( errorMsg != "<noerror>" );

        errorMsg = "<noerror>";
        result   = uut.fromJSON( "{\"id\":\"bob\",\"val\":\"0x10\"}", &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("errorMsg=[%s]", errorMsg.getString()) );
        REQUIRE( !result );
        REQUIRE( apple_.get( val, valid, locked ) );
        REQUIRE( val == 0xBB );
        REQUIRE( valid );
        REQUIRE( !locked );
        REQUIRE( errorMsg != "<noerror>" );

        errorMsg = "<noerror>";
        result   = uut.fromJSON( "{\"id\":4,\"val\":\"0x10\"}", &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("errorMsg=[%s]", errorMsg.getString()) );
        REQUIRE( !result );
        REQUIRE( apple_.get( val, valid, locked ) );
        REQUIRE( val == 0xBB );
        REQUIRE( valid );
        REQUIRE( !locked );
        REQUIRE( errorMsg != "<noerror>" );
    }

    SECTION( "other" )
    {
        Info_T info ={ &apple_, "APPLE" };
        REQUIRE( uut.add( appleId, info ) );
        info ={ &orange_, "ORANGE" };
        REQUIRE( uut.add( orangeId, info ) );
        info ={ &cherry_, "CHERRY" };
        REQUIRE( uut.add( cherryId, info ) );

        REQUIRE( strcmp( uut.getSymbolicName( appleId ), "APPLE" ) == 0 );
        REQUIRE( strcmp( uut.getSymbolicName( orangeId ), "ORANGE" ) == 0 );
        REQUIRE( strcmp( uut.getSymbolicName( cherryId ), "CHERRY" ) == 0 );
        REQUIRE( uut.getSymbolicName( limeId ) == nullptr );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}