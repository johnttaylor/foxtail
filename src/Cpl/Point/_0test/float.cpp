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
#include "Cpl/Point/Float.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"
#include <string.h>
#include "Cpl/Memory/HPool.h"

#define SECT_   "_0test"

/// 
using namespace Cpl::Point;

#define MAX_POINTS  2

constexpr Float::Id_T  appleId  = 0;
constexpr Float::Id_T  orangeId = 1;

#define ORANGE_INIT_VAL 7

static Float apple_( appleId );
static Float orange_( orangeId, ORANGE_INIT_VAL );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Float" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Database db( MAX_POINTS );
    Info_T info ={ &apple_, "APPLE" };
    REQUIRE( db.add( appleId, info ) );
    info ={ &orange_, "ORANGE" };
    REQUIRE( db.add( orangeId, info ) );
    bool  valid;
    float value;

    SECTION( "create" )
    {
        Cpl::Memory::HPool<Float> heap( 1 );
        Api* pt = Float::create( heap, 0 );
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
        apple_.write( 1 );
        valid = apple_.read( value );
        REQUIRE( valid );
        REQUIRE( Cpl::Math::areFloatsEqual(value,1.0) );

        apple_.setInvalid();
        valid = apple_.read( value );
        REQUIRE( valid == false );

        REQUIRE( apple_.getSize() == sizeof( float ) );
    }

    SECTION( "write" )
    {
        value = apple_.write( 2 );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 2.0 ) );
        value = apple_.increment();
        REQUIRE( Cpl::Math::areFloatsEqual( value, 3.0 ) );
        value = apple_.increment( 3 );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 3.0+3.0 ) );
        value = apple_.decrement();
        REQUIRE( Cpl::Math::areFloatsEqual( value, 3.0 + 3.0 -1.0) );
        REQUIRE( value == 3 + 3 - 1 );
        value = apple_.decrement( 3 );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 2.0 ) );
        REQUIRE( value == 2 );
    }

    SECTION( "write2" )
    {
        apple_.write( orange_, Api::eLOCK );
        valid = apple_.read( value );
        REQUIRE( Cpl::Math::areFloatsEqual( value, ORANGE_INIT_VAL ) );
        REQUIRE( apple_.isLocked() );
        apple_.write( orange_ );
        REQUIRE( apple_.isLocked() );
        apple_.write( orange_, Api::eUNLOCK );
        REQUIRE( apple_.isLocked() == false );
        orange_.setInvalid();
        apple_.write( orange_ );
        REQUIRE( apple_.isNotValid() );
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
        value = doc["val"];
        REQUIRE( Cpl::Math::areFloatsEqual( value, 127 ) );

        result = db.fromJSON( "{\"id\":0,\"val\":3.14}" );
        valid  = apple_.read( value );
        REQUIRE( valid );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 3.14F ) );
    }
    
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
