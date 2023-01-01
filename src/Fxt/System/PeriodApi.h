#ifndef Fxt_System_PeriodicApi_h_
#define Fxt_System_PeriodicApi_h_
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

/** This abstract class defines Period that can be scheduled/executed by
    the PeriodicSchedular class.

    NOTE: Exposing the Period's variable as public is done purpose to minimize
          the overhead in the scheduling processing, i.e. the variables are 
          access multiple times per the main loop iterations!
          
 */
class PeriodApi
{
public:
    /** This method is called when the period time has expired for its interval.

        Where:
            currentTick     - is current system when the interval method is called
            currentInterval - is the deterministic interval boundary that is being
                              logically executed.

        Example:
            Given a interval method is scheduled to execute at 10Hz and the
            current system time in the Runnable object's forever loop is 10.212
            seconds when the interval method is called THEN:

                currentTick:=      10212000 us
                currentInterval:=  10200000 us
      */
    virtual void execute( uint64_t currentTick, uint64_t currentInterval ) noexcept = 0;

public:
    /// Period duration in microseconds
    uint64_t                m_duration;

    /// Time, in microseconds, of the Period's last interval/execution time
    uint64_t                m_timeMarker;

public:
    /// Virtual destructor
    virtual ~PeriodApi() {}
};


};      // end namespaces
};
#endif  // end header latch
