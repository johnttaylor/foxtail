#ifndef Driver_RHTemp_HUT31D_Api_h_
#define Driver_RHTemp_HUT31D_Api_h_
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
#include "Driver/RHTemp/Api.h"
#include "Driver/I2C/Master.h"


/// Number of read attempts when reading conversion results
#ifndef OPTION_DRIVER_RHTEMP_HUT31D_READ_RESULT_ATTEMPTS
#define OPTION_DRIVER_RHTEMP_HUT31D_READ_RESULT_ATTEMPTS        2   // Initial read, plus one retry
#endif

///
namespace Driver {
///
namespace RHTemp {
///
namespace HUT31D {

/** This class implements the RH/Temperature driver interface using a TE HUT31D
    RH/T Sensor IC.

    The implementation ASSUMES (i.e. requires) that the Application is RESPONSIBLE
    for starting/stopping the i2cDriver since it is a shared resource.

    The interface itself is NOT thread safe. It is the responsibility of
    the users/clients of the driver to handle any threading issues.
 */
class Api: public Driver::RHTemp::Api
{
public:
    /// Constructor
    Api( Driver::I2C::Master& i2cDriver, uint8_t i2cDevice7BitAddress );

public:
    /// See Driver::RHTemp::Api
    bool start() noexcept;
    
    /// See Driver::RHTemp::Api
    void stop() noexcept;

public:
    /// See Driver::RHTemp::Api
    bool sample( float& rhOut, float& tempCOut ) noexcept;

public:
    /// See Driver::RHTemp::Api
    bool setHeaterState( bool enabled ) noexcept;

public:
    /// See Driver::RHTemp::Api
    SamplingState_T startSample() noexcept;
    
    /// See Driver::RHTemp::Api
    SamplingState_T getSamplingState() noexcept;
    
    /// See Driver::RHTemp::Api
    SamplingState_T getSample( float& rhOut, float& tempCOut ) noexcept;

protected:
    /// Helper: Starts non-blocking sampling
    Driver::I2C::Master::Result_T startConversion();

    /// Helper: Reads the conversion results
    Driver::I2C::Master::Result_T readConversionResult( float& rhOut, float& tempCOut );

    /// Helper: Reads the conversion results
    SamplingState_T checkSamplingTime();

protected:
    /// I2C driver
    Driver::I2C::Master&    m_i2cDriver;

    /// Timestamp of start-sampling-request
    uint32_t                m_timeMarker;

    /// Current Sampling state
    SamplingState_T         m_sampleState;

    /// Device address of the sensor
    uint8_t                 m_devAddress;

    /// Started state
    bool                    m_started;
};

} // End namespace(s)
}
}

/*--------------------------------------------------------------------------*/
#endif  // end header latch
