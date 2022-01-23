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
#include "Cpl/Point/Array_.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Memory/HPool.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Cpl::Point;

#define NUM_ELEM    5

// Test Class
class MyInt16Array : public IntegerArray_<NUM_ELEM, int16_t>
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
    inline MyInt16Array::Id_T getId() const noexcept { return m_id; }

public:
    /** Constructor. Invalid Point.
     */
    MyInt16Array( const Id_T myIdentifier ) : IntegerArray_<NUM_ELEM, int16_t>(), m_id( myIdentifier ) {}

    /// Constructor. Valid Point.  Requires an initial value
    MyInt16Array( const Id_T myIdentifier, const int16_t initialSrcData[NUM_ELEM] ) :IntegerArray_<NUM_ELEM, int16_t>( initialSrcData ), m_id( myIdentifier ) {}

public:
    /// Pull in overloaded methods from base class
    using IntegerArray_<NUM_ELEM, int16_t>::write;

    /// Updates the MP's data from 'src'
    virtual void write( MyInt16Array& src, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST ) noexcept 
    {
        if ( src.isNotValid() )
        {
            setInvalid();
        }
        else
        {
            IntegerArray_<NUM_ELEM, int16_t>::write( src.m_array, NUM_ELEM, lockRequest );
        }
    }


public:
    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept { return "Cpl::Point::MyInt16Array::5"; }

    ///  See Cpl::Point::Api
    size_t getTotalSize() const noexcept { return sizeof( MyInt16Array); }
    
    /// Creates a concrete instance in the invalid state
    static Api* create( Cpl::Memory::Allocator& allocatorForPoints, uint32_t pointId ) { return new(allocatorForPoints.allocate( sizeof( MyInt16Array ) )) MyInt16Array( pointId ); }

protected:
    /// The points numeric identifier
    Id_T m_id;
};

#define MAX_POINTS  2

constexpr MyInt16Array::Id_T  appleId  = 0;
constexpr MyInt16Array::Id_T  orangeId = 1;

static const int16_t orangeInitVal_[NUM_ELEM] ={ 1, 2, 3, 4, 5 };

static MyInt16Array apple_( appleId );
static MyInt16Array orange_( orangeId, orangeInitVal_ );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "MyInt16Array" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Database db( MAX_POINTS );
    Info_T info ={ &apple_, "APPLE" };
    REQUIRE( db.add( appleId, info ) );
    info ={ &orange_, "ORANGE" };
    REQUIRE( db.add( orangeId, info ) );
    bool valid;
    int16_t value[NUM_ELEM];

    SECTION( "create" )
    {
        Cpl::Memory::HPool<MyInt16Array> heap( 1 );
        Api* pt = MyInt16Array::create( heap, 0 );
        REQUIRE( pt );
        REQUIRE( strcmp( pt->getTypeAsText(), apple_.getTypeAsText() ) == 0 );

        REQUIRE( apple_.getTotalSize() == sizeof( apple_ ) );
    }

    SECTION( "read" )
    {
        valid = orange_.read( value, NUM_ELEM );
        REQUIRE( valid == true );
        REQUIRE( memcmp( value, orangeInitVal_, sizeof( value ) ) == 0 );

        valid = apple_.read( value, sizeof( value ) );
        REQUIRE( valid == false );
        int16_t writeVal[NUM_ELEM] ={ 1, };
        apple_.write( writeVal, NUM_ELEM );
        valid = apple_.read( value, sizeof( value ) );
        REQUIRE( valid );
        REQUIRE( memcmp( value, writeVal, sizeof( value ) ) == 0 );

        apple_.setInvalid();
        valid = apple_.read( value, sizeof( value ) );
        REQUIRE( valid == false );

        REQUIRE( apple_.getSize() == sizeof( int16_t ) * NUM_ELEM );
    }

    SECTION( "write" )
    {
        int16_t writeVal[NUM_ELEM] ={ 2,3,4,5,6 };
        apple_.write( writeVal, NUM_ELEM, Api::eLOCK );
        valid = apple_.read( value, sizeof( value ) );
        REQUIRE( valid );
        REQUIRE( memcmp( value, writeVal, sizeof( value ) ) == 0 );

        int16_t writePartial[3] ={ 20,30,40 };
        int16_t expectedVal[NUM_ELEM] ={ 2, 20, 30, 40, 6 };
        apple_.write( writePartial, 3, Api::eUNLOCK, 1 );
        valid = apple_.read( value, sizeof( value ) );
        REQUIRE( valid );
        REQUIRE( memcmp( value, expectedVal, sizeof( value ) ) == 0 );
    }

    SECTION( "write2" )
    {
        apple_.write( orange_, Api::eLOCK );
        valid = apple_.read( value, sizeof( value ) );
        REQUIRE( memcmp( value, orangeInitVal_, sizeof( value ) ) == 0 );
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
        int16_t writeVal[NUM_ELEM] ={ 12,13,14,15,16 };
        apple_.write( writeVal, NUM_ELEM );
        bool isValid;
        bool locked;
        valid = apple_.get( value, sizeof( value ), isValid, locked );
        REQUIRE( memcmp( value, writeVal, sizeof( value ) ) == 0 );
        REQUIRE( valid == true );
        REQUIRE( locked == false );
        apple_.setInvalid( Api::eLOCK );
        valid = apple_.get( value, sizeof( value ), isValid, locked );
        REQUIRE( valid == false );
        REQUIRE( locked == true );
        apple_.setLockState( Api::eUNLOCK );
    }

    SECTION( "json" )
    {
        char buffer[256];
        bool truncated;
        int16_t writeVal[NUM_ELEM] ={ 112,113,114,115,116 };
        apple_.write( writeVal, NUM_ELEM );
        bool result = db.toJSON( appleId, buffer, sizeof( buffer ), truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( result );
        REQUIRE( truncated == false );
        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, buffer );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( strcmp( doc["val"]["elems"][0], "0x70" ) == 0 );
        REQUIRE( strcmp( doc["val"]["elems"][1], "0x71" ) == 0 );
        REQUIRE( strcmp( doc["val"]["elems"][2], "0x72" ) == 0 );
        REQUIRE( strcmp( doc["val"]["elems"][3], "0x73" ) == 0 );
        REQUIRE( strcmp( doc["val"]["elems"][4], "0x74" ) == 0 );

        result = db.fromJSON( "{id:0,val:{startIdx:0,elems:[\"50\",\"40\",\"30\",\"20\",\"10\"]}}" );
        REQUIRE( result );
        valid  = apple_.read( value, NUM_ELEM );
        int16_t expectedValue[NUM_ELEM] ={ 0x50, 0x40, 0x30, 0x20, 0x10 };
        REQUIRE( valid );
        REQUIRE( memcmp( value, expectedValue, sizeof( value ) ) == 0 );

        result = db.fromJSON( "{\"id\":0,\"val\":123}" );
        REQUIRE( result == false );
        valid  = apple_.read( value, NUM_ELEM );
        REQUIRE( valid );
        REQUIRE( memcmp( value, expectedValue, sizeof( value ) ) == 0 );

        result = db.fromJSON( "{id:0,val:{startIdx:3,elems:[\"55\",\"44\"]}}" );
        REQUIRE( result );
        valid  = apple_.read( value, NUM_ELEM );
        int16_t expectedValue2[NUM_ELEM] ={ 0x50, 0x40, 0x30, 0x55, 0x44};
        REQUIRE( valid );
        REQUIRE( memcmp( value, expectedValue2, sizeof( value ) ) == 0 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
