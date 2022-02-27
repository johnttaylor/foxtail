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
#include "Cpl/Point/Uint32.h"
#include "Cpl/Point/Int64.h"
#include "Cpl/System/Trace.h"
#include "Fxt/Point/Bank.h"
#include "Cpl/Memory/LeanHeap.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Point;

#define MAX_POINTS  4

static Descriptor       apple_( "APPLE", 100, Cpl::Point::Uint32::create );
static Descriptor       orange_( "ORANGE", 200, Cpl::Point::Int64::create );
static Descriptor       cherry_( "CHERRY", 300, Cpl::Point::Uint32::create );
static Descriptor       lime_( "LIME", 400, Cpl::Point::Int64::create );

#define MEM_SIZE        (sizeof( Cpl::Point::Uint32 ) * 2 + sizeof( Cpl::Point::Int64 ) * 2 + (sizeof( size_t ) - 1)) / sizeof( size_t )
static size_t           heapMemory_[MEM_SIZE];
static size_t           heapMemory2_[MEM_SIZE];
static size_t           heapMemory3_[MEM_SIZE];
static size_t           tempBuffer_[MEM_SIZE];

#define CMP(db,s,id)    (strcmp( db.getSymbolicName(id), s ) == 0)

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Database" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    SECTION( "create" )
    {
        Bank uut;
        Cpl::Memory::LeanHeap heap( heapMemory_, sizeof( heapMemory_ ) );
        Fxt::Point::Database db( MAX_POINTS );
        Descriptor* list[4 + 1] ={ &apple_, &orange_, &cherry_, &lime_, 0 };

        bool result = uut.populate( list, heap, db );
        REQUIRE( result );
        REQUIRE( CMP( db, "APPLE", 0 ) );
        REQUIRE( CMP( db, "ORANGE", 1 ) );
        REQUIRE( CMP( db, "CHERRY", 2 ) );
        REQUIRE( CMP( db, "LIME", 3 ) );
    }

    SECTION( "copy" )
    {
        Bank uut;
        Cpl::Memory::LeanHeap heap( heapMemory_, sizeof( heapMemory_ ) );
        Fxt::Point::Database db( MAX_POINTS );
        Descriptor* list[4 + 1] ={ &apple_, &orange_, &cherry_, &lime_, 0 };

        bool result = uut.populate( list, heap, db );
        REQUIRE( result );
        REQUIRE( CMP( db, "APPLE", 0 ) );
        REQUIRE( CMP( db, "ORANGE", 1 ) );
        REQUIRE( CMP( db, "CHERRY", 2 ) );
        REQUIRE( CMP( db, "LIME", 3 ) );

        size_t dstAllocatedLenInBytes;
        heap.getMemoryStart( dstAllocatedLenInBytes );
        REQUIRE( dstAllocatedLenInBytes == uut.getAllocatedSize() );
        REQUIRE( uut.copyTo( tempBuffer_, sizeof( tempBuffer_ ) ) );
        REQUIRE( memcmp( tempBuffer_, heapMemory_, uut.getAllocatedSize() ) == 0 );
        REQUIRE( uut.copyTo( tempBuffer_+1, uut.getAllocatedSize() - 1 ) == false );
        REQUIRE( memcmp( tempBuffer_, heapMemory_, uut.getAllocatedSize() ) == 0 );

        memset( tempBuffer_, 0, sizeof( tempBuffer_ ) );
        uut.copyFrom( tempBuffer_, sizeof( tempBuffer_ ) );
        REQUIRE( memcmp( tempBuffer_, heapMemory_, uut.getAllocatedSize() ) == 0 );
        REQUIRE( uut.copyFrom( tempBuffer_ + 1, uut.getAllocatedSize() + 1 ) == false );
        REQUIRE( memcmp( tempBuffer_, heapMemory_, uut.getAllocatedSize() ) == 0 );
    }

    SECTION( "copy-bank" )
    {
        Bank uut;
        Cpl::Memory::LeanHeap heap( heapMemory_, sizeof( heapMemory_ ) );
        Fxt::Point::Database db( MAX_POINTS * 3 );
        Descriptor* list[4 + 1] ={ &apple_, &orange_, &cherry_, &lime_, 0 };
        Bank uut2;
        Cpl::Memory::LeanHeap heap2( heapMemory2_, sizeof( heapMemory2_ ) );
        Descriptor* list2[4 + 1] ={ &orange_, &cherry_, &lime_, &apple_, 0 };

        Bank uut3;
        Cpl::Memory::LeanHeap heap3( heapMemory3_, sizeof( heapMemory3_ ) );
        Descriptor* list3[3 + 1] ={ &orange_, &cherry_, &lime_, 0 };

        bool result = uut.populate( list, heap, db );
        REQUIRE( result );
        REQUIRE( CMP( db, "APPLE", 0 ) );
        REQUIRE( CMP( db, "ORANGE", 1 ) );
        REQUIRE( CMP( db, "CHERRY", 2 ) );
        REQUIRE( CMP( db, "LIME", 3 ) );

        result = uut2.populate( list2, heap2, db );
        REQUIRE( result );
        REQUIRE( CMP( db, "ORANGE", 4 ) );
        REQUIRE( CMP( db, "CHERRY", 5 ) );
        REQUIRE( CMP( db, "LIME", 6 ) );
        REQUIRE( CMP( db, "APPLE", 7 ) );

        uut.copyFrom( uut2 );
        REQUIRE( memcmp( heapMemory2_, heapMemory_, uut.getAllocatedSize() ) == 0 );

        result = uut3.populate( list3, heap3, db );
        REQUIRE( result );
        REQUIRE( CMP( db, "ORANGE", 8 ) );
        REQUIRE( CMP( db, "CHERRY", 9 ) );
        REQUIRE( CMP( db, "LIME", 10 ) );
        REQUIRE( uut2.copyFrom( uut3 ) == false );
    }

    SECTION( "db-out-of-space" )
    {
        Bank uut;
        Cpl::Memory::LeanHeap heap( heapMemory_, sizeof( heapMemory_ ) );
        Fxt::Point::Database db( MAX_POINTS-1 );
        Descriptor* list[4 + 1] ={ &apple_, &orange_, &cherry_, &lime_, 0 };

        bool result = uut.populate( list, heap, db );
        REQUIRE( result == false );
        REQUIRE( CMP( db, "APPLE", 0 ) );
        REQUIRE( CMP( db, "ORANGE", 1 ) );
        REQUIRE( CMP( db, "CHERRY", 2 ) );
        REQUIRE( db.getSymbolicName( 3 ) == nullptr );
    }

    SECTION( "heap-out-of-space" )
    {
        Bank uut;
        Cpl::Memory::LeanHeap heap( heapMemory_, sizeof( heapMemory_ ) - sizeof( Cpl::Point::Uint32 ) );
        Fxt::Point::Database db( MAX_POINTS - 1 );
        Descriptor* list[4 + 1] ={ &apple_, &orange_, &cherry_, &lime_, 0 };

        bool result = uut.populate( list, heap, db );
        REQUIRE( result == false );
        REQUIRE( CMP( db, "APPLE", 0 ) );
        REQUIRE( CMP( db, "ORANGE", 1 ) );
        REQUIRE( CMP( db, "CHERRY", 2 ) );
        REQUIRE( db.getSymbolicName( 3 ) == nullptr );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}