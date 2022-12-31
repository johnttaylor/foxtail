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

///
namespace Fxt {
///
namespace Chassis {


/** This concrete template class providing the Chassis timing, scheduling, and
    'executing' of the Chassis's Scanners and ExecutionSets.  There is one-to-one
    mapping of Chassis to a Chassis Server to a Thread.

    The Server supports ITC messaging as well as Synchronous open/close methods
    to start/stop the chassis

    Template Args: 
        MAINLOOP - is required to be a child class of Cpl::System::MainLoopApi
 */
template <class MAINLOOP>
class Server : public MAINLOOP, public Cpl::Itc::CloseSync, public Cpl::Itc::Mailbox
{
public:
    /** Constructor.  The argument 'timingTickInMicroseconds' specifies the 
        tick timing resolution.
     */
    Server( uint64_t                            timingTickInMicroseconds,
            Cpl::System::SharedEventHandlerApi* eventHandler = 0 ) noexcept
        : MAINLOOP( timingTickInMicroseconds, eventHandler )
        , Cpl::Itc::Mailbox( *((Cpl::System::EventLoop*) this) )
    {
    }


protected:
    /// See Cpl::System::Runnable (adds ITC messaging support to the MAINLOOP)
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
            }
        }
        stopMainLoop();
    }

protected:
    /// Starts the Chassis. 
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
    {
        msg.returnToSender();
    }

    /// Stops the Chassis
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
    {
        msg.returnToSender();
    }
};



};      // end namespaces
};
#endif  // end header latch
