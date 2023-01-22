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


#include "../../Main.h"
#include "Cpl/System/Api.h"
#include "Cpl/TShell/Cmd/Win32/Threads.h"
#include "Cpl/Io/Stdio/StdIn.h"
#include "Cpl/Io/Stdio/StdOut.h"
#include "Cpl/System/Semaphore.h"

static Cpl::TShell::Cmd::Win32::Threads cmdThreads_( g_cmdlist );


static Cpl::Io::Stdio::StdIn    infd_;
static Cpl::Io::Stdio::StdOut   outfd_;

static Cpl::System::Semaphore   waitForShutdown_;

int main( int argc, char* const argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();

    // Run the application
    runTheApplication( infd_, outfd_ );

    // Wait to be shutdown
    waitForShutdown_.wait();
    return 0;
}

void signalPlatformToTerminate() noexcept
{
    waitForShutdown_.signal();
}
