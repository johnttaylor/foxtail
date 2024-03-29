#ifndef Fxt_Node_SBC_Automation2040W_Node_h_
#define Fxt_Node_SBC_Automation2040W_Node_h_
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
#ifndef OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_1_THREAD_NAME
#define OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_1_THREAD_NAME           "CHASSIS_1"
#endif

/// The thread priority for all thread(s)
#ifndef OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_THREAD_PRIORITY
#define OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_THREAD_PRIORITY         (CPL_SYSTEM_THREAD_PRIORITY_HIGHEST+CPL_SYSTEM_THREAD_PRIORITY_LOWEST)
#endif

/// The tick timing, in microseconds, for all chassis thread(s)
#ifndef OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_TICK_TIMING
#define OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_TICK_TIMING             OPTION_FXT_SYSTEM_TICK_1MSEC_BLOCKING_TICK_DELAY_US
#endif



///
namespace Fxt {
///
namespace Node {
///
namespace SBC {
///
namespace Automation2040W {


/** This concrete class implements the 'Automation2040W' Node.  This node is intended
    to run on the Pimoroni Automation2040W board.
    
    The Node supports the following IO: 
        - Composite card with (slot 0):
            - 4 Digital outputs 
            - 3 Relay outputs
            - 4 Digital inputs
            - 3 Analog Inputs
            - 2 User buttons (on board)
            - 2 User button LEDS (on board)
            - 1 Connected LED (on board)
            - On board temperature sensor
        - A Sensor card (slot 255): external HUT31D RH/Temp sensor on a I2C bus (default I2C address)
        - All current MOCK cards

    The assumption is that the Chassis runs on a dedicate core.

 */
class Node : public Fxt::Node::Common_
{
public:
    /// Type ID for the card
    static constexpr const char*    GUID_STRING = "dce5cbcc-7d87-4e84-98b6-42b8188ab12f";

    /// Type name for the card
    static constexpr const char*    TYPE_NAME   = "Fxt::Node::SBC::Automation2040W";

    /// Maximum number of supported chassis
    static constexpr const uint8_t  MAX_ALLOWED_CHASSIS = 1;

public:
    /// Constructor
    Node( Fxt::Point::DatabaseApi&            pointDb,
          size_t                              sizeGeneralHeap,
          size_t                              sizeCardStatefulHeap,
          size_t                              sizeHaStatefulHeap,
          Cpl::System::SharedEventHandlerApi* eventHandler = nullptr)
        : Common_( MAX_ALLOWED_CHASSIS, pointDb, sizeGeneralHeap, sizeCardStatefulHeap, sizeHaStatefulHeap )
        , m_eventHandler( eventHandler )
    {
        initialize( sizeCardStatefulHeap );
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
        Fxt::Chassis::Server<Fxt::System::Tick1MsecBlocking>* runnable = new(memRunnable) Fxt::Chassis::Server<Fxt::System::Tick1MsecBlocking>( OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_TICK_TIMING, 
                                                                                                                                                nullptr,
                                                                                                                                                nullptr,
                                                                                                                                                nullptr,
                                                                                                                                                m_eventHandler );
        serverApi = runnable;

        // Create/start the thread
        Cpl::System::Thread* thread =  Cpl::System::Thread::create( *runnable, OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_1_THREAD_NAME, OPTION_FXT_NODE_SBC_AUTOMATION2040W__CHASSIS_THREAD_PRIORITY );
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
