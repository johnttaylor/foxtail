#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#define CATCH_CONFIG_RUNNER  
#include "Catch/catch.hpp"


int main( int argc, char* argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();
    Cpl::System::Api::enableScheduling();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "Cpl::Itc" );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eVERBOSE );
	//CPL_SYSTEM_TRACE_SET_THREAD_FILTER( "Viewer" );
	//CPL_SYSTEM_TRACE_SET_THREAD_2FILTERS( "Viewer", "MASTER" );

    // Run the test(s)
    return Catch::Session().run( argc, argv );
}
