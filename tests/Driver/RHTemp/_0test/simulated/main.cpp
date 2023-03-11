#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/TShell/Cmd/Win32/Threads.h"
#include "Cpl/Dm/TShell/Dm.h"
#include "Driver/RHTemp/Simulated/Api.h"
#include "Cpl/Io/Stdio/StdIn.h"
#include "Cpl/Io/Stdio/StdOut.h"


// External references
extern void test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );

Cpl::Container::Map<Cpl::TShell::Command>   cmdlist( "ignore_this_parameter-used to invoke the static constructor" );
static Cpl::TShell::Cmd::Threads            threadsCmd_( cmdlist );

#define ALLOC_INVALID( n, t )               t n(  mp::modelDatabase_, (#n) );  
namespace mp {
static Cpl::Dm::ModelDatabase               modelDatabase_( "ignore_this_parameter-used to invoke the static constructor" );
ALLOC_INVALID( simRh, Cpl::Dm::Mp::Float )
ALLOC_INVALID( simTemp, Cpl::Dm::Mp::Float )
ALLOC_INVALID( simHeater, Cpl::Dm::Mp::Bool )
}

static Cpl::Dm::TShell::Dm                  dmCmd_( cmdlist, mp::modelDatabase_ );

static Cpl::Io::Stdio::StdIn                infd_;
static Cpl::Io::Stdio::StdOut               outfd_;


static Driver::RHTemp::Simulated::Api       uut_( mp::simRh, mp::simTemp, mp::simHeater );
Driver::RHTemp::Api*                        g_uutPtr = &uut_;

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
