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
#include "Fxt/System/MainLoop.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Semaphore.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/System/Tls.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/EventLoop.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


#define SECT_     "_0test"
/// 
using namespace Fxt::System;



////////////////////////////////////////////////////////////////////////////////
namespace {
class TickSource : public MainLoop
{
public:
    unsigned long m_tickDelayMs;

public:
    /// Constructor
    TickSource( unsigned long tickTimingMs = 1,
                Cpl::System::SharedEventHandlerApi* eventHandler = 0 )
        : MainLoop( eventHandler )
        , m_tickDelayMs( tickTimingMs )
    {
    }

    /// Tick
    void waitTickDuration() noexcept
    {
        Cpl::System::Api::sleep( m_tickDelayMs );
    }
};

class MyRunnable : public MainLoop
{
public:
    ///
    Cpl::System::Thread&    m_masterThread;
    ///
    Cpl::System::Thread*     m_nextThreadPtr;
    ///
    int                      m_loops;
    ///
    int                      m_maxLoops;

public:
    ///
    MyRunnable( Cpl::System::Thread& masterThread, Cpl::System::Thread* nextThreadPtr, int maxLoops  )
        :m_masterThread( masterThread ),
        m_nextThreadPtr( nextThreadPtr ),
        m_loops( 0 ),
        m_maxLoops( maxLoops )
    {
    }

public:
    ///
    void appRun()
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, Cpl::System::Thread::getCurrent().getName() );

        startMainLoop();

        while ( isRunning() && m_loops < m_maxLoops )
        {
            m_loops++;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Waiting.... (loops=%d)", m_loops) );
            Cpl::System::Thread::wait();
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Signaled.  (loops=%d)", m_loops) );
            if ( isRunning() && m_nextThreadPtr )
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Signaling: %s", m_nextThreadPtr->getName()) );
                m_nextThreadPtr->signal();
            }

            m_masterThread.signal();
        }

        stopMainLoop();
    }

    /// Tick
    void waitTickDuration() noexcept
    {
        Cpl::System::Api::sleep( 1 );
    }
};

}; // end namespace

////////////////////////////////////////////////////////////////////////////////
static int counter01 = 0;
static int counter02 = 0;
static int counter03 = 2;
static int counter11 = 0;
static int counter13 = 2;
static int counter21 = 0;

static void callback1( void* context )
{
    int* ptr = (int*) context;
    *ptr = *ptr + 1;
}
static void callback2( void* context )
{
    int* ptr = (int*) context;
    *ptr = *ptr - 1;
}
static void callback3( void* context )
{
    int* ptr = (int*) context;
    *ptr = *ptr + 10;
}

static Cpl::System::SharedEventHandlerApi::EventCallback_T callbacks1[3] ={ {callback1, &counter01}, {callback2, &counter02}, {callback3, &counter03} };
static Cpl::System::SharedEventHandlerApi::EventCallback_T callbacks2[3] ={ {callback1, &counter11}, {0,0}, { callback3, &counter13 } };
static Cpl::System::SharedEventHandlerApi::EventCallback_T callbacks3[1] ={ {callback1, &counter21} };


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "mainloop" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    SECTION( "basic test" )
    {
        MyRunnable appleRun( Cpl::System::Thread::getCurrent(), 0, 3 );
        Cpl::System::Thread* appleThreadPtr   = Cpl::System::Thread::create( appleRun, "Apple" );

        MyRunnable orangeRun( Cpl::System::Thread::getCurrent(), appleThreadPtr, 4 );
        Cpl::System::Thread* orangeThreadPtr  = Cpl::System::Thread::create( orangeRun, "Orange" );

        MyRunnable pearRun( Cpl::System::Thread::getCurrent(), orangeThreadPtr, 3  );
        Cpl::System::Thread* pearThreadPtr    = Cpl::System::Thread::create( pearRun, "Pear" );

        CPL_SYSTEM_TRACE_MSG( SECT_, ("Signaling: %s", appleThreadPtr->getName()) );
        appleRun.m_nextThreadPtr = pearThreadPtr;
        appleThreadPtr->signal();

        int i;
        for ( i=0; i < 3 + 3 + 3; i++ )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Wait %d", i + 1) );
            Cpl::System::Thread::wait();
        }

        pearRun.signal();
        Cpl::System::Thread::destroy( *pearThreadPtr );
        REQUIRE( orangeThreadPtr->isRunning() );
        Cpl::System::Thread::destroy( *orangeThreadPtr ); // Note: this is still an active thread at this point
        Cpl::System::Thread::destroy( *appleThreadPtr );
        Cpl::System::Api::sleep( 300 ); // Allow time for the threads to self terminate
    }

#define NUM_SEQ_    3
#define EVENT_FLAGS 4

    SECTION( "events" )
    {
        Cpl::System::SharedEventHandler<3> eventHandler1( callbacks1 );
        Cpl::System::SharedEventHandler<3> eventHandler2( callbacks2 );
        Cpl::System::SharedEventHandler<1> eventHandler3( callbacks3 );

        TickSource fruits( 10, &eventHandler1 );
        TickSource trees( 10, &eventHandler2 );
        TickSource flowers( 10, &eventHandler3 );

        // Create all of the threads
        Cpl::System::Thread* t1  = Cpl::System::Thread::create( fruits, "FRUITS" );
        Cpl::System::Thread* t2  = Cpl::System::Thread::create( trees, "TREES" );
        Cpl::System::Thread* t3  = Cpl::System::Thread::create( flowers, "FLOWERS" );

        // Give time for all of threads to be created before starting the test sequence
        Cpl::System::Api::sleep( 100 );

        // Run the sequence N times
        for ( int j=1; j <= NUM_SEQ_; j++ )
        {
            // Signal the event flags
            for ( int i=0; i < EVENT_FLAGS; i++ )
            {
                // Start a test sequence
                fruits.notify( i );
                flowers.notify( i );
                trees.notify( i );
                Cpl::System::Api::sleep( 100 );  // Allow other threads to process
            }

            REQUIRE( counter01 == j );
            REQUIRE( counter02 == -j );
            REQUIRE( counter03 == j * 10 + 2 );
            REQUIRE( counter11 == j );
            REQUIRE( counter13 == j * 10 + 2 );
            REQUIRE( counter21 == j );
        }

        // Shutdown threads
        fruits.pleaseStop();
        trees.pleaseStop();
        flowers.pleaseStop();

        Cpl::System::Api::sleep( 300 ); // allow time for threads to stop
        REQUIRE( t1->isRunning() == false );
        REQUIRE( t2->isRunning() == false );
        REQUIRE( t3->isRunning() == false );

        Cpl::System::Thread::destroy( *t1 );
        Cpl::System::Thread::destroy( *t2 );
        Cpl::System::Thread::destroy( *t3 );
        Cpl::System::Api::sleep( 300 ); // allow time for threads to stop
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
