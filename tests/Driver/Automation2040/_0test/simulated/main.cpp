#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/TShell/Cmd/Win32/Threads.h"
#include "Cpl/Dm/TShell/Dm.h"
#include "Driver/Automation2040/Simulated/ModelPoints.h"
#include "Cpl/Io/Stdio/StdIn.h"
#include "Cpl/Io/Stdio/StdOut.h"


// External references
extern void test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );

Cpl::Container::Map<Cpl::TShell::Command>   cmdlist( "ignore_this_parameter-used to invoke the static constructor" );
static Cpl::TShell::Cmd::Threads            threadsCmd_( cmdlist );

Cpl::Dm::ModelDatabase                      mp::g_modelDatabase( "ignore_this_parameter-used to invoke the static constructor" );
static Cpl::Dm::TShell::Dm                  dmCmd_( cmdlist, mp::g_modelDatabase );

static Cpl::Io::Stdio::StdIn                infd_;
static Cpl::Io::Stdio::StdOut               outfd_;

/*-----------------------------------------------------------*/
int main( void )
{
    // Initialize CPL
    Cpl::System::Api::initialize();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );


    // Run the test (the call never returns!)
    test( infd_, outfd_ );
    return 0;
}
