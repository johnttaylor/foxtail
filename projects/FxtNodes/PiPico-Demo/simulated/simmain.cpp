#include "../Main.h"
#include "../ModelPoints.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/TShell/Cmd/Win32/Threads.h"
#include "Cpl/Dm/TShell/Dm.h"
#include "Driver/DIO/Simulated/InOutModelPoints.h"
#include "Cpl/Io/Stdio/StdIn.h"
#include "Cpl/Io/Stdio/StdOut.h"
#include "Fxt/Node/SBC/PiPicoDemo/Drivers.h"
#include "Driver/RHTemp/Simulated/Api.h"

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

/////////////////////////////
#define ALLOC_INVALID( n, t )               t n(  mp::g_modelDatabase, (#n) );  
namespace mp {
ALLOC_INVALID( simRh, Cpl::Dm::Mp::Float )
ALLOC_INVALID( simTemp, Cpl::Dm::Mp::Float )
ALLOC_INVALID( simHeater, Cpl::Dm::Mp::Bool )
}

static Driver::RHTemp::Simulated::Api rhTempDriver_(mp::simRh, mp::simTemp, mp::simHeater);

Driver::RHTemp::Api* g_rhTempDrivers[OPTION_FXT_NODE_SBC_PIPICODEMO__CHASSIS_RHTEMP_SENSORS_NUM_SUPPORTED] =
{
    &rhTempDriver_
};
