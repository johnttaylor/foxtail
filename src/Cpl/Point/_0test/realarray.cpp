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
#include "Cpl/Math/real.h"
#include "Cpl/Memory/HPool.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Cpl::Point;

#define NUM_ELEM    5

// Test Class
class MyRealArray : public RealArray_<NUM_ELEM, double>
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
    inline MyRealArray::Id_T getId() const noexcept { return m_id; }

public:
    /** Constructor. Invalid Point.
     */
    MyRealArray( const Id_T myIdentifier ) : RealArray_<NUM_ELEM, double>(), m_id( myIdentifier ) {}

    /// Constructor. Valid Point.  Requires an initial value
    MyRealArray( const Id_T myIdentifier, const double initialSrcData[NUM_ELEM] ) :RealArray_<NUM_ELEM, double>( initialSrcData ), m_id( myIdentifier ) {}

public:
    /// Pull in overloaded methods from base class
    using RealArray_<NUM_ELEM, double>::write;

    /// Updates the MP's data from 'src'
    virtual void write( MyRealArray& src, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST ) noexcept
    {
        if ( src.isNotValid() )
        {
            setInvalid();
        }
        else
        {
            RealArray_<NUM_ELEM, double>::write( src.m_array, NUM_ELEM, lockRequest );
        }
    }

public:
    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept { return "Cpl::Point::MyRealArray::5"; }

    ///  See Cpl::Point::Api
    size_t getTotalSize() const noexcept { return sizeof( MyRealArray ); }
    
    /// Creates a concrete instance in the invalid state
    static Api* create( Cpl::Memory::Allocator& allocatorForPoints, uint32_t pointId ) { return new(allocatorForPoints.allocate( sizeof( MyRealArray ) )) MyRealArray( pointId ); }

protected:
    /// The points numeric identifier
    Id_T m_id;
};

#define MAX_POINTS  2

constexpr MyRealArray::Id_T  appleId  = 0;
constexpr MyRealArray::Id_T  orangeId = 1;

static const double orangeInitVal_[NUM_ELEM] ={ 1, 2, 3, 4, 5 };

static MyRealArray apple_( appleId );
static MyRealArray orange_( orangeId, orangeInitVal_ );

static bool compareArray( const double* a1, const double* a2, size_t num_elems )
{
    for ( size_t i=0; i < num_elems; i++ )
    {
        if ( Cpl::Math::areDoublesEqual( a1[i], a2[i] ) == false )
        {
            return false;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "MyRealArray" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Database db( MAX_POINTS );
    Info_T info ={ &apple_, "APPLE" };
    REQUIRE( db.add( appleId, info ) );
    info ={ &orange_, "ORANGE" };
    REQUIRE( db.add( orangeId, info ) );
    bool valid;
    double value[NUM_ELEM];


    SECTION( "create" )
    {
        Cpl::Memory::HPool<MyRealArray> heap( 1 );
        Api* pt = MyRealArray::create( heap, 0 );
        REQUIRE( pt );
        REQUIRE( strcmp( pt->getTypeAsText(), apple_.getTypeAsText() ) == 0 );

        REQUIRE( apple_.getTotalSize() == sizeof( apple_ ) );
    }

    SECTION( "read" )
    {
        valid = orange_.read( value, NUM_ELEM );
        REQUIRE( valid == true );
        REQUIRE( compareArray( value, orangeInitVal_, NUM_ELEM ) );

        valid = apple_.read( value, sizeof( value ) );
        REQUIRE( valid == false );
        double writeVal[NUM_ELEM] ={ 1, };
        apple_.write( writeVal, NUM_ELEM );
        valid = apple_.read( value, sizeof( value ) );
        REQUIRE( valid );
        REQUIRE( compareArray( value, writeVal, NUM_ELEM ) );

        apple_.setInvalid();
        valid = apple_.read( value, sizeof( value ) );
        REQUIRE( valid == false );

        REQUIRE( apple_.getSize() == sizeof( double ) * NUM_ELEM );
    }

    SECTION( "write" )
    {
        double writeVal[NUM_ELEM] ={ 2,3,4,5,6 };
        apple_.write( writeVal, NUM_ELEM, Api::eLOCK );
        valid = apple_.read( value, sizeof( value ) );
        REQUIRE( valid );
        REQUIRE( compareArray( value, writeVal, NUM_ELEM ) );

        double writePartial[3] ={ 20,30,40 };
        double expectedVal[NUM_ELEM] ={ 2, 20, 30, 40, 6 };
        apple_.write( writePartial, 3, Api::eUNLOCK, 1 );
        valid = apple_.read( value, sizeof( value ) );
        REQUIRE( valid );
        REQUIRE( compareArray( value, expectedVal, NUM_ELEM ) );
    }

    SECTION( "write2" )
    {
        apple_.write( orange_, Api::eLOCK );
        valid = apple_.read( value, sizeof( value ) );
        REQUIRE( compareArray( value, orangeInitVal_, NUM_ELEM ) );
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
        double writeVal[NUM_ELEM] ={ 12,13,14,15,16 };
        apple_.write( writeVal, NUM_ELEM );
        bool isValid;
        bool locked;
        valid = apple_.get( value, sizeof( value ), isValid, locked );
        REQUIRE( compareArray( value, writeVal, NUM_ELEM ) );
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
        double writeVal[NUM_ELEM] ={ 112.01,113,114,115,116.6787 };
        apple_.write( writeVal, NUM_ELEM );
        bool result = db.toJSON( appleId, buffer, sizeof( buffer ), truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", buffer) );
        REQUIRE( result );
        REQUIRE( truncated == false );
        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, buffer );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( Cpl::Math::areDoublesEqual( doc["val"]["elems"][0], 112.01 ) );
        REQUIRE( Cpl::Math::areDoublesEqual( doc["val"]["elems"][1], 113 ) );
        REQUIRE( Cpl::Math::areDoublesEqual( doc["val"]["elems"][2], 114 ) );
        REQUIRE( Cpl::Math::areDoublesEqual( doc["val"]["elems"][3], 115 ) );
        REQUIRE( Cpl::Math::areDoublesEqual( doc["val"]["elems"][4], 116.6787 ) );

        result = db.fromJSON( "{id:0,val:{startIdx:0,elems:[50.5,40.4,30.3,20.2,10.1]}}" );
        REQUIRE( result );
        valid  = apple_.read( value, NUM_ELEM );
        double expectedValue[NUM_ELEM] ={ 50.5,40.4,30.3,20.2,10.1 };
        REQUIRE( valid );
        REQUIRE( compareArray( value, expectedValue, NUM_ELEM ) );

        result = db.fromJSON( "{\"id\":0,\"val\":123}" );
        REQUIRE( result == false );
        valid  = apple_.read( value, NUM_ELEM );
        REQUIRE( valid );
        REQUIRE( compareArray( value, expectedValue, NUM_ELEM ) );

        result = db.fromJSON( "{id:0,val:{startIdx:3,elems:[55,44.32]}}" );
        REQUIRE( result );
        valid  = apple_.read( value, NUM_ELEM );
        double expectedValue2[NUM_ELEM] ={ 50.5,40.4,30.3, 55, 44.32 };
        REQUIRE( valid );
        REQUIRE( compareArray( value, expectedValue2, NUM_ELEM ) );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
