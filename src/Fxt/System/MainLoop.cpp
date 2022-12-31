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

#include "MainLoop.h"
#include "Cpl/System/SimTick.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/GlobalLock.h"
#include "ElapsedTime.h"

#define SECT_ "Fxt::System"


///
using namespace Fxt::System;

/////////////////////
MainLoop::MainLoop( Cpl::System::SharedEventHandlerApi* eventHandler )
    : m_myThreadPtr( 0 )
    , m_eventHandler( eventHandler )
    , m_events( 0 )
    , m_run( true )
{
    CPL_SYSTEM_ASSERT( timeOutPeriodInMicroseconds != 0 )
}
void MainLoop::setThreadOfExecution_( Cpl::System::Thread* myThreadPtr )
{
    m_myThreadPtr = myThreadPtr;
}

int MainLoop::signal( void ) noexcept
{
    // Return zero for 'success' (not really event driven -->so this method does nothing)
    return 0;
}

int MainLoop::su_signal( void ) noexcept
{
    // Return zero for 'success' (not really event driven -->so this method does nothing)
    return 0;
}

void MainLoop::appRun( void )
{
    startMainLoop();
    bool run = true;
    while ( run )
    {
        run = waitAndProcessEvents();
    }
    stopMainLoop();
}

void MainLoop::startMainLoop() noexcept
{
    // Nothing currently needed
}

void MainLoop::stopMainLoop() noexcept
{
    // Nothing currently needed
}

void MainLoop::processEventFlag( uint8_t eventNumber ) noexcept
{
    if ( m_eventHandler )
    {
        m_eventHandler->processEventFlag( eventNumber );
    }
}

bool MainLoop::waitAndProcessEvents() noexcept
{
    // Trap my exit/please-stop condition
    Cpl::System::GlobalLock::begin();
    bool stayRunning = m_run;
    Cpl::System::GlobalLock::end();
    if ( !stayRunning )
    {
        return false;
    }

    // Wait on the tick
    waitTickDuration();

    // Trap my exit/please-stop condition AGAIN since a lot could have happen while I was waiting....
    Cpl::System::GlobalLock::begin();
    stayRunning = m_run;
    Cpl::System::GlobalLock::end();
    if ( !stayRunning )
    {
        return false;
    }

    // Capture the current state of the event flags
    Cpl::System::GlobalLock::begin();
    Cpl_System_EventFlag_T events = m_events;
    m_events                      = 0;
    Cpl::System::GlobalLock::end();

    // Process Event Flags
    if ( events )
    {
        Cpl_System_EventFlag_T eventMask   = 1;
        uint8_t                eventNumber = 0;
        for ( ; eventMask; eventMask <<= 1, eventNumber++ )
        {
            if ( (events & eventMask) )
            {
                processEventFlag( eventNumber );
            }
        }
    }

    // Tick delay over (and any pending Event flags have been processed)
    return true;
}

/////////////////////
void MainLoop::pleaseStop()
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );

    // Set my flag/state to exit my top level thread loop
    Cpl::System::GlobalLock::begin();
    m_run = false;
    Cpl::System::GlobalLock::end();
}

////////////////////////////////////////////////////////////////////////////////
void MainLoop::notifyEvents( Cpl_System_EventFlag_T events ) noexcept
{
    // Mark that I was signaled
    Cpl::System::GlobalLock::begin();
    m_events |= events;
    Cpl::System::GlobalLock::end();
}

void MainLoop::notify( uint8_t eventNumber ) noexcept
{
    notifyEvents( 1 << eventNumber );
}

// NOTE: Same logic as notifyEvents(), EXCEPT no critical section is used -->this is because su_signal() is called from an ISR and no mutex is required (and mutexes don't work in from ISRs anyway)
void MainLoop::su_notifyEvents( Cpl_System_EventFlag_T events ) noexcept
{
    // Mark that I was signaled 
    m_events |= events;
}

void MainLoop::su_notify( uint8_t eventNumber ) noexcept
{
    su_notifyEvents( 1 << eventNumber );
}

