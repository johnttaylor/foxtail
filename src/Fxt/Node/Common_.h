#ifndef Fxt_Node_Common_h_
#define Fxt_Node_Common_h_
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


#include "Fxt/Node/Api.h"
#include "Cpl/Memory/LeanHeap.h"

///
namespace Fxt {
///
namespace Node {


/** This partially concrete class provide common infrastructure for implementing
    a Node
 */
class Common_ : public Api
{
protected:
    /// Struct to associate a Chassis instance with its Thread
    struct Chassis_T
    {
        Fxt::Chassis::Api*          chassis;      //!< Chassis pointer
        Cpl::System::Thread*        thread;       //!< Chassis thread pointer
    };

protected:
    /// Constructor.  
    Common_( uint8_t                  numChassis,
             Fxt::Point::DatabaseApi& pointDb,
             size_t                   sizeGeneralHeap,
             size_t                   sizeCardStatefulHeap,
             size_t                   sizeHaStatefulHeap );

    /// Destructor
    ~Common_();

public:
    /// See Fxt::Node::Api
    bool start( uint64_t currentElapsedTimeUsec ) noexcept;

    /// See Fxt::Node::Api
    void stop() noexcept;

    /// See Fxt::Node::Api
    bool isStarted() const noexcept;

    /// See Fxt::Node::Api
    Fxt::Type::Error add( Fxt::Chassis::Api&       chassisToAdd,
                          Cpl::System::Thread&     chassisThreadToAdd ) noexcept;

    /// See Fxt::Node::Api
    Fxt::Type::Error getErrorCode() const noexcept;

    /// See Fxt::Node::Api
    Cpl::Memory::ContiguousAllocator& getGeneralAlloactor() noexcept;

    /// See Fxt::Node::Api
    Cpl::Memory::ContiguousAllocator& getCardStatefulAlloactor() noexcept;

    /// See Fxt::Node::Api
    Cpl::Memory::ContiguousAllocator& getHaStatefulAlloactor() noexcept;

    /// See Fxt::Node::Api.  Assumes the thread was created using Cpl::System::Thread::create() method
    void destroyChassisThread( Cpl::System::Thread& chassisThreadToDelete ) noexcept;

protected:
    /// Helper function that waits (but not forever) for a Chassis thread to spin up.  Returns true if the thread is running when done waiting
    bool waitForThreadToRun( Cpl::System::Runnable& runnable );

protected:
    /// Allocator|Heap: General 
    Cpl::Memory::LeanHeap               m_generalAllocator;

    /// Allocator|Heap: Card Stateful data 
    Cpl::Memory::LeanHeap               m_cardStatefulAllocator;

    /// Allocator|Heap: HA Stateful data 
    Cpl::Memory::LeanHeap               m_haStatefulAllocator;

    /// Point Database
    Fxt::Point::DatabaseApi&            m_pointDb;

    /// Array/List of Chassis
    Chassis_T*                          m_chassis;

    /// Error state. A value of 0 indicates NO error
    Fxt::Type::Error                    m_error;

    /// Number of Chassis
    uint16_t                            m_numChassis;

    /// Array index for the next Chassis add operation
    uint16_t                            m_nextChassisIdx;

    /// My started state
    bool                                m_started;

    /// Track if Point references have been resolved
    bool                                m_pointReferencesResolved;
};



};      // end namespaces
};
#endif  // end header latch
