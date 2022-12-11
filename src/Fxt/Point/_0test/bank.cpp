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
#include "Cpl/System/Trace.h"
#include "Fxt/Point/Bank.h"
#include "Cpl/Memory/LeanHeap.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Point;

#define MAX_POINTS  4

static Descriptor       apple_(  0, Fxt::Point::Uint32::create );
static Descriptor       orange_(  1, Fxt::Point::Int64::create );
static Descriptor       cherry_(  2, Fxt::Point::Uint32::create );
static Descriptor       lime_(  3, Fxt::Point::Int64::create );

static Descriptor       apple2_( 7, Fxt::Point::Uint32::create );
static Descriptor       orange2_( 4, Fxt::Point::Int64::create );
static Descriptor       cherry2_( 5, Fxt::Point::Uint32::create );
static Descriptor       lime2_( 6, Fxt::Point::Int64::create );

static Descriptor       apple3_( 11, Fxt::Point::Uint32::create );
static Descriptor       orange3_( 8, Fxt::Point::Int64::create );
static Descriptor       cherry3_( 9, Fxt::Point::Uint32::create );
static Descriptor       lime3_( 10, Fxt::Point::Int64::create );

#define ELEM_SIZE_AS_SIZET(elemSize)    (((elemSize)+sizeof( size_t ) - 1) / sizeof(size_t))
#define MEM_SIZE        (ELEM_SIZE_AS_SIZET( sizeof(Fxt::Point::Uint32) ) * 2 + ELEM_SIZE_AS_SIZET( sizeof(Fxt::Point::Int64) ) * 2 )

static size_t           heapMemory_[MEM_SIZE];
static size_t           heapMemory2_[MEM_SIZE];
static size_t           heapMemory3_[MEM_SIZE];
static size_t           heapMemory4_[MEM_SIZE*3];
static size_t           heapMemory5_[MEM_SIZE*3];
static size_t           tempBuffer_[MEM_SIZE];

#define CMP(db,s,id)    (strcmp( db.lookupById(id)->getName(), s ) == 0)

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Bank" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap stateHeap1( heapMemory4_, sizeof( heapMemory4_ ) );
    Cpl::Memory::LeanHeap stateHeap2( heapMemory5_, sizeof( heapMemory5_ ) );

    SECTION( "create" )
    {
        Bank uut;
        Cpl::Memory::LeanHeap            heap( heapMemory_, sizeof( heapMemory_ ) );
        Fxt::Point::Database<MAX_POINTS> db;
        Descriptor*                      list[4 + 1] ={ &apple_, &orange_, &cherry_, &lime_, 0 };

        CPL_SYSTEM_TRACE_MSG( SECT_, ("Start section") );
        bool result = uut.populate( list, heap, db, stateHeap1 );
        REQUIRE( result );
        REQUIRE( CMP( db, "APPLE", 0 ) );
        REQUIRE( CMP( db, "ORANGE", 1 ) );
        REQUIRE( CMP( db, "CHERRY", 2 ) );
        REQUIRE( CMP( db, "LIME", 3 ) );
    }

    SECTION( "copy" )
    {
        Bank uut;
        Cpl::Memory::LeanHeap            heap( heapMemory_, sizeof( heapMemory_ ) );
        Fxt::Point::Database<MAX_POINTS> db;
        Descriptor*                      list[4 + 1] ={ &apple_, &orange_, &cherry_, &lime_, 0 };

        CPL_SYSTEM_TRACE_MSG( SECT_, ("Start section") );
        bool result = uut.populate( list, heap, db, stateHeap1 );
        REQUIRE( result );
        REQUIRE( CMP( db, "APPLE", 0 ) );
        REQUIRE( CMP( db, "ORANGE", 1 ) );
        REQUIRE( CMP( db, "CHERRY", 2 ) );
        REQUIRE( CMP( db, "LIME", 3 ) );

        size_t dstAllocatedLenInBytes;
        stateHeap1.getMemoryStart( dstAllocatedLenInBytes );
        REQUIRE( dstAllocatedLenInBytes == uut.getStatefulAllocatedSize() );
        REQUIRE( uut.copyStatefulMemoryTo( tempBuffer_, sizeof( tempBuffer_ ) ) );
        REQUIRE( memcmp( tempBuffer_, heapMemory4_, uut.getStatefulAllocatedSize() ) == 0 );
        REQUIRE( uut.copyStatefulMemoryTo( tempBuffer_+1, uut.getStatefulAllocatedSize() - 1 ) == false );
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
        Fxt::Point::Database<MAX_POINTS*3> db;
        Descriptor*                        list[4 + 1] ={ &apple_, &orange_, &cherry_, &lime_, 0 };
        Bank uut2;
        Cpl::Memory::LeanHeap heap2( heapMemory2_, sizeof( heapMemory2_ ) );
        Descriptor* list2[4 + 1] ={ &orange2_, &cherry2_, &lime2_, &apple2_, 0 };

        Bank uut3;
        Cpl::Memory::LeanHeap heap3( heapMemory3_, sizeof( heapMemory3_ ) );
        Descriptor* list3[3 + 1] ={ &orange3_, &cherry3_, &lime3_, 0 };

        CPL_SYSTEM_TRACE_MSG( SECT_, ("Start section") );
        bool result = uut.populate( list, heap, db, stateHeap1 );
        REQUIRE( result );
        REQUIRE( CMP( db, "APPLE", 0 ) );
        REQUIRE( CMP( db, "ORANGE", 1 ) );
        REQUIRE( CMP( db, "CHERRY", 2 ) );
        REQUIRE( CMP( db, "LIME", 3 ) );

        result = uut2.populate( list2, heap2, db, stateHeap2 );
        REQUIRE( result );
        REQUIRE( CMP( db, "ORANGE", 4 ) );
        REQUIRE( CMP( db, "CHERRY", 5 ) );
        REQUIRE( CMP( db, "LIME", 6 ) );
        REQUIRE( CMP( db, "APPLE", 7 ) );

        uut.copyStatefulMemoryFrom( uut2 );
        REQUIRE( memcmp( heapMemory4_, heapMemory5_, uut.getStatefulAllocatedSize() ) == 0 );

        result = uut3.populate( list3, heap3, db, stateHeap1 );
        REQUIRE( result );
        REQUIRE( CMP( db, "ORANGE", 8 ) );
        REQUIRE( CMP( db, "CHERRY", 9 ) );
        REQUIRE( CMP( db, "LIME", 10 ) );
        REQUIRE( uut2.copyStatefulMemoryFrom( uut3 ) == false );
    }

    SECTION( "db-out-of-space" )
    {
        Bank uut;
        Cpl::Memory::LeanHeap               heap( heapMemory_, sizeof( heapMemory_ ) );
        Fxt::Point::Database<MAX_POINTS-1>  db;
        Descriptor*                         list[4 + 1] ={ &apple_, &orange_, &cherry_, &lime_, 0 };

        CPL_SYSTEM_TRACE_MSG( SECT_, ("Start section") );
        bool result = uut.populate( list, heap, db, stateHeap1 );
        REQUIRE( result == false );
        REQUIRE( CMP( db, "APPLE", 0 ) );
        REQUIRE( CMP( db, "ORANGE", 1 ) );
        REQUIRE( CMP( db, "CHERRY", 2 ) );
        REQUIRE( db.lookupById( 3 ) == nullptr );
    }

    SECTION( "heap-out-of-space" )
    {
        Bank uut;
        Cpl::Memory::LeanHeap                 heap( heapMemory_, sizeof( heapMemory_ ) - sizeof( Fxt::Point::Uint32 ) );
        Fxt::Point::Database<MAX_POINTS - 1>  db;
        Descriptor*                           list[4 + 1] ={ &apple_, &orange_, &cherry_, &lime_, 0 };

        CPL_SYSTEM_TRACE_MSG( SECT_, ("Start section") );
        bool result = uut.populate( list, heap, db, stateHeap1 );
        REQUIRE( result == false );
        REQUIRE( CMP( db, "APPLE", 0 ) );
        REQUIRE( CMP( db, "ORANGE", 1 ) );
        REQUIRE( CMP( db, "CHERRY", 2 ) );
        REQUIRE( db.lookupById( 3 ) == nullptr );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}