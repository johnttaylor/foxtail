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
#include "Cpl/Point/Enum_.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Type/enum.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Memory/HPool.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Cpl::Point;

BETTER_ENUM( MyColors, int, eRED, eBLUE, eGREEN );

// Test Class
class MyEnum : public Enum_<MyColors>
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
    inline MyEnum::Id_T getId() const noexcept { return m_id; }

public:
    /** Constructor. Invalid Point.
     */
    MyEnum( const Id_T myIdentifier ) : Enum_<MyColors>(), m_id( myIdentifier ) {}

    /// Constructor. Valid Point.  Requires an initial value
    MyEnum( const Id_T myIdentifier, MyColors initialValue ) :Enum_<MyColors>( initialValue ), m_id( myIdentifier ) {}

public:
    /// Pull in overloaded methods from base class
    using Enum_<MyColors>::write;

    /// Updates the MP's data from 'src'
    virtual void write( MyEnum& src, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST ) noexcept 
    {
        if ( src.isNotValid() )
        {
            setInvalid();
        }
        else
        {
            Enum_<MyColors>::write( src.m_data, lockRequest );
        }
    }

public:
    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept { return "Cpl::Point::MyEnum"; }

    /// Creates a concrete instance in the invalid state
    static Api* create( Cpl::Memory::Allocator& allocatorForPoints, uint32_t pointId ) { return new(allocatorForPoints.allocate( sizeof( MyEnum ) )) MyEnum( pointId ); }

protected:
    /// The points numeric identifier
    Id_T m_id;
};


#define MAX_POINTS  2

constexpr MyEnum::Id_T  appleId  = 0;
constexpr MyEnum::Id_T  orangeId = 1;

#define ORANGE_INIT_VAL MyColors::eRED

static MyEnum apple_( appleId );
static MyEnum orange_( orangeId, ORANGE_INIT_VAL );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Enum" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Database db( MAX_POINTS );
    Info_T info ={ &apple_, "APPLE" };
    REQUIRE( db.add( appleId, info ) );
    info ={ &orange_, "ORANGE" };
    REQUIRE( db.add( orangeId, info ) );
    bool     valid;
    MyColors value = MyColors::eBLUE;


    SECTION( "create" )
    {
        Cpl::Memory::HPool<MyEnum> heap( 1 );
        Api* pt = MyEnum::create( heap, 0 );
        REQUIRE( pt );
        REQUIRE( strcmp( pt->getTypeAsText(), apple_.getTypeAsText() ) == 0 );
    }

    SECTION( "read" )
    {
        valid = orange_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value == +ORANGE_INIT_VAL );

        valid = apple_.read( value );
        REQUIRE( valid == false );
        apple_.write( MyColors::eGREEN );
        valid = apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == +MyColors::eGREEN );

        apple_.setInvalid();
        valid = apple_.read( value );
        REQUIRE( valid == false );

        REQUIRE( apple_.getSize() == sizeof( MyColors ) );
    }

    SECTION( "write" )
    {
        value = apple_.write( MyColors::eGREEN );
        REQUIRE( value == +MyColors::eGREEN );
        value = apple_.write( MyColors::eRED );
        REQUIRE( value == +MyColors::eRED );
    }

    SECTION( "write2" )
    {
        apple_.write( orange_, Api::eLOCK );
        valid = apple_.read( value );
        REQUIRE( value == +ORANGE_INIT_VAL );
        REQUIRE( apple_.isLocked() );
        apple_.write( orange_ );
        REQUIRE( apple_.isLocked() );
        apple_.write( orange_, Api::eUNLOCK );
        REQUIRE( apple_.isLocked() == false );
        orange_.setInvalid();
        apple_.write( orange_ );
        REQUIRE( apple_.isNotValid() );
    }

    SECTION( "other" )
    {
        value = apple_.write( MyColors::eGREEN );
        bool isValid;
        bool locked;
        valid = apple_.get( value, isValid, locked );
        REQUIRE( value == +MyColors::eGREEN );
        REQUIRE( valid == true );
        REQUIRE( locked == false );
        apple_.setInvalid( Api::eLOCK );
        valid = apple_.get( value, isValid, locked );
        REQUIRE( valid == false );
        REQUIRE( locked == true );
        apple_.setLockState( Api::eUNLOCK );
    }

    SECTION( "json" )
    {
        char buffer[256];
        bool truncated;
        apple_.write( MyColors::eGREEN );
        bool result = db.toJSON( appleId, buffer, sizeof( buffer ), truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( result );
        REQUIRE( truncated == false );
        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, buffer );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( strcmp( doc["val"], "eGREEN" ) == 0 );

        result = db.fromJSON( "{\"id\":0,\"val\":\"eBLUE\"}" );
        valid  = apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == +MyColors::eBLUE );
    
        result = db.fromJSON( "{\"id\":0,\"val\":123}" );
        REQUIRE( result == false );
        Cpl::Text::FString<256> errorMsg = "<noerror>";
        result = db.fromJSON( "{\"id\":0,\"val\":true}", &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("errorMsg: [%s]", errorMsg.getString() ) );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "<noerror>" );
    }


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
