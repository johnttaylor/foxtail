#ifndef Fxt_Card_Sensor_I2C_DriverApi_h_
#define Fxt_Card_Sensor_I2C_DriverApi_h_
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

#include "Driver/RHTemp/Api.h"
#include <stdint.h>

///
namespace Fxt {
///
namespace Card {
///
namespace Sensor {
///
namespace I2C {

/** This class is used to get access to RH Temperature sensor driver instances.  
    
    The Application/concrete Node is responsible for implementing this class.
 */
class RHTemperatureDriver
{
public:
    /** This method returns a RH Temperature sensor driver instance based on the
        specified slot number.

        The method returns a null pointer when/if the specified slot is 'out-of-range'.

        The application/concrete Node defines what the valid slot number range is.
     */
    static Driver::RHTemp::Api* get( uint8_t slotNumber ) noexcept;

    /** This method returns true if the specific slot number is valid slot for
        a RH/Temperature sensor on the platform.  See comments above.
     */
    static bool isValidSlot( uint8_t slotNumber ) noexcept;
};

};      // end namespaces
};
};
};
#endif  // end header latch
