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


#include "Drivers.h"
#include "Fxt/Card/Sensor/I2C/RHTemperatureDriver.h"

Driver::RHTemp::Api* Fxt::Card::Sensor::I2C::RHTemperatureDriver::get( uint8_t slotNumber ) noexcept
{
    return g_rhTempDrivers[slotNumber - OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_RHTEMP_SENSOR_BASE_SLOTNUM];
}

bool Fxt::Card::Sensor::I2C::RHTemperatureDriver::isValidSlot( uint8_t slotNumber ) noexcept
{
    if ( slotNumber >= OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_RHTEMP_SENSOR_BASE_SLOTNUM &&
         slotNumber < (OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_RHTEMP_SENSOR_BASE_SLOTNUM + OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_RHTEMP_SENSORS_NUM_SUPPORTED) )    
    {
        return true;
    }
    return false;
}
