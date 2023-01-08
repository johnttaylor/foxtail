#ifndef Fxt_Node_Api_h_
#define Fxt_Node_Api_h_
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


#include "Fxt/Type/Error.h"
#include "Fxt/Chassis/Api.h"
#include "Fxt/Point/DatabaseApi.h"
#include "Cpl/System/Thread.h"
#include "Cpl/Json/Arduino.h"
#include <stdint.h>



///
namespace Fxt {
///
namespace Node {


/** This abstract class defines operations that can be performed on Node

    NOTE: Node semantics are NOT thread-safe.
 */
class Api
{
public:
    /** This method is used to start/run the Node's chassis. If the node fails
        to be started the method returns false; else true is returned. The Node
        can be  started/stopped multiple times. When a Node is created it is
        in the 'stopped' state.

        The 'currentElapsedTimeUsec' argument represents the current elapsed
        time in microseconds since power-up of the application.
    */
    virtual bool start( uint64_t currentElapsedTimeUsec ) noexcept = 0;

    /** This method is used to stop/deactivate the Node.  A Node MUST be
        in the 'stopped state' before it can be deleted/destroyed
     */
    virtual void stop() noexcept = 0;

    /** This method returns true if the Node is in the started state
     */
    virtual bool isStarted() const noexcept = 0;


public:
    /** This method returns the Node's GUID (that identifies its type) as a
        text string in 8-4-4-4-12 format
     */
    virtual const char* getTypeGuid() const noexcept = 0;

    /// Returns the Node's 'human readable' type name (note: this is NOT guaranteed to be unique)
    virtual const char* getTypeName() const noexcept = 0;

public:
    /// Returns the Node's general allocator
    virtual Cpl::Memory::ContiguousAllocator&   getGeneralAlloactor() noexcept = 0;

    /// Returns the Node's Card Stateful allocator
    virtual Cpl::Memory::ContiguousAllocator&   getCardStatefulAlloactor() noexcept = 0;

    /// Returns the Node's HA Stateful allocator
    virtual Cpl::Memory::ContiguousAllocator&   getHaStatefulAlloactor() noexcept = 0;


public:
    /**  Returns the total number of Chassis instances. If the Node is in an 
         error state, then zero is returned;
     */
    virtual uint16_t getNumChassis() const noexcept = 0;

    /** Returns a pointer to the specified chassis.  If not a valid index or 
        the Node is in an error state, then nullptr is returned.
     */
    virtual Fxt::Chassis::Api* getChassis( uint16_t chassisIndex ) noexcept = 0;

public:
    /** This method is used to add a Chassis to the Node.  If the add is
        successful then Fxt::Type::Err_T::SUCCESS is returned; else and error
        code is returned.

        Note: Node will destroy the thread when the Node is deleted.
     */
    virtual Fxt::Type::Error add( Fxt::Chassis::Api&       chassisToAdd,
                                  Cpl::System::Thread&     chassisThreadToAdd ) noexcept = 0;

public:
    /** This method returns the current error state of the Chassis.  A value
        of Fxt::Type::Err_T::SUCCESS indicates the Chassis is operating
        properly
     */
    virtual Fxt::Type::Error getErrorCode() const noexcept = 0;

public:
    /** This method creates a Chassis server (aka runnable object) and the 
        thread that the Chassis to execute in. When successful a pointer to the 
        Chassis Server's Open/Close interface is returned; else nullptr is 
        returned.  When an error occurs - the details are returned view 
        'errorCode'

        A pointer to the Chassis' ServerApi instance is returned via
        'serverApi' argument.  If the method fails than 'serverApi'
        has no meaning.
     */
    virtual Cpl::System::Thread* createChassisThread( Fxt::Chassis::ServerApi*& serverApi ) noexcept = 0;

    /** This method is used to destroy the thread created by createChassisThread()
     */
    virtual void destroyChassisThread( Cpl::System::Thread& chassisThreadToDelete ) noexcept = 0;

public:
    /** Returns a pointer to the one-and-only created Node instance for the platform.
        If the Node has not yet been created (or has been destroyed), then nullptr is 
        returned.
     */
    static Api* getNode() noexcept;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~Api() {}
};


}       // end namespaces
}
#endif  // end header latch
