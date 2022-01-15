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
    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept { return "Cpl::Point::MyInt16Array::5"; }

protected:
    /// The points numeric identifier
    Id_T m_id;
};

#define MAX_POINTS  2

constexpr MyInt16Array::Id_T  appleId  = 0;
constexpr MyInt16Array::Id_T  orangeId = 1;

static const int16_t orangeInitVal_[NUM_ELEM] = { 1, 2, 3, 4, 5 };

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

    SECTION( "read" )
    {
        valid = orange_.read( value, NUM_ELEM  );
        REQUIRE( valid == true );
        REQUIRE( memcmp( value, orangeInitVal_, sizeof( value ) ) == 0 );

        valid = apple_.read( value, sizeof(value) );
        REQUIRE( valid == false );
        int16_t writeVal[NUM_ELEM] ={ 1, };
        apple_.write( writeVal, sizeof(writeVal)  );
        valid = apple_.read( value, sizeof( value ) );
        REQUIRE( valid );
        REQUIRE( memcmp( value, writeVal, sizeof( value ) ) == 0 );

        apple_.setInvalid();
        valid = apple_.read( value, sizeof( value ) );
        REQUIRE( valid == false );

        REQUIRE( apple_.getSize() == sizeof(int16_t) * NUM_ELEM);
    }

    SECTION( "write" )
    {
        int16_t writeVal[NUM_ELEM] = { 2,3,4,5,6 };
        apple_.write( writeVal, sizeof(writeVal), Api::eLOCK );
        valid = apple_.read( value, sizeof( value ) );
        REQUIRE( valid );
        REQUIRE( memcmp( value, writeVal, sizeof(value) ) == 0 );
    }

#if 0
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
        valid = apple_.get( value, NUM_ELEM , isValid, locked );
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
#endif

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
