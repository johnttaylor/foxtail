#include "colony_config.h"
#include "Bsp/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/TShell/Cmd/Threads.h"
#include "Cpl/System/RP2040/Console.h"
#include "Driver/I2C/RP2040/Master.h"
#include "Driver/RHTemp/HUT31D/Api.h"

// External references
extern void test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );

Cpl::Container::Map<Cpl::TShell::Command>  cmdlist( "ignore_this_parameter-used to invoke the static constructor" );
Cpl::TShell::Cmd::Threads                  threadsCmd_( cmdlist );



#ifndef OPTION_I2C_DEVICE
#define OPTION_I2C_DEVICE   i2c0
#endif

#ifndef OPTION_I2C_BAUDRATE
#define OPTION_I2C_BAUDRATE (1000*1000)      // 1MHz
#endif

#ifndef OPTION_I2C_TIMEOUT
#define OPTION_I2C_TIMEOUT  (1000)          // 1 second
#endif

#ifndef OPTION_I2C_SDA_PIN
#define OPTION_I2C_SDA_PIN  PICO_DEFAULT_I2C_SDA_PIN    // GP4/pin.6
#endif

#ifndef OPTION_I2C_SCL_PIN
#define OPTION_I2C_SCL_PIN  PICO_DEFAULT_I2C_SCL_PIN    // GP5/pin.7
#endif

#ifndef OPTION_I2C_DEV_ADDRESS
#define OPTION_I2C_DEV_ADDRESS  0x40
#endif

static Driver::I2C::RP2040::Master          i2cBus0Driver_( OPTION_I2C_DEVICE, OPTION_I2C_BAUDRATE, OPTION_I2C_TIMEOUT );
static Driver::RHTemp::HUT31D::Api          uut_( i2cBus0Driver_, OPTION_I2C_DEV_ADDRESS );
Driver::RHTemp::Api*                        g_uutPtr = &uut_;


/*-----------------------------------------------------------*/
int main(void)
{
    // Initialize CPL
    Cpl::System::Api::initialize();

    // Initialize the board
    Bsp_Api_initialize();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );

    // Start the Console/Trace output: Accepting the default UART Config parameters, e.g. 115200, 8N1
    Cpl::System::RP2040::startConsole();

    // Initialize/Start the I2C driver
    Driver::I2C::RP2040::Master::configureI2CPins( OPTION_I2C_SDA_PIN, OPTION_I2C_SCL_PIN );
    i2cBus0Driver_.start();

    // Run the test (the call never returns!)
    test( Cpl::System::RP2040::getConsoleStream(), Cpl::System::RP2040::getConsoleStream() );
    return 0;
}
