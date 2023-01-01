#ifndef Fxt_System_PeriodicScheduler_h_
#define Fxt_System_PeriodicScheduler_h_
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

#include "Fxt/System/ElapsedTime.h"
#include "Fxt/System/PeriodApi.h"



///
namespace Fxt {
///
namespace System {

/** This concrete class is a 'policy' object that is used to add polled based,
    cooperative monotonic scheduling to a Runnable object.  Monotonic in this
    content means that if a period is scheduled to execute at a periodic rate 
    of 200Hz, then the method will be called on every 5ms boundary of the 
    system time.

    The scheduler makes its best attempt at being monotonic and deterministic,
    BUT because the scheduling is polled and cooperative (i.e. its is the
    application's responsibility to not-overrun/over-allocate the processing
    done during each period) the actual the timing cannot be guaranteed.
    That said, the scheduler will detect and report when the period timing
    slips. 

    The usage of this class is to implement (or extended) a Cpl::System::Runnable
    object, i.e. invoked inside the 'forever' loop of the Runnable object's appRun()
    method.  The scheduler can be use within a 'bare' forever loop and it can be
    used with existing event based Runnable objects to add periodic scheduling.
 */
class PeriodicScheduler
{
    /** Defines the method that is used to report to the Application when an
        Period does not execute 'on time'.  When the method returns true
        the scheduler continues; else the Scheduler puts itself into the
        stopped state.

        Where:
            periodThatSlipped  - Reference to the period that did not execute 
                                 on time
            currentTick        - The system time when the scheduler executed 
                                 the period
            missedInterval     - The period boundary that was missed
            periodContext      - Application defined argument/value that was
                                 passed to the period method.

     */
    typedef bool (*ReportSlippageFunc_T)(PeriodApi& periodThatSlipped,
                                         uint64_t   currentTick,
                                         uint64_t   missedInterval );

public:
    /** Constructor. The application provides a variable length array of period
        definitions that will be scheduled.  The last entry in the
        array must be a nullptr. The Scheduler assumes that each PeriodApi
        definition has a unique period time.

        Notes:
            - When extending a event based (i.e. inherits from MainLoop) runnable
              object with scheduling, the Application should define all of the
              scheduled Periods to have period times that are multiples of
              the MainLoop's 'timeOutPeriodInMicroseconds' constructor value.
     */
    PeriodicScheduler( ReportSlippageFunc_T slippageFunc = nullptr );

public: 
    /** This method starts the scheduler.  This method must be called BEFORE
        executeScheduler() is called. The scheduler can be started/stopped
        multiple times.

        The caller provides an array of PeriodApi pointers. The last entry in 
        the array MUST be a nullptr (i.e. end-of-entries)
     */
    virtual void start( PeriodApi** arrayOfPeriods ) noexcept;

    /** This method is used to invoke the scheduler.  When called zero or more
        Period definitions will be executed.  

        If a scheduled Period does not execute 'on time', then the reportSlippage()
        method will called.  It is the Application's to decide (what if anything)
        is done when there is slippage in the scheduling. The slippage is reported
        AFTER the Period's PeriodCallbackFunc_T is called.
     */
    virtual void executeScheduler( uint64_t currentTick );


    /** This method stopped the scheduler.  The scheduler can be started/stopped
        multiple times.
     */
    virtual void stop() noexcept;


protected:
    /** Helper method to Round DOWN to the nearest 'period' boundary.
        A side effect the rounding-down is the FIRST execution of an period
        will NOT be accurate (i.e. will be something less than 'periodMs').
     */
    void setTimeMarker( PeriodApi& period, uint64_t currentTick ) noexcept;

protected:
    /// List of Periods.  The last entry in the array MUST be a nullptr.
    PeriodApi**             m_periods;

    /// Report slippage method
    ReportSlippageFunc_T    m_reportSlippage;

    /// Flag to managing the 'first' execution
    bool                    m_firstExecution;
};

};      // end namespaces
};
#endif  // end header latch
