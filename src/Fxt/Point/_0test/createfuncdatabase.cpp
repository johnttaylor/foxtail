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
#include "Fxt/Point/CreateFuncDatabase.h"
#include "Fxt/Point/RegisterCreateFunc.h"
#include "Fxt/Point/Uint32.h"
#include "Fxt/Point/Int64.h"
#include "Fxt/Point/Bool.h"
#include "Cpl/System/Trace.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Point;

#define MAX_POINT_TYPES     3

static CreateFuncDatabase<MAX_POINT_TYPES> uut1_( "static constructor" );

static RegisterCreateFunc<Uint32> uint32_( uut1_ );
static RegisterCreateFunc<Int64>  int64_( uut1_ );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "CreateDatabase" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    SECTION( "lookup" )
    {
        Descriptor::CreateFunc_T funcPtr = uut1_.lookup( Bool::GUID_STRING );
        REQUIRE( funcPtr == nullptr );

        funcPtr = uut1_.lookup( Int64::GUID_STRING );
        REQUIRE( funcPtr == Int64::create );

        funcPtr = uut1_.lookup( Uint32::GUID_STRING );
        REQUIRE( funcPtr == Uint32::create );
    }

    SECTION( "create" )
    {
        CreateFuncDatabase<MAX_POINT_TYPES> uut2;
        static RegisterCreateFunc<Int64>    int64Func( uut2 );
        static RegisterCreateFunc<Bool>     boolFunc( uut2 );

        Descriptor::CreateFunc_T funcPtr = uut2.lookup( Bool::GUID_STRING );
        REQUIRE( funcPtr == Bool::create );

        funcPtr = uut2.lookup( Int64::GUID_STRING );
        REQUIRE( funcPtr == Int64::create );

        funcPtr = uut2.lookup( Uint32::GUID_STRING );
        REQUIRE( funcPtr == nullptr );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}