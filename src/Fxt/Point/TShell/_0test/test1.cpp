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
#include "statics.h"


#include "Cpl/TShell/Stdio.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/Shutdown.h"
#include <stdio.h>

/// 
extern void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );


////////////////////////////////////////////////////////////////////////////////
static Cpl::TShell::Stdio shell_( cmdProcessor_ );

#define APPLE_ID         1
#define ORANGE_ID        3
#define CHERRY_ID        4
#define PLUM_ID          9

void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
{
    // Start the shell
    shell_.launch( infd, outfd );

    // Create some points
    new(std::nothrow) Fxt::Point::Int32( db_, APPLE_ID, "apple", stateHeap );
    new(std::nothrow) Fxt::Point::Int32( db_, ORANGE_ID, "orange", stateHeap, 3 );
    new(std::nothrow) Fxt::Point::Int32( db_, CHERRY_ID, "cherry", stateHeap, 333 );
    new(std::nothrow) Fxt::Point::Int32( db_, PLUM_ID, "plum", stateHeap, -42 );

	// Start the scheduler
    Cpl::System::Api::enableScheduling();

	// Run forever (i.e. user interactive test)
	for ( ;;)
	{
		Cpl::System::Api::sleep( 1000 );
	}
}


