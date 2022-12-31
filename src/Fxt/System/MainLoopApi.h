#ifndef Fxt_Chassis_MainLoopApi_h_
#define Fxt_Chassis_MainLoopApi_h_
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


#include "Cpl/System/EventFlag.h"
#include "Cpl/System/Runnable.h"
#include "Cpl/System/Signable.h"

///
namespace Fxt {
///
namespace System {


/** This abstract class defines the interface for a 'main-loop'. The 
    main-loop is 'polled' driven, i.e it waits (block or busy - depends on
    the concrete implementation) for its 'tick' expire, then executes
    a iteration of the its 'loop' before waiting again on its 'tick' to 
    expired.  

    NOTE: The actual execution of the main loop is NOT guaranteed to be rate
          monotonic.  The realized timing is determined by the child class
          providing the tick delay and what processing occurs during a loop
          interaction.
    
    The main loop does have pseudo event semantic, except that the
    events don't wake-up/unblock/unwait the loop - the event processing is
    deferred until the tick has expired. Possible 'events' are:
        1. The main-loop/runnable-object is signaled
        2. Event Flags is set (see Cpl::System::EventFlag)

    NOTE: All of the PUBLIC method come from the parent classes.
 */
class MainLoopApi : public Cpl::System::Runnable, 
    public Cpl::System::EventFlag, 
    public Cpl::System::Signable
{
public:
    /// Virtual destructor
    virtual ~MainLoopApi() {}
};



};      // end namespaces
};
#endif  // end header latch
