#ifndef Fxt_Node_Mock_Kestrel_Node_h_
#define Fxt_Node_Mock_Kestrel_Node_h_
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
#include "Fxt/Node/Common_.h"
#include "Fxt/Chassis/Server.h"
#include "Fxt/System/Tick1MsecBlocking.h"


/// The thread name for Chassis #1
#ifndef OPTION_FXT_NODE_MOCK_KESTREL_CHASSIS_1_THREAD_NAME
#define OPTION_FXT_NODE_MOCK_KESTREL_CHASSIS_1_THREAD_NAME      "CHASSIS_1"
#endif

/// The thread priority for all thread(s)
#ifndef OPTION_FXT_NODE_MOCK_KESTREL_CHASSIS_THREAD_PRIORITY
#define OPTION_FXT_NODE_MOCK_KESTREL_CHASSIS_THREAD_PRIORITY    (CPL_SYSTEM_THREAD_PRIORITY_HIGHEST+CPL_SYSTEM_THREAD_PRIORITY_LOWEST)
#endif

/// The tick timing, in microseconds, for all chassis thread(s)
#ifndef OPTION_FXT_NODE_MOCK_KESTREL_CHASSIS_TICK_TIMING
#define OPTION_FXT_NODE_MOCK_KESTREL_CHASSIS_TICK_TIMING        OPTION_FXT_SYSTEM_TICK_1MSEC_BLOCKING_TICK_DELAY_US
#endif



///
namespace Fxt {
///
namespace Node {
///
namespace Mock {
///
namespace Kestrel {


/** This concrete class implements the 'Kestrel' Node.  This node is intended
    to run on desktop OS and primarily used for experimentation and testing,
    i.e. NOT really for control

    All chassis threads are created at the SAME priority, tick timing and all 
    thread stack sizes are the platforms default sizes.

 */
class Node : public Fxt::Node::Common_
{
public:
    /// Type ID for the card
    static constexpr const char*    GUID_STRING = "d65ee614-dce4-43f0-af2c-830e3664ecaf";

    /// Type name for the card
    static constexpr const char*    TYPE_NAME   = "Fxt::Node::Mock::Kestrel";

    /// Maximum number of supported chassis
    static constexpr const uint8_t  MAX_ALLOWED_CHASSIS = 1;

public:
    /// Constructor
    Node( size_t                              sizeGeneralHeap,
          size_t                              sizeCardStatefulHeap,
          size_t                              sizeHaStatefulHeap,
          Cpl::System::SharedEventHandlerApi* eventHandler = nullptr)
        : Common_( MAX_ALLOWED_CHASSIS, sizeGeneralHeap, sizeCardStatefulHeap, sizeHaStatefulHeap )
        , m_eventHandler( eventHandler )
    {
    }


public:
    /// See Fxt::Node::Api
    const char* getTypeGuid() const noexcept { return GUID_STRING; }

    /// See Fxt::Node::Api
    const char* getTypeName() const noexcept { return TYPE_NAME; }

    /// See Fxt::Node::Api
    Cpl::System::Thread* createChassisThread( Fxt::Chassis::ServerApi*& serverApi ) noexcept
    {
        // Create the runnable object
        void* memRunnable = m_generalAllocator.allocate( sizeof( Fxt::Chassis::Server<Fxt::System::Tick1MsecBlocking> ) );
        if ( memRunnable == nullptr )
        {
            return nullptr;
        }
        
        // TODO: FIXME: Implement/support the Slippage callbacks!!
        Fxt::Chassis::Server<Fxt::System::Tick1MsecBlocking>* runnable = new(memRunnable) Fxt::Chassis::Server<Fxt::System::Tick1MsecBlocking>( OPTION_FXT_NODE_MOCK_KESTREL_CHASSIS_TICK_TIMING, 
                                                                                                                                                nullptr,
                                                                                                                                                nullptr,
                                                                                                                                                nullptr,
                                                                                                                                                m_eventHandler );
        serverApi = runnable;

        // Create/start the thread
        Cpl::System::Thread* thread =  Cpl::System::Thread::create( *runnable, OPTION_FXT_NODE_MOCK_KESTREL_CHASSIS_1_THREAD_NAME, OPTION_FXT_NODE_MOCK_KESTREL_CHASSIS_THREAD_PRIORITY );
        if ( waitForThreadToRun( *runnable ) == false )
        {
            // Throw an error if the thread failed to spin up.
            Cpl::System::Thread::destroy( *thread );
            thread = nullptr;
        }
        return thread;
    };

protected:
    /// Shared Event handler for a Chassis thread
    Cpl::System::SharedEventHandlerApi* m_eventHandler;
};


}       // end namespaces
}
}
}
#endif  // end header latch
