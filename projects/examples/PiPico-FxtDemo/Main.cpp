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

#include "colony_config.h"
#include "Main.h"
#include "ModelPoints.h"
#include "Cpl/Dm/PeriodicScheduler.h"
#include "Cpl/TShell/Cmd/Help.h"
#include "Cpl/TShell/Cmd/Bye.h"
#include "Cpl/TShell/Cmd/Trace.h"
#include "Cpl/TShell/Cmd/TPrint.h"
#include "Cpl/TShell/PolledMaker.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/System/Thread.h"
#include "Cpl/TShell/Stdio.h"
#include "Cpl/Io/InputOutput.h"
#include "Fxt/Logging/Api.h"
#include "Cpl/System/EventLoop.h"
#include "Cpl/System/Semaphore.h"
#include "Fxt/Card/Mock/TShell/Ain8.h"
#include "Fxt/Card/Mock/TShell/Dio8.h"
#include "Fxt/Node/TShell/Node.h"
#include "Fxt/Node/SBC/PiPicoDemo/Factory.h"
#include "Cpl/System/Shutdown.h"

#define SECT_       "main"

////////////////////////////////////////////////////////////////////////////////

#ifndef OPTION_TEST_MAIN_MAX_POINTS
#define OPTION_TEST_MAIN_MAX_POINTS                 (256)
#endif

#ifndef OPTION_TEST_MAIN_GENERAL_HEAP_SIZE
#define OPTION_TEST_MAIN_GENERAL_HEAP_SIZE          (8*1024)
#endif

#ifndef OPTION_TEST_MAIN_STATEFUL_HEAP_SIZE
#define OPTION_TEST_MAIN_STATEFUL_HEAP_SIZE         (4*1024)
#endif

#ifndef OPTION_TEST_MAIN_CARD_STATEFUL_HEAP_SIZE
#define OPTION_TEST_MAIN_CARD_STATEFUL_HEAP_SIZE    (4*1024)
#endif

static Fxt::Point::Database<OPTION_TEST_MAIN_MAX_POINTS>   pointDb_( "dummyArgUsedToCreateStaticConstructorSignature" );
static Fxt::Node::SBC::PiPicoDemo::Factory                 nodeFactory_( OPTION_TEST_MAIN_GENERAL_HEAP_SIZE, OPTION_TEST_MAIN_CARD_STATEFUL_HEAP_SIZE, OPTION_TEST_MAIN_STATEFUL_HEAP_SIZE );

// Create the TShell (aka the debug console) and populate with the basic set 
// of commands
Cpl::Container::Map<Cpl::TShell::Command>           g_cmdlist( "ignore_this_parameter-used to invoke the static constructor" );
static Cpl::TShell::PolledMaker                     cmdProcessor_( g_cmdlist );
static Cpl::TShell::Cmd::Help                       helpCmd_( g_cmdlist );
static Cpl::TShell::Cmd::Bye                        byeCmd_( g_cmdlist );
static Cpl::TShell::Cmd::Trace                      traceCmd_( g_cmdlist );
static Cpl::TShell::Cmd::TPrint                     tprintCmd_( g_cmdlist );

static Fxt::Node::TShell::Node                      nodeCmd_( g_cmdlist, nodeFactory_, pointDb_ );
static Fxt::Card::Mock::TShell::Ain8                ain8Cmd_( g_cmdlist );
static Fxt::Card::Mock::TShell::Dio8                dio8Cmd_( g_cmdlist );

// Create the Logging FIFO (current size does not matter since we never drain the Log queue)
#define MAX_FIFO_ENTRIES    5
static Cpl::Logging::EntryData_T fifoMemory_[MAX_FIFO_ENTRIES];
static Cpl::Container::RingBufferMP<Cpl::Logging::EntryData_T> logFifo_( MAX_FIFO_ENTRIES, fifoMemory_, mp::logEntryCount );


/*-----------------------------------------------------------*/
//
// Set up Periodic scheduling Core 0 (The Chassis runs on Core 1)
//
static Cpl::Io::Input*   consoleInputFdPtr_;
static Cpl::Io::Output*  consoleOutputFdPtr_;


// Forward References
static void core0Start( Cpl::System::ElapsedTime::Precision_T currentTick );
static void core0Shutdown( Cpl::System::ElapsedTime::Precision_T currentTick );
static void core0Idle( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted );

// Periodic Intervals
static Cpl::System::PeriodicScheduler::Interval_T core0Intervals_[] =
{
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS
};

// Runnable object for Core0 (aka the thread's 'main loop')
Cpl::Dm::PeriodicScheduler core0Mbox_( core0Intervals_,
                                       core0Start,
                                       core0Shutdown,
                                       nullptr,
                                       Cpl::System::ElapsedTime::precision,
                                       core0Idle );

// In thread initialization 
void core0Start( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    CPL_SYSTEM_ASSERT( consoleInputFdPtr_ );
    CPL_SYSTEM_ASSERT( consoleOutputFdPtr_ );
    cmdProcessor_.getCommandProcessor().start( *consoleInputFdPtr_, *consoleOutputFdPtr_ );
}

// In thread shutdown 
void core0Shutdown( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Shutting down...") );
    Cpl::System::Api::sleep( 100 );

    // Stop the Chassis (if it running)
    Fxt::Node::Api* node = Fxt::Node::Api::getNode();
    if ( node )
    {
        // Ensure the node is stop then destroy the node
        node->stop();
        nodeFactory_.destroy( *node );
    }

    // Terminate the platform
    signalPlatformToTerminate();
}

void core0Idle( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted )
{
    cmdProcessor_.getCommandProcessor().poll();
}


int runTheApplication( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
{
    // Housekeeping
    consoleInputFdPtr_  = &infd;
    consoleOutputFdPtr_ = &outfd;

    // Enable Trace 
    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( SECT_ );
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "ERRCODE" );

    CPL_SYSTEM_TRACE_MSG( SECT_, ("Initializing logging...") );

    // Enable logging
    Cpl::Logging::initialize( logFifo_,
                              Fxt::Logging::CategoryId::WARNING,
                              (+Fxt::Logging::CategoryId::WARNING)._to_string(),
                              Fxt::Logging::WarningMsg::LOGGING_OVERFLOW,
                              (+Fxt::Logging::WarningMsg::LOGGING_OVERFLOW)._to_string() );


    // Create main application thread 
    Cpl::System::Thread::create( core0Mbox_, "APP" );

    // Start the thread scheduling
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Enabling scheduling") );
    Cpl::System::Api::enableScheduling();   // NOTE: This method does NOT return when running on the PICO until all cores have been stopped
    return 0;
}

//////////////////////////////////////////////////
int Cpl::System::Shutdown::success( void )
{
    core0Mbox_.pleaseStop();
    return OPTION_CPL_SYSTEM_SHUTDOWN_SUCCESS_ERROR_CODE;
}

int Cpl::System::Shutdown::failure( int exit_code )
{
    core0Mbox_.pleaseStop();
    return exit_code;
}
