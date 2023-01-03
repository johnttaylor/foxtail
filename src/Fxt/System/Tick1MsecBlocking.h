#ifndef Fxt_System_Tick1MsecBlocking_h_
#define Fxt_System_Tick1MsecBlocking_h_
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
#include "Fxt/System/MainLoop.h"

/** Specifies the tick timeout period
 */
#ifndef OPTION_FXT_SYSTEM_TICK_1MSEC_BLOCKING_TICK_DELAY_US
#define OPTION_FXT_SYSTEM_TICK_1MSEC_BLOCKING_TICK_DELAY_US       (1000ULL)  //!< 1 msec timeout
#endif

///
namespace Fxt {
///
namespace System {


/** This concrete class provides the timing source for the MainLoop interface.
    The resolution of the tick is 1 millisecond, i.e. the main loop will NOT
    execute faster than once every millisecond.  
    
    The implementation uses the Cpl::System::Api::sleep() method for waiting, 
    i.e. it is a blocking/non-busy wait.
 */
class Tick1MsecBlocking : public MainLoop
{
public:
    /// Constructor
    Tick1MsecBlocking( uint64_t                            tickTimeInMicroseconds = OPTION_FXT_SYSTEM_TICK_1MSEC_BLOCKING_TICK_DELAY_US,
                       Cpl::System::SharedEventHandlerApi* eventHandler           = 0 );

protected:
    /// See Fxt::System::MainLoop
    void waitTickDuration() noexcept;

protected:
    /// Timeout period for a single tick in milliseconds
    unsigned long  m_timeout;
};



};      // end namespaces
};
#endif  // end header latch
