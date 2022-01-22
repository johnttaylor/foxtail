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
#include "Cpl/Point/String_.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Memory/HPool.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Cpl::Point;

#define STR_LEN     10

// Test Class
class MyString : public String_<STR_LEN>
{
public:
    /// Type safe Point Identifier
    class Id_T : public Identifier_T
    {
    public:
        constexpr Id_T() : Identifier_T() {}
        constexpr Id_T( uint32_t x ) : Identifier_T( x ) {}
    };


public:
    /// Returns the Point's Identifier
    inline MyString::Id_T getId() const noexcept { return m_id; }

public:
    /** Constructor. Invalid Point.
     */
    MyString( const Id_T myIdentifier ) : String_<STR_LEN>(), m_id( myIdentifier ) {}

    /// Constructor. Valid Point.  Requires an initial value
    MyString( const Id_T myIdentifier, const char* initialValue ) :String_<STR_LEN>( initialValue ), m_id( myIdentifier ) {}

public:
    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept { return "Cpl::Point::MyString::10"; }

    /// Creates a concrete instance in the invalid state
    static Api* create( Cpl::Memory::Allocator& allocatorForPoints, uint32_t pointId ) { return new(allocatorForPoints.allocate( sizeof( MyString ) )) MyString( pointId ); }

protected:
    /// The points numeric identifier
    Id_T m_id;
};
#define MAX_POINTS  2

constexpr MyString::Id_T  appleId  = 0;
constexpr MyString::Id_T  orangeId = 1;

#define ORANGE_INIT_VAL "Hello Bob"

static MyString apple_( appleId );
static MyString orange_( orangeId, ORANGE_INIT_VAL );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "MyString" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Database db( MAX_POINTS );
    Info_T info ={ &apple_, "APPLE" };
    REQUIRE( db.add( appleId, info ) );
    info ={ &orange_, "ORANGE" };
    REQUIRE( db.add( orangeId, info ) );
    bool valid;
    char value[STR_LEN+1];
    Cpl::Text::FString<20> valStr;

    SECTION( "create" )
    {
        Cpl::Memory::HPool<MyString> heap( 1 );
        Api* pt = MyString::create( heap, 0 );
        REQUIRE( pt );
        REQUIRE( strcmp( pt->getTypeAsText(), apple_.getTypeAsText() ) == 0 );
    }

    SECTION( "read" )
    {
        valid = orange_.read( value, STR_LEN );
        REQUIRE( valid == true );
        REQUIRE( strcmp( value, ORANGE_INIT_VAL ) == 0 );

        valid = apple_.read( valStr );
        REQUIRE( valid == false );
        apple_.write( "Hello World" );
        valid = apple_.read( valStr );
        REQUIRE( valid );
        REQUIRE( valStr == "Hello Worl" );

        apple_.setInvalid();
        valid = apple_.read( valStr );
        REQUIRE( valid == false );

        REQUIRE( apple_.getSize() == STR_LEN+1 );
    }

    SECTION( "write" )
    {
        valStr = apple_.write( "Bob was here, or maybe not", Api::eLOCK );
        REQUIRE( valStr == "Bob was he" );
        valStr = apple_.write( "Hello" );
        REQUIRE( valStr == "Bob was he" );
        valStr = apple_.write( "Hello", Api::eUNLOCK );
        REQUIRE( valStr == "Hello" );
    }

    SECTION( "other" )
    {
        apple_.write( "Hi" );
        bool isValid;
        bool locked;
        valid = apple_.get( valStr, isValid, locked );
        REQUIRE( valStr == "Hi" );
        REQUIRE( valid == true );
        REQUIRE( locked == false );
        apple_.setInvalid( Api::eLOCK );
        valid = apple_.get( value, STR_LEN, isValid, locked );
        REQUIRE( valid == false );
        REQUIRE( locked == true );
        apple_.setLockState( Api::eUNLOCK );
    }

    SECTION( "json" )
    {
        char buffer[256];
        bool truncated;
        apple_.write( "Bobby" );
        bool result = db.toJSON( appleId, buffer, sizeof( buffer ), truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( result );
        REQUIRE( truncated == false );
        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, buffer );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( strcmp( doc["val"], "Bobby" ) == 0 );

        result = db.fromJSON( "{\"id\":0,\"val\":\" hi there \"}" );
        REQUIRE( result );
        valid  = apple_.read( valStr );
        REQUIRE( valid );
        REQUIRE( valStr == " hi there "  );

        result = db.fromJSON( "{\"id\":0,\"val\":123}" );
        REQUIRE( result == false );
        valid  = apple_.read( valStr );
        REQUIRE( valid );
        REQUIRE( valStr == " hi there " );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
