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
#include "Fxt/Component/Digital/And16GateFactory.h"
#include "Fxt/Component/FactoryDatabase.h"
#include "Fxt/Point/Database.h"
#include "Fxt/Point/Uint8.h"
#include "Cpl/Memory/LeanHeap.h"
#include "Cpl/System/Trace.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Component;


static size_t generalHeap_[10000];
static size_t statefulHeap_[10000];


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "FactoryDatabase" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap statefulAllocator( statefulHeap_, sizeof( statefulHeap_ ) );
    FactoryDatabase uut;

    SECTION( "add" )
    {
        REQUIRE( uut.first() == nullptr );

        REQUIRE( uut.lookup( Fxt::Component::Digital::And16Gate::GUID_STRING ) == nullptr );

        Fxt::Component::Digital::And16GateFactory factory1( uut );
        REQUIRE( uut.first() == &factory1 );
        REQUIRE( uut.next( factory1 ) == nullptr );
        REQUIRE( uut.lookup( Fxt::Component::Digital::And16Gate::GUID_STRING ) == &factory1 );

        Fxt::Component::Digital::And16GateFactory factory2( uut );
        REQUIRE( uut.lookup( Fxt::Component::Digital::And16Gate::GUID_STRING ) == &factory1 );
        REQUIRE( uut.first() == &factory1 );
        REQUIRE( uut.next( factory1 ) == &factory2 );
        REQUIRE( uut.next( factory2 ) == nullptr );

        REQUIRE( uut.lookup( "00d33888-62c7-45b2-a4d4-9dbc55914ed3" ) == nullptr );

        uut.remove_( factory1 );
        REQUIRE( uut.first() == &factory2 );
        REQUIRE( uut.next( factory2 ) == nullptr );
        REQUIRE( uut.lookup( Fxt::Component::Digital::And16Gate::GUID_STRING ) == &factory2 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}