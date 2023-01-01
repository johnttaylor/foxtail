#ifndef Fxt_System_MainLoop_h_
#define Fxt_System_MainLoop_h_
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

#include "Fxt/System/MainLoopApi.h"
#include "Cpl/System/SharedEventHandler.h"


///
namespace Fxt {
///
namespace System {

/** This mostly concrete class implements the MainLoop Interface. A Child class 
    is required to provide the 'tick' timing.
 */
class MainLoop : public MainLoopApi
{
public:
    /** Constructor. The 'timeOutPeriodInMsec' parameter specifies how
        long the MainLoop will wait for an event before timing out and
        processing the software timers.  The value of 'timeOutPeriodInMsec' is
        the resolution of the timing source for the Software Timers, i.e. the
        event loop will 'wake-up' at least every 'timeOutPeriodInMsec'
        milliseconds.

        A fatal error is generated if 'timeOutPeriodInMsec' is set to zero.
     */
    MainLoop( Cpl::System::SharedEventHandlerApi* eventHandler = 0 );

protected:
    /** This method waits a single 'tick' duration. A child class is responsible
        for providing the basic tick timing.
     */
    virtual void waitTickDuration() noexcept = 0;

protected:
    /** This method is used to initialize the Main Loop's thread has started
        to executed.

        This method is intended to be used by child classes that are extending
        the Main Loop.  For this use case - this method MUST be called once on
        the beginning of the appRun() method and BEFORE the "main loop" for the
        appRun() method is entered. 
     */
    virtual void startMainLoop() noexcept;

    /** This method is used to wait (and process) the next event(s).  This
        method returns after being woken up and processing all timer expired
        and pending Event Flag events. This method should always be wrapped
        in a loop (typically a forever loop).

        The method typically returns true.  The method only returns false if
        the pleaseStop() method was called on the Main Loop instance.

        This method is intended to be used by child classes that are extending
        the Main Loop.  For this use case - this method MUST be called inside
        the child class's "main loop" in its the appRun() method.

        Example appRun() for a child class extending the Main Loop:
        @code

        void ChildMainLoop::appRun( void )
        {
            startMainLoop();
            <child specific intialization>

            bool run = true;
            while( run )
            {
                run = waitAndProcessEvents();
                if ( run )
                {
                    <child specific event processing>
                }
            }
            stopMainLoop()
        }

        @endcode
     */
    virtual bool waitAndProcessEvents() noexcept;

    /** This method is used to clean-up the Main Loop's when the thread is
        being stopped.

        This method is intended to be used by child classes that are extending
        the Main Loop.  For this use case - this method MUST be called once
        AFTER the event-processing loop has exited.  
     */
    virtual void stopMainLoop() noexcept;

protected:
    /** This method is used (by the concrete child class(es)) to process one
        or more Event Flags.  This method is called when at least one Event
        Flag was set. The method is called N consecutive times - one call for
        each Event Flag that is set.  The 'eventNumber' (which is zero based)
        identifies which Event Flag is/was set.

        If no ShareEventHandler was provided when the event loop was created,
        the default implementation of this method does NOTHING; else the
        ShareEventHandler instance is used to process the event flag/number.
     */
    virtual void processEventFlag( uint8_t eventNumber ) noexcept;

public:
    /// See Cpl::System::Signable
    int signal( void ) noexcept;

    /// See Cpl::System::Signable
    int su_signal( void ) noexcept;


public:
    /// See Cpl::System::Runnable
    void pleaseStop();


protected:
    /// See Cpl::System::Runnable
    void appRun();

public:
    /// See Cpl::System::EventFlag
    void notifyEvents( Cpl_System_EventFlag_T events ) noexcept;

    /// See Cpl::System::EventFlag
    void notify( uint8_t eventNumber ) noexcept;

    /// See Cpl::System::EventFlag
    void su_notifyEvents( Cpl_System_EventFlag_T events ) noexcept;

    /// See Cpl::System::EventFlag
    void su_notify( uint8_t eventNumber ) noexcept;

public:
    /// See Cpl::System::Runnable
    void setThreadOfExecution_( Cpl::System::Thread* myThreadPtr );

protected:
    /// A pointer to the thread the Main Loop executes in
    Cpl::System::Thread*                    m_myThreadPtr;

    /// My shared event handler (if I have one)
    Cpl::System::SharedEventHandlerApi*     m_eventHandler;

    /// The variable holds the current state of all Event Flags
    Cpl_System_EventFlag_T                  m_events;

    /// Flag used to help with the pleaseStop() request.
    bool                                    m_run;
};

};      // end namespaces
};
#endif  // end header latch
