#ifndef Fxt_System_ElaspedTime_h_
#define Fxt_System_ElaspedTime_h_
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

#include <stdint.h>


/// 
namespace Fxt {
/// 
namespace System {

/** This class defines the interface for accessing the elapsed time since
    power up and/or reset of the platform.  Elapsed time resolution is
    microseconds.
 */
class ElapsedTime
{
public:
    /** This method returns the elapsed time, in seconds with microseconds
        since the system was powered on and/or reset.  
     */
    static uint64_t now() noexcept;

public:
    /** This method returns the delta time, in microseconds, between the
        specified 'startTime' and 'endTime'.  'endTime' is defaulted to
        NOW.
     */
    static uint64_t delta( uint64_t startTime, uint64_t endTime = now() ) noexcept
    {
        return endTime - startTime;
    }

public:
    /** This method returns true if the specified amount of time has elapsed
        since the 'timeMarker'.  
     */
    inline static bool expired( uint64_t timeMarker, uint64_t duration, uint64_t currentTime = now() ) noexcept
    {
        return delta( timeMarker, currentTime ) >= duration;
    }


public:
    /// Convert microseconds to milliseconds (rounds down)
    inline static uint64_t toMilliseconds( uint64_t elapsedTime ) noexcept
    {
        return elapsedTime / 1000LL;
    }

    /// Converts microseconds to seconds (rounds down)
    inline static uint64_t toSeconds( uint64_t elapsedTime ) noexcept
    {
        return elapsedTime / (1000LL * 1000LL);
    }

    /// Convert milliseconds to microseconds
    inline static uint64_t millisecondsToMicroseconds( uint64_t milliseconds ) noexcept
    {
        return milliseconds * 1000LL;
    }

    /// Convert seconds to microseconds
    inline static uint64_t secondsToMicroseconds( uint64_t seconds ) noexcept
    {
        return seconds * 1000LL * 1000LL;
    }

public:
    /** This method is the same as now(), EXCEPT that is ALWAYS
        guaranteed to return elapsed time in 'real time'.  See the
        Cpl::System::SimTick for more details about real time vs. simulated
        time.  It is recommended the application NOT CALL this method because
        then that code can NOT be simulated using the SimTick interface.
     */
    static uint64_t nowInRealTime() noexcept;
};


};      // end namespaces
};
#endif  // end header latch

