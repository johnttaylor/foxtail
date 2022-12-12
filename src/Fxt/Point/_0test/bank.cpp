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
#include "Fxt/Point/Uint32.h"
#include "Fxt/Point/Int64.h"
#include "Fxt/Point/Factory.h"
#include "Fxt/Point/FactoryDatabase.h"
#include "Cpl/System/Trace.h"
#include "Fxt/Point/Bank.h"
#include "Fxt/Type/Error.h"
#include "Cpl/Memory/LeanHeap.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Point;

#define MAX_POINTS  14

#define ELEM_SIZE_AS_SIZET(elemSize)    (((elemSize)+sizeof( size_t ) - 1) / sizeof(size_t))
#define MEM_SIZE        (ELEM_SIZE_AS_SIZET( sizeof(Fxt::Point::Uint32) ) * 3 + ELEM_SIZE_AS_SIZET( sizeof(Fxt::Point::Int64) ) * 3 )

static size_t           heapMemory_[MEM_SIZE];
static size_t           heapMemory2_[MEM_SIZE];
static size_t           heapMemory4_[MEM_SIZE * 3];
static size_t           heapMemory5_[MEM_SIZE * 3];
static size_t           tempBuffer_[MEM_SIZE];

#define JSON_APPLE      "{"\
                        "  \"id\":0," \
                        "  \"type\":\"99f91433-b8e4-4480-9203-8fcab5d9f20f\"" \
                        "}"
#define JSON_ORANGE     "{"\
                        "  \"id\":1," \
                        "  \"type\":\"49d79369-32d7-4f78-bba3-0fcb1ddff2a1\"" \
                        "}"
#define JSON_CHERRY     "{"\
                        "  \"id\":2," \
                        "  \"type\":\"99f91433-b8e4-4480-9203-8fcab5d9f20f\"" \
                        "}"
#define JSON_LIME      "{"\
                        "  \"id\":3," \
                        "  \"type\":\"49d79369-32d7-4f78-bba3-0fcb1ddff2a1\"" \
                        "}"

#define JSON_BANK       "[" \
                            JSON_APPLE "," \
                            JSON_ORANGE "," \
                            JSON_CHERRY "," \
                            JSON_LIME \
                        "]"

#define JSON_APPLE2     "{"\
                        "  \"id\":7," \
                        "  \"type\":\"99f91433-b8e4-4480-9203-8fcab5d9f20f\"," \
                        "  \"initial\": {" \
                        "     \"valid\":false," \
                        "     \"id\":8" \
                        "   }" \
                        "}"
#define JSON_ORANGE2    "{"\
                        "  \"id\":4," \
                        "  \"type\":\"49d79369-32d7-4f78-bba3-0fcb1ddff2a1\"" \
                        "}"
#define JSON_CHERRY2    "{"\
                        "  \"id\":5," \
                        "  \"type\":\"99f91433-b8e4-4480-9203-8fcab5d9f20f\"" \
                        "}"
#define JSON_LIME2      "{"\
                        "  \"id\":6," \
                        "  \"type\":\"49d79369-32d7-4f78-bba3-0fcb1ddff2a1\"," \
                        "  \"initial\": {" \
                        "     \"val\":42," \
                        "     \"id\":9" \
                        "   }" \
                        "}"

#define JSON_APPLE3     "{"\
                        "  \"id\":13," \
                        "  \"type\":\"99f91433-b8e4-4480-9203-8fcab5d9f20f\"" \
                        "}"
#define JSON_ORANGE3    "{"\
                        "  \"id\":10," \
                        "  \"type\":\"49d79369-32d7-4f78-bba3-0fcb1ddff2a1\"" \
                        "}"
#define JSON_CHERRY3    "{"\
                        "  \"id\":11," \
                        "  \"type\":\"99f91433-b8e4-4480-9203-8fcab5d9f20f\"" \
                        "}"
#define JSON_LIME3      "{"\
                        "  \"id\":12," \
                        "  \"type\":\"49d79369-32d7-4f78-bba3-0fcb1ddff2a1\"" \
                        "}"

#define JSON_BANK       "[" \
                            JSON_APPLE "," \
                            JSON_ORANGE "," \
                            JSON_CHERRY "," \
                            JSON_LIME \
                        "]"

#define JSON_BANK2      "[" \
                            JSON_APPLE2 "," \
                            JSON_ORANGE2 "," \
                            JSON_CHERRY2 "," \
                            JSON_LIME2 \
                        "]"

#define JSON_BANK3      "[" \
                            JSON_APPLE3 "," \
                            JSON_ORANGE3 "," \
                            JSON_CHERRY3"," \
                            JSON_LIME3 \
                        "]"


static FactoryDatabase      pointFactoryDb_;
static Factory<Int64>       int64Factory_( pointFactoryDb_ );
static Factory<Uint32>      uint32Factory_( pointFactoryDb_ );

static inline void writeUint32( DatabaseApi& db, uint32_t id, uint32_t val )
{
    Api* point = db.lookupById( id );
    ((Uint32*) point)->write( val );
}

static inline void writeInt64( DatabaseApi& db, uint32_t id, int64_t val )
{
    Api* point = db.lookupById( id );
    ((Int64*) point)->write( val );
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Bank" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap stateHeap1( heapMemory4_, sizeof( heapMemory4_ ) );
    Cpl::Memory::LeanHeap stateHeap2( heapMemory5_, sizeof( heapMemory5_ ) );
    Fxt::Type::Error pointErrorCode;


    SECTION( "create" )
    {
        Bank uut;
        Cpl::Memory::LeanHeap            heap( heapMemory_, sizeof( heapMemory_ ) );
        Fxt::Point::Database<MAX_POINTS> db;

        StaticJsonDocument<1024> doc;
        printf( "JSON IN[%s]\n", JSON_BANK );
        DeserializationError err = deserializeJson( doc, JSON_BANK );
        REQUIRE( err == DeserializationError::Ok );
        JsonArray list = doc.as<JsonArray>();
        for ( unsigned i=0; i < list.size(); i++ )
        {
            JsonObject json = list[i].as<JsonObject>();
            REQUIRE( uut.createPoint( pointFactoryDb_,
                                      json,
                                      pointErrorCode,
                                      heap,
                                      stateHeap1,
                                      db,
                                      "id" ) );
        }

        Api* point = db.lookupById( 0 );
        REQUIRE( !point->hasSetter() );
        REQUIRE( strcmp( point->getTypeGuid(), "99f91433-b8e4-4480-9203-8fcab5d9f20f" ) == 0 );

        point = db.lookupById( 1 );
        REQUIRE( !point->hasSetter() );
        REQUIRE( strcmp( point->getTypeGuid(), "49d79369-32d7-4f78-bba3-0fcb1ddff2a1" ) == 0 );

        point = db.lookupById( 2 );
        REQUIRE( !point->hasSetter() );
        REQUIRE( strcmp( point->getTypeGuid(), "99f91433-b8e4-4480-9203-8fcab5d9f20f" ) == 0 );

        point = db.lookupById( 3 );
        REQUIRE( !point->hasSetter() );
        REQUIRE( strcmp( point->getTypeGuid(), "49d79369-32d7-4f78-bba3-0fcb1ddff2a1" ) == 0 );

    }

    SECTION( "create2" )
    {
        Bank uut;
        Cpl::Memory::LeanHeap            heap( heapMemory_, sizeof( heapMemory_ ) );
        Fxt::Point::Database<MAX_POINTS> db;

        StaticJsonDocument<1024> doc;
        printf( "JSON IN[%s]\n", JSON_BANK2 );
        DeserializationError err = deserializeJson( doc, JSON_BANK2 );
        REQUIRE( err == DeserializationError::Ok );
        JsonArray list = doc.as<JsonArray>();
        for ( unsigned i=0; i < list.size(); i++ )
        {
            JsonObject json = list[i].as<JsonObject>();
            bool result = uut.createPoint( pointFactoryDb_,
                                           json,
                                           pointErrorCode,
                                           heap,
                                           stateHeap1,
                                           db,
                                           "id" );
            printf( "errCode=%08X\n", pointErrorCode.errVal );
            REQUIRE( result );
        }

        Api* point = db.lookupById( 7 );
        REQUIRE( point->hasSetter() );
        REQUIRE( strcmp( point->getTypeGuid(), "99f91433-b8e4-4480-9203-8fcab5d9f20f" ) == 0 );
        REQUIRE( point->isNotValid() );
        point->updateFromSetter();
        REQUIRE( point->isNotValid() );

        point = db.lookupById( 4 );
        REQUIRE( !point->hasSetter() );
        REQUIRE( strcmp( point->getTypeGuid(), "49d79369-32d7-4f78-bba3-0fcb1ddff2a1" ) == 0 );

        point = db.lookupById( 5 );
        REQUIRE( !point->hasSetter() );
        REQUIRE( strcmp( point->getTypeGuid(), "99f91433-b8e4-4480-9203-8fcab5d9f20f" ) == 0 );

        point = db.lookupById( 6 );
        REQUIRE( point->hasSetter() );
        REQUIRE( strcmp( point->getTypeGuid(), "49d79369-32d7-4f78-bba3-0fcb1ddff2a1" ) == 0 );
        REQUIRE( point->isNotValid() );
        point->updateFromSetter();
        Int64* pt = (Int64*) point;
        int64_t val;
        REQUIRE( pt->read( val ) );
        REQUIRE( val == 42 );

        REQUIRE( db.lookupById( 8 ) );
        REQUIRE( db.lookupById( 9 ) );
    }

    SECTION( "copy" )
    {
        Bank uut;
        Cpl::Memory::LeanHeap            heap( heapMemory_, sizeof( heapMemory_ ) );
        Fxt::Point::Database<MAX_POINTS> db;

        // Populate the Bank
        StaticJsonDocument<1024> doc;
        printf( "JSON IN[%s]\n", JSON_BANK );
        DeserializationError err = deserializeJson( doc, JSON_BANK );
        REQUIRE( err == DeserializationError::Ok );
        JsonArray list = doc.as<JsonArray>();
        for ( unsigned i=0; i < list.size(); i++ )
        {
            JsonObject json = list[i].as<JsonObject>();
            REQUIRE( uut.createPoint( pointFactoryDb_,
                                      json,
                                      pointErrorCode,
                                      heap,
                                      stateHeap1,
                                      db,
                                      "id" ) );
        }

        size_t dstAllocatedLenInBytes;
        stateHeap1.getMemoryStart( dstAllocatedLenInBytes );
        REQUIRE( dstAllocatedLenInBytes == uut.getStatefulAllocatedSize() );
        REQUIRE( uut.copyStatefulMemoryTo( tempBuffer_, sizeof( tempBuffer_ ) ) );
        REQUIRE( memcmp( tempBuffer_, heapMemory4_, uut.getStatefulAllocatedSize() ) == 0 );
        REQUIRE( uut.copyStatefulMemoryTo( tempBuffer_ + 1, uut.getStatefulAllocatedSize() - 1 ) == false );
        REQUIRE( memcmp( tempBuffer_, heapMemory4_, uut.getStatefulAllocatedSize() ) == 0 );

        memset( tempBuffer_, 0, sizeof( tempBuffer_ ) );
        uut.copyStatefulMemoryFrom( tempBuffer_, sizeof( tempBuffer_ ) );
        REQUIRE( memcmp( tempBuffer_, heapMemory4_, uut.getStatefulAllocatedSize() ) == 0 );
        REQUIRE( uut.copyStatefulMemoryFrom( tempBuffer_ + 1, uut.getStatefulAllocatedSize() + 1 ) == false );
        REQUIRE( memcmp( tempBuffer_, heapMemory4_, uut.getStatefulAllocatedSize() ) == 0 );
    }

    SECTION( "copy-bank" )
    {
        Bank uut;
        Cpl::Memory::LeanHeap              heap( heapMemory_, sizeof( heapMemory_ ) );
        Fxt::Point::Database<MAX_POINTS * 3> db;

        // Populate the Bank/UUT1
        StaticJsonDocument<1024> doc;
        printf( "JSON IN[%s]\n", JSON_BANK );
        DeserializationError err = deserializeJson( doc, JSON_BANK );
        REQUIRE( err == DeserializationError::Ok );
        JsonArray list = doc.as<JsonArray>();
        for ( unsigned i=0; i < list.size(); i++ )
        {
            JsonObject json = list[i].as<JsonObject>();
            REQUIRE( uut.createPoint( pointFactoryDb_,
                                      json,
                                      pointErrorCode,
                                      heap,
                                      stateHeap1,
                                      db,
                                      "id" ) );
        }

        // Populate Bank/UUT2
        Bank uut2;
        Cpl::Memory::LeanHeap heap2( heapMemory2_, sizeof( heapMemory2_ ) );
        doc.clear();
        printf( "JSON IN[%s]\n", JSON_BANK2 );
        err = deserializeJson( doc, JSON_BANK2 );
        REQUIRE( err == DeserializationError::Ok );
        list = doc.as<JsonArray>();
        for ( unsigned i=0; i < list.size(); i++ )
        {
            JsonObject json = list[i].as<JsonObject>();
            bool result = uut2.createPoint( pointFactoryDb_,
                                            json,
                                            pointErrorCode,
                                            heap2,
                                            stateHeap2,
                                            db,
                                            "id" );
            printf( "errCode=%08X\n", pointErrorCode.errVal );
            REQUIRE( result );
        }

        REQUIRE( uut.getStatefulAllocatedSize() < uut2.getStatefulAllocatedSize() );

        // Update the source bank
        writeUint32( db, 0, 1 ); // Bank1
        writeInt64( db, 1, 2 );
        writeUint32( db, 2, 3 );
        writeInt64( db, 3, 4 );

        REQUIRE( uut.copyStatefulMemoryFrom( uut2 ) == false );

        // Should fail because Bank2 has setter's
        REQUIRE( memcmp( heapMemory4_, heapMemory5_, uut2.getStatefulAllocatedSize() ) != 0 );
    }
    SECTION( "copy-bank2" )
    {
        Bank uut;
        Cpl::Memory::LeanHeap              heap( heapMemory_, sizeof( heapMemory_ ) );
        Fxt::Point::Database<MAX_POINTS * 3> db;

        // Populate the Bank/UUT1
        StaticJsonDocument<1024> doc;
        printf( "JSON IN[%s]\n", JSON_BANK );
        DeserializationError err = deserializeJson( doc, JSON_BANK );
        REQUIRE( err == DeserializationError::Ok );
        JsonArray list = doc.as<JsonArray>();
        for ( unsigned i=0; i < list.size(); i++ )
        {
            JsonObject json = list[i].as<JsonObject>();
            REQUIRE( uut.createPoint( pointFactoryDb_,
                                      json,
                                      pointErrorCode,
                                      heap,
                                      stateHeap1,
                                      db,
                                      "id" ) );
        }

        // Populate Bank/UUT
        Bank uut2;
        Cpl::Memory::LeanHeap heap2( heapMemory2_, sizeof( heapMemory2_ ) );
        doc.clear();
        printf( "JSON IN[%s]\n", JSON_BANK3 );
        err = deserializeJson( doc, JSON_BANK3 );
        REQUIRE( err == DeserializationError::Ok );
        list = doc.as<JsonArray>();
        for ( unsigned i=0; i < list.size(); i++ )
        {
            JsonObject json = list[i].as<JsonObject>();
            bool result = uut2.createPoint( pointFactoryDb_,
                                            json,
                                            pointErrorCode,
                                            heap2,
                                            stateHeap2,
                                            db,
                                            "id" );
            printf( "errCode=%08X\n", pointErrorCode.errVal );
            REQUIRE( result );
        }

        REQUIRE( uut.getStatefulAllocatedSize() == uut2.getStatefulAllocatedSize() );

        // Update the source bank
        writeUint32( db, 0, 1 ); // Bank1
        writeInt64( db, 1, 2 );
        writeUint32( db, 2, 3 );
        writeInt64( db, 3, 4 );

        // Should pass because Bank1 and Bank3 are the 'same' structure/points
        REQUIRE( uut.copyStatefulMemoryFrom( uut2 ) );
        REQUIRE( memcmp( heapMemory4_, heapMemory5_, uut2.getStatefulAllocatedSize() ) == 0 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}