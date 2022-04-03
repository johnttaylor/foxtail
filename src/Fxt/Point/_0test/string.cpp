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
#include "Fxt/Point/String_.h"
#include "Cpl/System/Trace.h"
#include <string.h>
#include "Cpl/Memory/HPool.h"
#include "Cpl/Memory/LeanHeap.h"
#include "Cpl/Type/enum.h"
#include "Cpl/Text/FString.h"
#include <new>

#define SECT_   "_0test"

/// 
using namespace Fxt::Point;


// Anonymous namespace
namespace {



class String16 : public String_<16>
{
public:
    /// Constructor. Invalid Point.
    String16( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData ) : String_<16>( pointId, pointName, allocatorForPointStatefulData )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value
    String16( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData, const char* initialValue ) :String_<16>( pointId, pointName, allocatorForPointStatefulData, initialValue )
    {
    }

    /// Pull in overloaded methods from base class
    using StringBase_::write;

    /// Updates the MP's data from 'src'
    virtual void write( String16& src, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        updateFrom( &(((StateBlock_T*) (src.m_state))->data), sizeof( (((StateBlock_T*) (src.m_state))->data) ), src.isNotValid(), lockRequest );
    }

    ///  See Cpl::Dm::ModelPoint.
    const char* getType() const noexcept { return "Fxt::Point::String16"; }

    /// Creates a concrete instance in the invalid state
    static Api* create( Cpl::Memory::Allocator&             allocatorForPoints,
                        uint32_t                            pointId,
                        const char*                         pointName,
                        Cpl::Memory::ContiguousAllocator&   allocatorForPointStatefulData )
    {
        return PointCommon_::create<String16>( allocatorForPoints, pointId, pointName, allocatorForPointStatefulData );
    }
};

}; // end anonymous namespace

#define MAX_POINTS  2


#define ORANGE_INIT_VAL "Hello World"

#define APPLE_ID        0
#define APPLE_LABEL     "APPLE"

#define ORANGE_ID       1
#define ORANGE_LABEL    "ORANGE"

static size_t stateHeapMemory_[(sizeof( String16::StateBlock_T ) * MAX_POINTS + sizeof( size_t ) - 1) / sizeof( size_t )];

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "String16" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Database<MAX_POINTS>     db;
    Cpl::Memory::LeanHeap    stateHeap( stateHeapMemory_, sizeof( stateHeapMemory_ ) );
    bool                     valid;
    char                     buffer[16 + 1];
    Cpl::Text::FString<16>   value;

    String16* apple = new(std::nothrow) String16( APPLE_ID, APPLE_LABEL, stateHeap );
    REQUIRE( apple );
    String16* orange = new(std::nothrow) String16( ORANGE_ID, ORANGE_LABEL, stateHeap, ORANGE_INIT_VAL );
    REQUIRE( orange );


    SECTION( "read/write/invalid" )
    {
        valid = orange->read( value );
        REQUIRE( valid == true );
        REQUIRE( value == ORANGE_INIT_VAL );

        valid = apple->read( value );
        REQUIRE( valid == false );
        apple->write( "Fred" );
        valid = apple->read( buffer, sizeof( buffer ) );
        REQUIRE( valid );
        REQUIRE( strcmp( buffer, "Fred" ) == 0 );

        apple->setInvalid();
        valid = apple->read( value );
        REQUIRE( valid == false );

        apple->write( "Not Fred" );
        valid = apple->read( value );
        REQUIRE( valid );
        REQUIRE( value == "Not Fred" );

        REQUIRE( apple->getStatefulMemorySize() >= (sizeof(buffer) + sizeof( bool ) * 2) );
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

    SECTION( "json" )
    {
        char buffer[256];
        bool truncated;
        apple->write( "Hello JSON" );
        REQUIRE( db.add( *apple ) );
        REQUIRE( db.add( *orange ) );

        bool result = db.toJSON( APPLE_ID, buffer, sizeof( buffer ), truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( result );
        REQUIRE( truncated == false );
        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, buffer );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( strcmp( doc["val"], "Hello JSON" ) == 0 );

        orange->setInvalid();
        result = db.toJSON( ORANGE_ID, buffer, sizeof( buffer ), truncated, false );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( result );
        REQUIRE( truncated == false );

        result = db.fromJSON( "{\"id\":0,\"val\":\"eBLUE\"}" );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == "eBLUE" );

        result = db.fromJSON( "{\"id\":0,\"val\":\"eGREEN\"}" );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == "eGREEN");

        result = db.fromJSON( "{\"id\":0,\"val\":\"eRED\",locked:true}" );
        REQUIRE( result );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == "eRED" );
        REQUIRE( apple->isLocked() );

        result = db.fromJSON( "{\"id\":0,\"val\":\"eGREEN\",locked:false}" );
        REQUIRE( result );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == "eGREEN" );
        REQUIRE( !apple->isLocked() );

        result = db.fromJSON( "{\"id\":0,\"val\":\"123456789 123456789 2\",locked:false}" );
        REQUIRE( result );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == "123456789 123456" );
        REQUIRE( !apple->isLocked() );

        result = db.fromJSON( "{\"id\":0,locked:true}" );
        REQUIRE( result );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == "123456789 123456" );
        REQUIRE( apple->isLocked() );

        result = db.fromJSON( "{\"id\":0,locked:false,valid:false}" );
        REQUIRE( result );
        REQUIRE( apple->isNotValid() );
        REQUIRE( !apple->isLocked() );

        // ERROR
        result = db.fromJSON( "{\"id\":100,\"val\":\"eBLUE\",locked:true}" );
        REQUIRE( result == false );

        // ERROR
        Cpl::Text::FString<100> errMsg = "NOERROR";
        result = db.fromJSON( "{\"id\":100,\"val\":\"eBLUE\",locked:true}", &errMsg );
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
        result = db.toJSON( ORANGE_ID + 2, buffer, sizeof( buffer ), truncated, false );
        REQUIRE( result == false );

        // ERROR
        errMsg = "NOERROR";
        result = db.fromJSON( "{\"id\"=100,\"val\":\"eBLUE\",locked:true}", &errMsg );
        REQUIRE( result == false );
        REQUIRE( errMsg != "NOERROR" );

        // ERROR
        result = db.fromJSON( "{\"id\"=100,\"val\":\"eBLUE\",locked:true}" );
        REQUIRE( result == false );

        // ERROR
        errMsg = "NOERROR";
        result = db.fromJSON( "{\"val\":\"eBLUE\",locked:true}", &errMsg );
        REQUIRE( result == false );
        REQUIRE( errMsg != "NOERROR" );

        // ERROR
        result = db.fromJSON( "{\"val\":\"eBLUE\",locked:true}" );
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
        REQUIRE( db.add( *apple ) );
        REQUIRE( db.add( *orange ) );
        REQUIRE( db.add( *orange ) == false );

        REQUIRE( db.lookupById( ORANGE_ID ) == orange );
        REQUIRE( db.lookupById( APPLE_ID ) == apple );
        REQUIRE( db.lookupById( ORANGE_ID + 2 ) == nullptr );

        REQUIRE( db.first() == apple );
        REQUIRE( db.next( *apple ) == orange );
        REQUIRE( db.next( *orange ) == nullptr );
    }

    delete apple;
    delete orange;
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
