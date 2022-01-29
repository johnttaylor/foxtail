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

static size_t           heapMemory_[(sizeof( Cpl::Point::Uint32 ) * 2 + sizeof( Cpl::Point::Int64 ) * 2 + (sizeof(size_t)-1)) / sizeof(size_t)];

#define CMP(db,s,id)    (strcmp( db.getSymbolicName(id), s ) == 0)

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Database" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Bank uut;
    Cpl::Memory::LeanHeap heap( heapMemory_, sizeof( heapMemory_ ) );

    SECTION( "create" )
    {
        Cpl::Point::Database db( MAX_POINTS );
        Descriptor* list[4+1] = { &apple_, &orange_, &cherry_, &lime_, 0 };
        
        uint32_t pointId = 0;
        bool result = uut.populate( list, heap, db, pointId );
        REQUIRE( result );
        REQUIRE( CMP( db, "APPLE", 0 ) );
        REQUIRE( CMP( db, "ORANGE", 1 ) );
        REQUIRE( CMP( db, "CHERRY", 2 ) );
        REQUIRE( CMP( db, "LIME", 3 ) );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}