#ifndef Fxt_LogicChain_Api_h_
#define Fxt_LogicChain_Api_h_
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


#include "Cpl/Container/Item.h"
#include "Cpl/Type/Guid.h"
#include "Fxt/Type/Error.h"
#include "Fxt/Point/BankApi.h"
#include "Fxt/Point/Api.h"
#include "Cpl/Memory//ContiguousAllocator.h"
#include "Cpl/Json/Arduino.h"
#include <stdint.h>



///
namespace Fxt {
///
namespace LogicChain {


/** This abstract class defines operations that can be on Logic Chain

    Note: Logic Chain semantics are NOT thread-safe.

 */
class Api
{
public:
    /** This method is used to start/initialize the Logic Chain for execution.
        If the Logic Chain fails to be started the method returns an error code;
        else Fxt::Type::Err_T::SUCCESS is returned.

        The 'currentElapsedTimeUsec' argument represents the current elapsed
        time in microseconds since power-up of the application.

        A Logic Chain can be started/stopped multiple times, however it should only
        started/stopped when its containing Chassis is started/stopped. When
        a Logic Chain is created it is in the 'stopped' state.

        Restarting (after being stopped) a Logic Chain will clear any existing
        error conditions.
     */
    virtual Fxt::Type::Error start( uint64_t currentElapsedTimeUsec ) noexcept = 0;

    /** This method is used to stop/deactivate the Logic Chain.  If the Logic Chain fails
        to be stopped the method returns an error code; else Fxt::Type::Err_T::SUCCESS
        is returned.

        A Logic Chain MUST be in the 'stopped state' before it can be deleted/destroyed
     */
    virtual void stop() noexcept = 0;

    /** This method returns true if the Logic Chain is in the started state
     */
    virtual bool isStarted() const noexcept = 0;


public:
    /** This method is called to have a Logic execute is contained Components.
        It should be called periodically by the 'Chassis' object

        The 'currentTickUsec' argument represents the current elapsed time
        in microseconds since power-up of the application.  For given execution
        cycle,

        This method return Fxt::Type::Err_T::SUCCESS if the Logic Chain completed
        ALL of its logic for the current processing cycle; else if error occurred
        then an error code is returned. Once the Logic Chain has encountered
        an error, subsequence calls to this method will always fail.
     */
    virtual Fxt::Type::Error execute( int64_t currentTickUsec ) noexcept = 0;


public:
    /** This method returns the current error state of the Logic.  A value
        of Fxt::Type::Err_T::SUCCESS indicates the Logic Chain is operating
        properly
     */
    virtual Fxt::Type::Error getErrorCode() const noexcept = 0;



public:
    /** This method attempts to parse the provided JSON Object that represents
        a Logic Chain and create the contained components.  If there is an error
        (e.g. component not supported, missing key/value pairs, etc.) the
        method returns nullptr; else a pointer to the newly created Logic Chain
        object is returned.  When an error occurs, details about the specific
        card error is returned via 'logicChainErrorode' argument.
     */
    static Api* createLogicChainfromJSON( JsonVariant                        logicChainObject,
                                          Fxt::Point::BankApi&               statePointBank,
                                          Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                          Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                                          Fxt::Point::DatabaseApi&           dbForPoints,
                                          Fxt::Type::Error&                  logicChainErrorode ) noexcept;
public:
    /// Virtual destructor to make the compiler happy
    virtual ~Api() {}
};


};      // end namespaces
};
#endif  // end header latch
