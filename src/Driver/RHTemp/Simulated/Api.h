#ifndef Driver_RHTemp_Api_h_
#define Driver_RHTemp_Api_h_
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

///
namespace Driver {
///
namespace RHTemp {

/** This class defines an interface for reading and managing a combined RH
    and Temperature sensor. 
    
    Note: There is no requirement for how RH/Temperature is measured; or if 
          the sensor 'package' is connected to the MCU via discrete signals or 
          through a serial bus.

    The class is NOT thread safe.  The application is RESPONSIBLE for ensuring
    thread-safe usage.
 */
class Api
{
public:
    /** Requested Accuracy.  Typically the higher the accuracy, the longer
        sensor/measurement time.
     */
    enum Accuracy_T
    {
        eLOWEST  = 0,   //!< Lowest possible accuracy
        eGOOD    = 1,   //!< More accurate than eLOWEST
        eBETTER  = 2,   //!< More accurate than eGOOD
        eHIGHEST = 3    //!< Highest possible accuracy
    };


public:
    /** This method initializes the driver.  It must be called only ONCE and
        BEFORE any of the other methods are called.  The on-board heater is
        placed in the disabled state.
     */
    virtual void initialize() noexcept = 0;


    /// Starts the driver actively sampling and outputting signals
    virtual void start() noexcept = 0;

    /** Stops the driver from sampling inputs, and places all of the output
        into their 'safe' state
     */
    virtual void stop() noexcept = 0;
    
public:
    /** This method is used to read/sample both RH (percentage 0 to 100) and 
        Temperature (in degrees Centigrade).  This method blocks/busy-waits 
        until the both RH and Temperature has been captured.

        The method returns true when it is successful is reading RH and
        Temperature; else false is returned (e.g. a communication error occurred).

        Note: The 'requestedAccuracy' argument is a suggestion, i.e. the
              concrete implementation may or may not support the accuracy
              request.
     */
    virtual bool sampleRHAndTemperature( float& rhOut, float& tempCOut, Accuracy_T requestedAccuracy = eLOWEST ) noexcept = 0;

    /** This method is similar to sampleRHAndTemperature(), except on RH is
        sampled.
     */
    virtual bool sampleRH( float& rhOut, Accuracy_T requestedAccuracy = eLOWEST ) noexcept = 0;

    /** This method is similar to sampleRHAndTemperature(), except on Temperature is
        sampled.
     */
    virtual bool sampleTemperature( float& rhOut, Accuracy_T requestedAccuracy = eLOWEST ) noexcept = 0;

public:
    /** This method is used to enable/disable a 'on-board heater' that is used to 
        evaporate condensation on the sensing element(s). 

        NOTE: The concrete implementation is NOT required to support this
              method, i.e. it is Application RESPONSIBILITY for ensuring it
              business logic that relies on this functionality - that it has
              create the appropriate concrete driver.
        When the heater is on, the Temperature reading are not guaranteed to 
        be accurate.

        The method returns true when it is successful; else false is returned
        (e.g. a communication error occurred).
     */
    virtual bool setHeaderState( bool enabled ) noexcept { return true };

public:
    /// Virtual destructor
    virtual ~Api(){}
};

} // End namespace(s)
}

/*--------------------------------------------------------------------------*/
#endif  // end header latch
