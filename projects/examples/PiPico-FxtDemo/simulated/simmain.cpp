#include "../Main.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/TShell/Cmd/Win32/Threads.h"
#include "Cpl/Dm/TShell/Dm.h"
#include "Driver/DIO/Simulated/InOutModelPoints.h"
#include "Cpl/Io/Stdio/StdIn.h"
#include "Cpl/Io/Stdio/StdOut.h"


static Cpl::TShell::Cmd::Threads            threadsCmd_( g_cmdlist );
static Cpl::Dm::TShell::Dm                  dmCmd_( g_cmdlist, mp::g_modelDatabase );

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


    // Run the application 
    runTheApplication( infd_, outfd_ ); 

    // Don't let the application terminated
    for ( ;;)
    {
        Cpl::System::Api::sleep( 1000 );
    }
    return 0;
}

void signalPlatformToTerminate() noexcept
{
    exit(0);
}
