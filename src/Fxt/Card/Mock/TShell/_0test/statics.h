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

#include "Cpl/Memory/LeanHeap.h"
#include "Fxt/Point/Database.h"
#include "Fxt/Point/Int32.h"
#include "Cpl/TShell/Maker.h"
#include "Cpl/TShell/Cmd/Help.h"
#include "Cpl/TShell/Cmd/Bye.h"
#include "Cpl/TShell/Cmd/Trace.h"
#include "Cpl/TShell/Cmd/TPrint.h"

#include "Fxt/Point/TShell/Pt.h"



////////////////////////////////////////////////////////////////////////////////

extern Cpl::Container::Map<Cpl::TShell::Command>   cmdlist;

static Cpl::TShell::Maker cmdProcessor_( cmdlist );

static Cpl::TShell::Cmd::Help    helpCmd_( cmdlist );
static Cpl::TShell::Cmd::Bye     byeCmd_( cmdlist );
static Cpl::TShell::Cmd::Trace   traceCmd_( cmdlist );
static Cpl::TShell::Cmd::TPrint  tprintCmd_( cmdlist );


// Allocate/create my Model Database
#define MAX_POINTS  10
static Fxt::Point::Database<MAX_POINTS>     db_("ignoreThisParameter_usedToInvokeTheStaticConstructor" );

static Fxt::Point::TShell::Pt   ptCmd_( cmdlist, db_ );

// Contiguous Heap use for creating points
#define ELEM_SIZE_AS_SIZET(elemSize)    (((elemSize)+sizeof( size_t ) - 1) / sizeof(size_t))
static size_t stateHeapMemory_[ELEM_SIZE_AS_SIZET( sizeof( Fxt::Point::Int32::StateBlock_T ) ) * MAX_POINTS];
static Cpl::Memory::LeanHeap    stateHeap( stateHeapMemory_, sizeof( stateHeapMemory_ ) );


