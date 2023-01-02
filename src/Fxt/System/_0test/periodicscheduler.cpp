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
#include "Fxt/System/PeriodicScheduler.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


#define SECT_     "_0test"
/// 
using namespace Fxt::System;



////////////////////////////////////////////////////////////////////////////////

static inline void displayInfo( const char* label, unsigned count, uint64_t currentTick, uint64_t intervalTick, void* context )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("%-7s: %d, tick=%llu, interval=%llu, context=%p",
                                   label,
                                   count,
                                   currentTick,
                                   intervalTick,
                                   context) );
}


/// Mock Interval for testing
class Interval : public Fxt::System::PeriodApi
{
public:
    uint64_t m_lastCurrentTick;
    uint64_t m_lastCurrentInterval;
    unsigned m_count;
    bool     m_result;

public:
    /// Constructor
    Interval( uint64_t duration )
        : m_lastCurrentTick( 0 )
        , m_lastCurrentInterval( 0 )
        , m_count( 0 )
        , m_result( true )
    {
        m_duration = duration;
    }

public:
    /// Execute function
    bool execute( uint64_t currentTick, uint64_t currentInterval ) noexcept
    {
        m_count++;
        m_lastCurrentTick     = currentTick;
        m_lastCurrentInterval = currentInterval;
        return m_result;
    }
};

static unsigned     slippageCount_;
static uint64_t     slippageLastCurrentTick_;
static uint64_t     slippageLastMissedInterval_;
static PeriodApi*   slippageLastPeriod_;
static bool         slippageResult_ = true;
static bool reportSlippage( PeriodApi& periodThatSlipped, uint64_t currentTick, uint64_t missedInterval )
{
    slippageCount_++;
    slippageLastCurrentTick_     = currentTick;
    slippageLastMissedInterval_  = missedInterval;
    slippageLastPeriod_          = &periodThatSlipped;
    displayInfo( "SLIPPAGE", slippageCount_, slippageLastCurrentTick_, slippageLastMissedInterval_, &periodThatSlipped );
    return slippageResult_;
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "PeriodicScheduler" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Interval applePeriod( 10 * 1000LL );
    Interval orangePeriod( 20 * 1000LL );
    Interval cherryPeriod( 7 * 1000LL );
    PeriodApi* intervals[] =
    {
        &applePeriod,
        &orangePeriod,
        &cherryPeriod,
        nullptr
    };

    slippageCount_                 = 0;
    slippageLastCurrentTick_       = 0;
    slippageLastMissedInterval_    = 0;
    slippageLastPeriod_            = nullptr;
    slippageResult_                = true;

    uint64_t currentTick = 0;

    SECTION( "no slippage reporting" )
    {
        PeriodicScheduler uut;
        uint64_t intervalTime;

        currentTick = 5 * 1000LL;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %llu", currentTick) );
        uut.start( intervals );
        uut.executeScheduler( currentTick );

        REQUIRE( applePeriod.m_count == 0 );
        REQUIRE( orangePeriod.m_count == 0 );
        REQUIRE( cherryPeriod.m_count == 0 );
        currentTick += 5 * 1000LL;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %llu", currentTick) );
        uut.executeScheduler( currentTick );

        displayInfo( "APPLE", applePeriod.m_count, applePeriod.m_lastCurrentTick, applePeriod.m_lastCurrentInterval, &applePeriod );
        displayInfo( "ORANGE", orangePeriod.m_count, orangePeriod.m_lastCurrentTick, orangePeriod.m_lastCurrentInterval, &orangePeriod );
        displayInfo( "CHERRY", cherryPeriod.m_count, cherryPeriod.m_lastCurrentTick, cherryPeriod.m_lastCurrentInterval, &cherryPeriod );
        intervalTime = 10 * 1000LL;
        REQUIRE( applePeriod.m_count == 1 );
        REQUIRE( applePeriod.m_lastCurrentTick == currentTick );
        REQUIRE( applePeriod.m_lastCurrentInterval == intervalTime );
        REQUIRE( orangePeriod.m_count == 0 );
        intervalTime = 7 * 1000LL;
        REQUIRE( cherryPeriod.m_count == 1 );
        REQUIRE( cherryPeriod.m_lastCurrentTick == currentTick );
        REQUIRE( cherryPeriod.m_lastCurrentInterval == intervalTime );

        currentTick += 5 * 1000LL;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %llu", currentTick) );
        uut.executeScheduler( currentTick );

        displayInfo( "APPLE", applePeriod.m_count, applePeriod.m_lastCurrentTick, applePeriod.m_lastCurrentInterval, &applePeriod );
        displayInfo( "ORANGE", orangePeriod.m_count, orangePeriod.m_lastCurrentTick, orangePeriod.m_lastCurrentInterval, &orangePeriod );
        displayInfo( "CHERRY", cherryPeriod.m_count, cherryPeriod.m_lastCurrentTick, cherryPeriod.m_lastCurrentInterval, &cherryPeriod );
        REQUIRE( applePeriod.m_count == 1 );
        REQUIRE( orangePeriod.m_count == 0 );
        intervalTime = 14 * 1000LL;
        REQUIRE( cherryPeriod.m_count == 2 );
        REQUIRE( cherryPeriod.m_lastCurrentTick == currentTick );
        REQUIRE( cherryPeriod.m_lastCurrentInterval == intervalTime );

        currentTick += 5 * 1000LL;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %llu", currentTick) );
        uut.executeScheduler( currentTick );

        displayInfo( "APPLE", applePeriod.m_count, applePeriod.m_lastCurrentTick, applePeriod.m_lastCurrentInterval, &applePeriod );
        displayInfo( "ORANGE", orangePeriod.m_count, orangePeriod.m_lastCurrentTick, orangePeriod.m_lastCurrentInterval, &orangePeriod );
        displayInfo( "CHERRY", cherryPeriod.m_count, cherryPeriod.m_lastCurrentTick, cherryPeriod.m_lastCurrentInterval, &cherryPeriod );
        intervalTime = 20 * 1000LL;
        REQUIRE( applePeriod.m_count == 2 );
        REQUIRE( applePeriod.m_lastCurrentTick == currentTick );
        REQUIRE( applePeriod.m_lastCurrentInterval == intervalTime );
        intervalTime = 20 * 1000LL;
        REQUIRE( orangePeriod.m_count == 1 );
        REQUIRE( orangePeriod.m_lastCurrentTick == currentTick );
        REQUIRE( orangePeriod.m_lastCurrentInterval == intervalTime );
        REQUIRE( cherryPeriod.m_count == 2 );

        currentTick += 1 * 1000LL;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %llu", currentTick) );
        uut.executeScheduler( currentTick );
        displayInfo( "APPLE", applePeriod.m_count, applePeriod.m_lastCurrentTick, applePeriod.m_lastCurrentInterval, &applePeriod );
        displayInfo( "ORANGE", orangePeriod.m_count, orangePeriod.m_lastCurrentTick, orangePeriod.m_lastCurrentInterval, &orangePeriod );
        displayInfo( "CHERRY", cherryPeriod.m_count, cherryPeriod.m_lastCurrentTick, cherryPeriod.m_lastCurrentInterval, &cherryPeriod );
        REQUIRE( applePeriod.m_count == 2 );
        REQUIRE( orangePeriod.m_count == 1 );
        intervalTime = 21 * 1000LL;
        REQUIRE( cherryPeriod.m_count == 3 );
        REQUIRE( cherryPeriod.m_lastCurrentTick == currentTick );
        REQUIRE( cherryPeriod.m_lastCurrentInterval == intervalTime );

        // Verify scheduler stops when an error is encountered
        currentTick += 100 * 1000LL;
        applePeriod.m_result = false;
        uut.executeScheduler( currentTick );
        REQUIRE( applePeriod.m_count == 3 );
        REQUIRE( orangePeriod.m_count == 1 );
        REQUIRE( cherryPeriod.m_count == 3 );

        uut.stop();
    }

    SECTION( "Slippage" )
    {
        PeriodicScheduler uut( reportSlippage );
        uint64_t intervalTime;

        currentTick = 5 * 1000LL;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %llu", currentTick) );
        uut.start( intervals );
        uut.executeScheduler( currentTick );

        displayInfo( "APPLE", applePeriod.m_count, applePeriod.m_lastCurrentTick, applePeriod.m_lastCurrentInterval, &applePeriod );
        displayInfo( "ORANGE", orangePeriod.m_count, orangePeriod.m_lastCurrentTick, orangePeriod.m_lastCurrentInterval, &orangePeriod );
        displayInfo( "CHERRY", cherryPeriod.m_count, cherryPeriod.m_lastCurrentTick, cherryPeriod.m_lastCurrentInterval, &cherryPeriod );
        REQUIRE( applePeriod.m_count == 0 );
        REQUIRE( orangePeriod.m_count == 0 );
        REQUIRE( cherryPeriod.m_count == 0 );
        REQUIRE( slippageCount_ == 0 );

        currentTick += 5 * 1000LL;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %llu", currentTick) );
        uut.executeScheduler( currentTick );
        displayInfo( "APPLE", applePeriod.m_count, applePeriod.m_lastCurrentTick, applePeriod.m_lastCurrentInterval, &applePeriod );
        displayInfo( "ORANGE", orangePeriod.m_count, orangePeriod.m_lastCurrentTick, orangePeriod.m_lastCurrentInterval, &orangePeriod );
        displayInfo( "CHERRY", cherryPeriod.m_count, cherryPeriod.m_lastCurrentTick, cherryPeriod.m_lastCurrentInterval, &cherryPeriod );
        REQUIRE( applePeriod.m_count == 1 );
        REQUIRE( orangePeriod.m_count == 0 );
        REQUIRE( cherryPeriod.m_count == 1 );
        REQUIRE( slippageCount_ == 0 );

        // Slip a single interval
        currentTick += 20 * 1000LL;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %llu", currentTick) );
        uut.executeScheduler( currentTick );
        displayInfo( "APPLE", applePeriod.m_count, applePeriod.m_lastCurrentTick, applePeriod.m_lastCurrentInterval, &applePeriod );
        displayInfo( "ORANGE", orangePeriod.m_count, orangePeriod.m_lastCurrentTick, orangePeriod.m_lastCurrentInterval, &orangePeriod );
        displayInfo( "CHERRY", cherryPeriod.m_count, cherryPeriod.m_lastCurrentTick, cherryPeriod.m_lastCurrentInterval, &cherryPeriod );
        intervalTime = 20 * 1000LL;
        REQUIRE( applePeriod.m_count == 2 );
        REQUIRE( applePeriod.m_lastCurrentTick == currentTick );
        REQUIRE( applePeriod.m_lastCurrentInterval == intervalTime );
        REQUIRE( orangePeriod.m_count == 1 );
        intervalTime = 14 * 1000LL;
        REQUIRE( cherryPeriod.m_count == 2 );
        REQUIRE( cherryPeriod.m_lastCurrentTick == currentTick );
        REQUIRE( cherryPeriod.m_lastCurrentInterval == intervalTime );
        REQUIRE( slippageCount_ == 2 );
        REQUIRE( slippageLastPeriod_ == &cherryPeriod );

        currentTick += 10 * 1000LL;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %llu", currentTick) );
        uut.executeScheduler( currentTick );
        displayInfo( "APPLE", applePeriod.m_count, applePeriod.m_lastCurrentTick, applePeriod.m_lastCurrentInterval, &applePeriod );
        displayInfo( "ORANGE", orangePeriod.m_count, orangePeriod.m_lastCurrentTick, orangePeriod.m_lastCurrentInterval, &orangePeriod );
        displayInfo( "CHERRY", cherryPeriod.m_count, cherryPeriod.m_lastCurrentTick, cherryPeriod.m_lastCurrentInterval, &cherryPeriod );
        intervalTime = 40 * 1000LL;
        REQUIRE( applePeriod.m_count == 3 );
        REQUIRE( applePeriod.m_lastCurrentTick == currentTick );
        REQUIRE( applePeriod.m_lastCurrentInterval == intervalTime );
        REQUIRE( orangePeriod.m_count == 2 );
        intervalTime = ((40 / 7) * 7) * 1000LL;
        REQUIRE( cherryPeriod.m_count == 3 );
        REQUIRE( cherryPeriod.m_lastCurrentTick == currentTick );
        REQUIRE( cherryPeriod.m_lastCurrentInterval == intervalTime );
        REQUIRE( slippageCount_ == 2 );

        // Slip multiple intervals
        currentTick += 30 * 1000LL;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %llu", currentTick) );
        uut.executeScheduler( currentTick );
        displayInfo( "APPLE", applePeriod.m_count, applePeriod.m_lastCurrentTick, applePeriod.m_lastCurrentInterval, &applePeriod );
        displayInfo( "ORANGE", orangePeriod.m_count, orangePeriod.m_lastCurrentTick, orangePeriod.m_lastCurrentInterval, &orangePeriod );
        displayInfo( "CHERRY", cherryPeriod.m_count, cherryPeriod.m_lastCurrentTick, cherryPeriod.m_lastCurrentInterval, &cherryPeriod );
        intervalTime = (40 + 10) * 1000LL;
        REQUIRE( applePeriod.m_count == 4 );
        REQUIRE( applePeriod.m_lastCurrentTick == currentTick );
        REQUIRE( applePeriod.m_lastCurrentInterval == intervalTime );
        intervalTime =((40 / 7 + 1) * 7) * 1000LL;
        REQUIRE( cherryPeriod.m_count == 4 );
        REQUIRE( cherryPeriod.m_lastCurrentTick == currentTick );
        REQUIRE( cherryPeriod.m_lastCurrentInterval == intervalTime );

        // Verify interval boundaries after 'big' slippage
        currentTick += 10 * 1000LL;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %llu", currentTick) );
        uut.executeScheduler( currentTick );
        displayInfo( "APPLE", applePeriod.m_count, applePeriod.m_lastCurrentTick, applePeriod.m_lastCurrentInterval, &applePeriod );
        displayInfo( "ORANGE", orangePeriod.m_count, orangePeriod.m_lastCurrentTick, orangePeriod.m_lastCurrentInterval, &orangePeriod );
        displayInfo( "CHERRY", cherryPeriod.m_count, cherryPeriod.m_lastCurrentTick, cherryPeriod.m_lastCurrentInterval, &cherryPeriod );
        intervalTime = 80 * 1000LL;
        REQUIRE( applePeriod.m_count == 5 );
        REQUIRE( applePeriod.m_lastCurrentTick == currentTick );
        REQUIRE( applePeriod.m_lastCurrentInterval == intervalTime );
        REQUIRE( orangePeriod.m_count == 4 );
        intervalTime = ((80 / 7) * 7) * 1000LL;
        REQUIRE( cherryPeriod.m_count == 5 );
        REQUIRE( cherryPeriod.m_lastCurrentTick == currentTick );
        REQUIRE( cherryPeriod.m_lastCurrentInterval == intervalTime );
        REQUIRE( slippageCount_ == 4 );

        // Verify scheduler stops when an error is encountered
        currentTick += 100 * 1000LL;
        applePeriod.m_result = false;
        uut.executeScheduler( currentTick );
        REQUIRE( applePeriod.m_count == 6 );
        REQUIRE( orangePeriod.m_count == 4 );
        REQUIRE( cherryPeriod.m_count == 5 );
        REQUIRE( slippageCount_ == 4 );

        uut.stop();
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
