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

#include "PeriodicScheduler.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Assert.h"

#define SECT_ "Fxt::System"

///
using namespace Fxt::System;

/////////////////////
PeriodicScheduler::PeriodicScheduler( ReportSlippageFunc_T slippageFunc )
    : m_periods( nullptr )
    , m_reportSlippage( slippageFunc )
    , m_firstExecution( true )
{
}

/////////////////////
void PeriodicScheduler::start( PeriodApi** arrayOfPeriods ) noexcept
{
    m_firstExecution = true;
    m_periods        = arrayOfPeriods;
}

void PeriodicScheduler::stop() noexcept
{
    m_periods = nullptr;
}

void PeriodicScheduler::executeScheduler( uint64_t currentTick )
{
    if ( m_periods )
    {
        PeriodApi* period = m_periods[0];

        // Scan all Periods
        while ( period )
        {
            // Initialize the Periods' time (but only once)
            if ( m_firstExecution )
            {
                period->m_timeMarker = currentTick;
            }

            // Has the period expired?
            if ( ElapsedTime::expired( period->m_timeMarker, period->m_duration, currentTick ) )
            {
                period->m_timeMarker += period->m_duration;
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Executing Period: interval=%lu, tick=%lu, period=%p, dur=%lu",
                                               (unsigned long) period->m_timeMarker,
                                               (unsigned long) currentTick,
                                               period,
                                               (unsigned long) period->m_duration) );

                if ( period->execute( currentTick, period->m_timeMarker ) == false )
                {
                    // The period encountered a fatal error -->STOP the scheduler
                    stop();
                    return;
                }

                // Check for slippage
                if ( ElapsedTime::expired( period->m_timeMarker, period->m_duration, currentTick ) )
                {
                    // Report the slippage to the application
                    if ( m_reportSlippage )
                    {
                        CPL_SYSTEM_TRACE_MSG( SECT_, ("Slippage: interval=%lu, tick=%lu, period=%p, dur=%lu",
                                                       ,
                                                       (unsigned long) period->m_timeMarker,
                                                       (unsigned long) currentTick,
                                                       period,
                                                       (unsigned long) period->m_duration) );

                        (m_reportSlippage) (*period, currentTick, period->m_timeMarker);
                    }

                    // Re-sync the most recent past period boundary based on the actual time.
                    // Note: This operation only has a 'effect' if the slipped time is greater 
                    // than 2 duration times
                    setTimeMarker( *period, currentTick );
                }
            }

            // Get the next period
            period++;
        }

        // Clear flag now that we have properly initialized each period
        m_firstExecution = false;
    }
}

void PeriodicScheduler::setTimeMarker( PeriodApi& period, uint64_t currentTick ) noexcept
{
    // Make sure there is no divide by zero error
    uint64_t duration = period.m_duration;
    if ( duration == 0 )
    {
        period.m_timeMarker = currentTick;
        return;
    }

    // Round down to the nearest period boundary
    period.m_timeMarker = (currentTick / duration) * duration;
}
