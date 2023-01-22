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
#include "Cpl/System/Shutdown.h"
#include "Cpl/TShell/Cmd/Help.h"
#include "Cpl/TShell/Cmd/Bye.h"
#include "Cpl/TShell/Cmd/Trace.h"
#include "Cpl/TShell/Cmd/TPrint.h"
#include "Cpl/TShell/Maker.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/System/Thread.h"
#include "Cpl/TShell/Stdio.h"
#include "Fxt/Logging/Api.h"
#include "Cpl/System/EventLoop.h"
#include "Cpl/System/Semaphore.h"
#include "Fxt/Card/Mock/TShell/Ain8.h"
#include "Fxt/Card/Mock/TShell/Dio8.h"
#include "Fxt/Node/TShell/Node.h"
#include "Fxt/Node/Mock/Kestrel/Factory.h"

////////////////////////////////////////////////////////////////////////////////

#ifndef OPTION_TEST_MAIN_MAX_POINTS
#define OPTION_TEST_MAIN_MAX_POINTS                 (128*1024)
#endif

#ifndef OPTION_TEST_MAIN_GENERAL_HEAP_SIZE
#define OPTION_TEST_MAIN_GENERAL_HEAP_SIZE          (128*1024)
#endif

#ifndef OPTION_TEST_MAIN_STATEFUL_HEAP_SIZE
#define OPTION_TEST_MAIN_STATEFUL_HEAP_SIZE         (128*1024)
#endif

#ifndef OPTION_TEST_MAIN_CARD_STATEFUL_HEAP_SIZE
#define OPTION_TEST_MAIN_CARD_STATEFUL_HEAP_SIZE    (32*1024)
#endif

static Fxt::Point::Database<OPTION_TEST_MAIN_MAX_POINTS>   pointDb_("dummyArgUsedToCreateStaticConstructorSignature");
static Fxt::Node::Mock::Kestrel::Factory                   kestrelFactory_( OPTION_TEST_MAIN_GENERAL_HEAP_SIZE, OPTION_TEST_MAIN_CARD_STATEFUL_HEAP_SIZE, OPTION_TEST_MAIN_STATEFUL_HEAP_SIZE );

// Create the TShell (aka the debug console) and populate with the basic set 
// of commands
Cpl::Container::Map<Cpl::TShell::Command>           g_cmdlist( "ignore_this_parameter-used to invoke the static constructor" );
static Cpl::TShell::Maker                           cmdProcessor_( g_cmdlist );
static Cpl::TShell::Stdio                           shell_( cmdProcessor_, "TShell", CPL_SYSTEM_THREAD_PRIORITY_NORMAL + CPL_SYSTEM_THREAD_PRIORITY_LOWER + CPL_SYSTEM_THREAD_PRIORITY_LOWER );
static Cpl::TShell::Cmd::Help                       helpCmd_( g_cmdlist );
static Cpl::TShell::Cmd::Bye                        byeCmd_( g_cmdlist );
static Cpl::TShell::Cmd::Trace                      traceCmd_( g_cmdlist );
static Cpl::TShell::Cmd::TPrint                     tprintCmd_( g_cmdlist );

static Fxt::Node::TShell::Node                      nodeCmd_( g_cmdlist, kestrelFactory_, pointDb_ );
static Fxt::Card::Mock::TShell::Ain8                ain8Cmd_( g_cmdlist );
static Fxt::Card::Mock::TShell::Dio8                dio8Cmd_( g_cmdlist );

// Stuffs needed for receiving/handle the Shutdown signal (part of the CPL C++ class library)
static Cpl::System::Semaphore       waitForShutdown_;
static volatile int                 exitCode_;

// Create the Logging FIFO (current size does not matter since we never drain the Log queue)
#define MAX_FIFO_ENTRIES    5
static Cpl::Logging::EntryData_T fifoMemory_[MAX_FIFO_ENTRIES];
static Cpl::Container::RingBufferMP<Cpl::Logging::EntryData_T> logFifo_( MAX_FIFO_ENTRIES, fifoMemory_, mp::logEntryCount );

int runTheApplication( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
{
    // Enable Trace 
    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "ERRCODE" );

    // Enable logging
    Cpl::Logging::initialize( logFifo_,
                              Fxt::Logging::CategoryId::WARNING,
                              (+Fxt::Logging::CategoryId::WARNING)._to_string(),
                              Fxt::Logging::WarningMsg::LOGGING_OVERFLOW,
                              (+Fxt::Logging::WarningMsg::LOGGING_OVERFLOW)._to_string() );


    // Start the application
    //outDriver_.open();
    //inDriver_.open();
    //algo_.open();

    // Start the TShell/debug-console
    shell_.launch( infd, outfd );

    // Wait for the Application to be shutdown
    waitForShutdown_.wait();

    // Shutdown application objects (MUST be done in the reverse order of the open() calls)
    //algo_.close();
    //inDriver_.close();
    //outDriver_.close();
    //persistentRunnable_.close();
    
    // Shutdown logging
    Cpl::Logging::shutdown();

    // Shutdown the Application threads
    //appMbox_.pleaseStop();
    //driverMbox_.pleaseStop();
    //Cpl::System::Thread::destroy( *persistentThread );
    //Cpl::System::Thread::destroy( *appThread );
    //Cpl::System::Thread::destroy( *driverThread );

    // Run any/all register shutdown handlers (as registered by the Cpl::System::Shutdown interface) and then exit
    return Cpl::System::Shutdown::notifyShutdownHandlers_( exitCode_ );
}

////////////////////////////////////////////////////////////////////////////////
int Cpl::System::Shutdown::success( void )
{
    exitCode_ = OPTION_CPL_SYSTEM_SHUTDOWN_SUCCESS_ERROR_CODE;
    waitForShutdown_.signal();
    return OPTION_CPL_SYSTEM_SHUTDOWN_SUCCESS_ERROR_CODE;
}

int Cpl::System::Shutdown::failure( int exit_code )
{
    exitCode_ = exit_code;
    waitForShutdown_.signal();
    return exit_code;
}
