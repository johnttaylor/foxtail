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
#include "Fxt/Chassis/Server.h"
#include "Fxt/System/PeriodicScheduler.h"
#include "Fxt/System/Tick1MsecBlocking.h"
#include "Fxt/System/ElapsedTime.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


#define SECT_     "_0test"
/// 
using namespace Fxt::Chassis;



////////////////////////////////////////////////////////////////////////////////


/// Mock MockPeriod for testing
class MockPeriod : public Fxt::System::PeriodApi
{
public:
    uint64_t m_lastCurrentTick;
    uint64_t m_lastCurrentInterval;
    unsigned m_count;
    bool     m_result;

public:
    /// Constructor
    MockPeriod( uint64_t duration )
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
        //CPL_SYSTEM_TRACE_MSG( SECT_, ("Period=%p, count=%u, interval=%llu, tick=%llu", this, m_count, currentInterval, currentTick) );
        return m_result;
    }
};

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
TEST_CASE( "Server" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    MockPeriod in1Period( 100 * 1000LL );
    MockPeriod in2Period( 200 * 1000LL );
    MockPeriod out1Period( 500 * 1000LL );
    MockPeriod out2Period( 600 * 1000LL );
    MockPeriod exe1Period( 1000 * 1000LL );
    MockPeriod exe2Period( 2000 * 1000LL );
    Fxt::System::PeriodApi* inPeriods[]  ={ &in1Period, &in2Period, nullptr };
    Fxt::System::PeriodApi* outPeriods[] ={ &out1Period, &out2Period, nullptr };
    Fxt::System::PeriodApi* exePeriods[] ={ &exe1Period, &exe2Period, nullptr };
    ChassisPeriods_T        periods      ={ inPeriods, exePeriods, outPeriods };

    SECTION( "basic" )
    {
        Server<Fxt::System::Tick1MsecBlocking> uut( 100 * 1000LL );
        Cpl::System::Thread*                   t1  = Cpl::System::Thread::create( uut, "UUT" );

        uut.open( &periods );
        Cpl::System::Api::sleep( 5000 );
        uut.close();

        REQUIRE( in1Period.m_count > ((50 / 1) / 2) );
        REQUIRE( in2Period.m_count > ((50 / 2) / 2) );
        REQUIRE( out1Period.m_count > ((50 / 5) / 2) );
        REQUIRE( out2Period.m_count > ((50 / 6) / 2) );
        REQUIRE( exe1Period.m_count > ((50 / 10) / 2) );
        REQUIRE( exe2Period.m_count > ((50 / 20) / 2) );
        in1Period.m_count  = 0;
        in2Period.m_count  = 0;
        out1Period.m_count = 0;
        out2Period.m_count = 0;
        exe1Period.m_count = 0;
        exe2Period.m_count = 0;

        uut.open( &periods );
        Cpl::System::Api::sleep( 4000 );
        uut.close();

        REQUIRE( in1Period.m_count > ((40 / 1) / 2) );
        REQUIRE( in2Period.m_count > ((40 / 2) / 2) );
        REQUIRE( out1Period.m_count > ((40 / 5) / 2) );
        REQUIRE( out2Period.m_count > ((40 / 6) / 2) );
        REQUIRE( exe1Period.m_count > ((40 / 10) / 2) );
        REQUIRE( exe2Period.m_count > ((40 / 20) / 2) );

        // Shutdown threads
        uut.pleaseStop();
        Cpl::System::Api::sleep( 300 ); // allow time for threads to stop
        REQUIRE( t1->isRunning() == false );
        Cpl::System::Thread::destroy( *t1 );
        Cpl::System::Api::sleep( 300 ); // allow time for threads to stop
    }


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
