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

#define SECT_   "_0test"

/// 
using namespace Fxt::Point;

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

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}