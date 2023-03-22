/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "../Main.h"
#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/RP2040/Console.h"
#include "Cpl/TShell/Cmd/Threads.h"
#include "hardware/watchdog.h"
#include "Driver/I2C/RP2040/Master.h"
#include "Driver/RHTemp/HUT31D/Api.h"
#include "Fxt/Node/SBC/Automation2040W/Drivers.h"
#include "Driver/Automation2040/Api.h"


/// Generic thread TShell command
static Cpl::TShell::Cmd::Threads threadsCmd_( g_cmdlist );

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
static Driver::RHTemp::HUT31D::Api          rhTempDriver_( i2cBus0Driver_, OPTION_I2C_DEV_ADDRESS );

/*-----------------------------------------------------------*/
int main( void )
{
    // Initialize CPL
    Cpl::System::Api::initialize();

    // Initialize the board
    Bsp_Api_initialize();

    // Start the Console/Trace output: Accepting the default UART Config parameters, e.g. 115200, 8N1
    Cpl::System::RP2040::startConsole();
    Cpl::System::RP2040::getConsoleStream().write( "\n**** APPLICATION START-UP *****\n" );
    Cpl::System::RP2040::getConsoleStream().write( "Cpl and the Bsp have been initialized.\n" );

    // Initialize/Start the I2C driver
    Driver::I2C::RP2040::Master::configureI2CPins( OPTION_I2C_SDA_PIN, OPTION_I2C_SCL_PIN );
    i2cBus0Driver_.start();

    // Initialize the Automation2040 driver
    Driver::Automation2040::Api::initialize();

    // Start the application
    return runTheApplication( Cpl::System::RP2040::getConsoleStream(), Cpl::System::RP2040::getConsoleStream() ); // This method should never return
}

void signalPlatformToTerminate() noexcept
{
    watchdog_enable( 1, 1 );
    while ( 1 );
}


Driver::RHTemp::Api* g_rhTempDrivers[OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_RHTEMP_SENSORS_NUM_SUPPORTED] =
{
    &rhTempDriver_
};
