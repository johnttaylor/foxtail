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
#include "Cpl/System/Api.h"
#include "Cpl/System/ElapsedTime.h"
#include "Fxt/System/ElapsedTime.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


#define SECT_     "_0test"
/// 
using namespace Fxt::System;



////////////////////////////////////////////////////////////////////////////////
static bool     useCpl_ = true;
static uint64_t mockNow_;
static uint64_t mockRealTimeNow_;
uint64_t Fxt::System::ElapsedTime::now() noexcept
{
    if ( useCpl_ )
    {
        return millisecondsToMicroseconds( Cpl::System::ElapsedTime::milliseconds() );
    }
    else
    {
        return mockNow_;
    }
}

uint64_t Fxt::System::ElapsedTime::nowInRealTime() noexcept
{
    if ( useCpl_ )
    {
        return millisecondsToMicroseconds( Cpl::System::ElapsedTime::millisecondsInRealTime() );
    }
    else
    {
        return mockRealTimeNow_;
    }
}


////////////////////////////////////////////////////////////////////////////////
#define DELAY_MS        2000
#define DELAY_SEC       2
#define DLEAY_MICROSEC  (2000*1000LL)


TEST_CASE( "elapsedtime"  )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Get current time
    uint64_t startTime = ElapsedTime::now();

    // Sleep at least 1.5 second
    Cpl::System::Api::sleep( DELAY_MS );
    uint64_t endTime      = ElapsedTime::now();
    uint64_t expectedDelta = DELAY_MS * 1000LL;

    // Verify delta time
    REQUIRE( ElapsedTime::delta( startTime, endTime ) >= expectedDelta );
    REQUIRE( ElapsedTime::expired( startTime, 1500*1000LL, endTime ) == true );

    // Verify helper functions
    REQUIRE( ElapsedTime::toMilliseconds( expectedDelta + 600LL ) == DELAY_MS );
    REQUIRE( ElapsedTime::toSeconds( expectedDelta + 600 * 1000LL ) == DELAY_SEC );
    REQUIRE( ElapsedTime::secondsToMicroseconds( DELAY_SEC  ) == DLEAY_MICROSEC );
    REQUIRE( ElapsedTime::millisecondsToMicroseconds( DELAY_MS ) == DLEAY_MICROSEC );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
