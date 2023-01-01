#ifndef Fxt_Chassis_Server_h_
#define Fxt_Chassis_Server_h_
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


#include "Cpl/Itc/Mailbox.h"
#include "Cpl/Itc/CloseSync.h"
#include "Fxt/System/PeriodicScheduler.h"
#include "Fxt/System/ElapsedTime.h"

///
namespace Fxt {
///
namespace Chassis {


/** This concrete template class providing the Chassis timing, scheduling, and
    'executing' of the Chassis's Scanners and ExecutionSets.  There is one-to-one
    mapping of Chassis to a Chassis Server to a Thread.

    The Server supports ITC messaging as well as Synchronous open/close methods
    to start/stop the chassis executing

    Template Args: 
        TICKSOURCE - is required to be a child class of Cpl::System::MainLoopApi
 */
template <class TICKSOURCE>
class Server : public TICKSOURCE, 
    public Cpl::Itc::Mailbox,
    public Fxt::System::PeriodicScheduler,
    public Cpl::Itc::CloseSync
{
public:
    /** Constructor.  The argument 'timingTickInMicroseconds' specifies the 
        tick timing resolution.
     */
    Server( uint64_t                                                timingTickInMicroseconds,
            Fxt::System::PeriodicScheduler::ReportSlippageFunc_T    slippageFunc          = nullptr,
            Cpl::System::SharedEventHandlerApi*                     eventHandler          = nullptr ) noexcept
        : TICKSOURCE( timingTickInMicroseconds, eventHandler )
        , Cpl::Itc::Mailbox( *((Cpl::System::Signable*) this) )
        , Cpl::System::PeriodicScheduler( slippageFunc )
        , Cpl::Itc::CloseSync( *this )
    {
    }


protected:
    /// See Cpl::System::Runnable (adds ITC messaging support to the TICKSOURCE)
    void appRun()
    {
        startMainLoop();
        bool run = true;
        while ( run )
        {
            run = waitAndProcessEvents();
            if ( run )
            {
                processMessages();
                m_scheduler.executeScheduler( ElapsedTime::now() );
            }
        }
        stopMainLoop();
    }

protected:
    /// Starts the Chassis. The Chassis can be opened/closed multiple times
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
    {
        // TODO: Create List of Periods, i.e. executionSets and scanner instances
        //       ? Can pass the list of Periods in the OpenMsg
        m_scheduler.start( arrayOfPeriods );
        msg.returnToSender();
    }

    /// Stops the Chassis
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
    {
        m_scheduler.stop();
        msg.returnToSender();
    }

protected:
    /// Periodic scheduler
    PeriodicScheduler   m_scheduler;
};



};      // end namespaces
};
#endif  // end header latch
