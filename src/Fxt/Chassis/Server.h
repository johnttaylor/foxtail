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
#include "Cpl/System/Assert.h"
#include "Cpl/System/SharedEventHandler.h"
#include "Fxt/System/PeriodicScheduler.h"
#include "Fxt/System/ElapsedTime.h"
#include "Fxt/Chassis/ServerApi.h"

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
    public ServerApi
{
public:
    /** Constructor.  The argument 'timingTickInMicroseconds' specifies the 
        tick timing resolution.
     */
    Server( uint64_t                                                timingTickInMicroseconds,
            Fxt::System::PeriodicScheduler::ReportSlippageFunc_T    inSlippageFunc  = nullptr,
            Fxt::System::PeriodicScheduler::ReportSlippageFunc_T    exeSlippageFunc = nullptr,
            Fxt::System::PeriodicScheduler::ReportSlippageFunc_T    outSlippageFunc = nullptr,
            Cpl::System::SharedEventHandlerApi*                     eventHandler    = nullptr ) noexcept
        : TICKSOURCE( timingTickInMicroseconds, eventHandler )
        , ServerApi( *((Cpl::Itc::PostApi*) this) )
        , m_inputScheduler( inSlippageFunc )
        , m_executionScheduler( exeSlippageFunc )
        , m_outputScheduler( outSlippageFunc )
    {
    }

    /// Destructor
    ~Server() {}

protected:
    /// See Cpl::System::Runnable (adds ITC messaging support to the TICKSOURCE)
    void appRun()
    {
        TICKSOURCE::startMainLoop();
        bool run = true;
        while ( run )
        {
            run = TICKSOURCE::waitAndProcessEvents();
            if ( run )
            {
                processMessages();
                uint64_t now = Fxt::System::ElapsedTime::now();
                m_inputScheduler.executeScheduler( now );
                m_executionScheduler.executeScheduler( now );
                m_outputScheduler.executeScheduler( now );
            }
        }
        TICKSOURCE::stopMainLoop();
    }

protected:
    /// Starts the Chassis execution. The Chassis can be opened/closed multiple times
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
    {
        ChassisPeriods_T* chassisPeriods = (ChassisPeriods_T*) msg.getPayload().m_args;
        CPL_SYSTEM_ASSERT( chassisPeriods );

        // Start the schedulers
        m_inputScheduler.start( chassisPeriods->inputPeriods );
        m_executionScheduler.start( chassisPeriods->executionPeriods );
        m_outputScheduler.start( chassisPeriods->outputPeriods );

        msg.returnToSender();
    }

    /// Stops the Chassis execution
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
    {
        // Stop the schedulers
        m_outputScheduler.stop();
        m_executionScheduler.stop();
        m_inputScheduler.stop();
        msg.returnToSender();
    }

protected:
    /// Periodic scheduler for scanning inputs
    Fxt::System::PeriodicScheduler   m_inputScheduler;

    /// Periodic scheduler for executing Execution sets
    Fxt::System::PeriodicScheduler   m_executionScheduler;

    /// Periodic scheduler for flushing outputs
    Fxt::System::PeriodicScheduler   m_outputScheduler;
};



};      // end namespaces
};
#endif  // end header latch
