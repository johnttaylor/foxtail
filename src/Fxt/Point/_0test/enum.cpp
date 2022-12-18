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
#include "Fxt/Point/Enum_.h"
#include "Cpl/System/Trace.h"
#include <string.h>
#include "Cpl/Memory/HPool.h"
#include "Cpl/Memory/LeanHeap.h"
#include "Cpl/Type/enum.h"
#include <new>

#define SECT_   "_0test"

/// 
using namespace Fxt::Point;


// Anonymous namespace
namespace {

BETTER_ENUM( MyColors, int, eRED, eBLUE, eGREEN );

class MyEnum : public Enum_<MyColors>
{
public:
    /// Constructor. Invalid Point.
    MyEnum( DatabaseApi&                        db,
            uint32_t                            pointId,
            Cpl::Memory::ContiguousAllocator&   allocatorForPointStatefulData,
            Api*                                setterPoint  = nullptr )
        : Enum_<MyColors>( db, pointId, allocatorForPointStatefulData, setterPoint )
    {
    }

    ///  See Fxt::Point::Api
    const char* getTypeGuid() const noexcept { return "13208238-bbc9-4b38-a505-c20cadcafd87"; }

    ///  See Fxt::Point::Api
    const char* getTypeName() const noexcept { return "Fxt::Point::MyEnum"; }
};

}; // end anonymous namespace

#define MAX_POINTS  2


#define ORANGE_INIT_VAL MyColors::eGREEN

#define APPLE_ID        0

#define ORANGE_ID       1


#define ELEM_SIZE_AS_SIZET(elemSize)    (((elemSize)+sizeof( size_t ) - 1) / sizeof(size_t))
static size_t stateHeapMemory_[ELEM_SIZE_AS_SIZET( sizeof( MyEnum::StateBlock_T ) ) * MAX_POINTS];

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Enum" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Database<MAX_POINTS>     db;
    Cpl::Memory::LeanHeap    stateHeap( stateHeapMemory_, sizeof( stateHeapMemory_ ) );
    bool                     valid;
    MyColors                 value = MyColors::eBLUE;

    MyEnum* apple = new(std::nothrow) MyEnum( db, APPLE_ID, stateHeap );
    REQUIRE( apple );
    MyEnum* orange = new(std::nothrow) MyEnum( db, ORANGE_ID, stateHeap );
    REQUIRE( orange );


    SECTION( "read/write/invalid" )
    {
        valid = orange->read( value );
        REQUIRE( valid == false );

        valid = apple->read( value );
        REQUIRE( valid == false );
        apple->write( MyColors::eRED );
        valid = apple->read( value );
        REQUIRE( valid );
        REQUIRE( value == +MyColors::eRED );

        apple->setInvalid();
        valid = apple->read( value );
        REQUIRE( valid == false );

        apple->write( MyColors::eBLUE );
        valid = apple->read( value );
        REQUIRE( valid );
        REQUIRE( value == +MyColors::eBLUE );

        REQUIRE( apple->getStatefulMemorySize() >= (sizeof( MyColors ) + sizeof( bool ) * 2) );
    }

    SECTION( "write2" )
    {
        orange->write( ORANGE_INIT_VAL );
        apple->write( *orange, Api::eLOCK );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == +ORANGE_INIT_VAL );
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
        apple->write( MyColors::eRED );

        bool result = db.toJSON( APPLE_ID, buffer, sizeof( buffer ), truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( result );
        REQUIRE( truncated == false );
        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, buffer );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( strcmp( doc["val"], (+MyColors::eRED)._to_string() ) == 0 );

        orange->setInvalid();
        result = db.toJSON( ORANGE_ID, buffer, sizeof( buffer ), truncated, false );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( result );
        REQUIRE( truncated == false );

        result = db.fromJSON( "{\"id\":0,\"val\":\"eBLUE\"}" );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == +MyColors::eBLUE );

        result = db.fromJSON( "{\"id\":0,\"val\":\"eGREEN\"}" );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == +MyColors::eGREEN );

        result = db.fromJSON( "{\"id\":0,\"val\":\"eRED\",locked:true}" );
        REQUIRE( result );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == +MyColors::eRED );
        REQUIRE( apple->isLocked() );

        result = db.fromJSON( "{\"id\":0,\"val\":\"eGREEN\",locked:false}" );
        REQUIRE( result );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == +MyColors::eGREEN );
        REQUIRE( !apple->isLocked() );

        result = db.fromJSON( "{\"id\":0,locked:true}" );
        REQUIRE( result );
        REQUIRE( apple->read( value ) );
        REQUIRE( value == +MyColors::eGREEN );
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

        // ERROR
        errMsg = "NOERROR";
        result = db.fromJSON( "{\"id\":0,\"val\":\"eYELLOW\",locked:true}", &errMsg );
        REQUIRE( result == false );
        REQUIRE( errMsg != "NOERROR" );

        // ERROR
        result = db.fromJSON( "{\"id\":0,\"val\":\"eYELLOW\",locked:true}" );
        REQUIRE( result == false );

    }


    SECTION( "database" )
    {
        REQUIRE( db.add( *orange ) == false );

        REQUIRE( db.lookupById( ORANGE_ID ) == orange );
        REQUIRE( db.lookupById( APPLE_ID ) == apple );
        REQUIRE( db.lookupById( ORANGE_ID + 2 ) == nullptr );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
