#ifndef Fxt_Node_SBC_Automation2040W_Drivers_h_
#define Fxt_Node_SBC_Automation2040W_Drivers_h_
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
/** @file */


#include "colony_config.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Driver/RHTemp/Api.h"

/// Maximum number of RH/Temperature sensors supported
#ifndef OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_RHTEMP_SENSORS_NUM_SUPPORTED 
#define OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_RHTEMP_SENSORS_NUM_SUPPORTED    1 
#endif

/// Required Starting Slot number for the RH/Temperature Sensor (when used)
#ifndef OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_RHTEMP_SENSOR_BASE_SLOTNUM  
#define OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_RHTEMP_SENSOR_BASE_SLOTNUM      255
#endif


/*---------------------------------------------------------------------------*/
/** Reference to the Driver thread's Mailbox server
 */
extern Cpl::Dm::MailboxServer*   g_driverThreadMailboxPtr;

/// Reference(s) to RH/Temp sensor driver instance(s)
extern Driver::RHTemp::Api* g_rhTempDrivers[OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_RHTEMP_SENSORS_NUM_SUPPORTED];


#endif  // end header latch
