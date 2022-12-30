#ifndef Fxt_Chassis_ExecutionSet_h_
#define Fxt_Chassis_ExecutionSet_h_
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


#include "Fxt/Point/DatabaseApi.h"
#include "Cpl/Json/Arduino.h"
#include "Cpl/Memory/ContiguousAllocator.h"

///
namespace Fxt {
///
namespace Chassis {


/** This abstract class defines the operations that can be performed on
    a ExecutionSet
 */
class ExecutionSetApi
{
public:
    /** This method is used to resolve Point references once all of the
        configuration (i.e. all Points have been) has been processed. The
        method returns true if all references have been successfully resolved;
        else false is returned.

        This method MUST be called prior the start(...) method being called, i.e.
        a Chassis/LogicChain/Component is not ready to begin execution till all
        symbolic references have been resolved.
     */
    virtual Fxt::Type::Error resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept = 0;

public:
    /** This method is used to start/activate the ExecutionSet.  If the
        ExecutionSet fails to be started the method returns false; else true is
        returned.  Each Scanner is responsible for starting its contained Logic
        Chains

        A ExecutionSet can be started/stopped multiple times. When a ExecutionSet
        is created it is in the 'stopped' state.
     */
    virtual bool start() noexcept = 0;

    /** This method is used to stop/deactivate the ExecutionSet.  If the ExecutionSet
        fails to be stopped the method returns false; else true is returned.
        Each ExecutionSet is responsible for stopping its contained Logic Chains

        A ExecutionSet MUST be in the 'stopped state' before it can be deleted/destroyed
     */
    virtual bool stop() noexcept = 0;

    /** This method returns true if the ExecutionSet is in the started state
     */
    virtual bool isStarted() const noexcept = 0;

public:
    /** This method is called to have a ExecutionSet execute its contained
        Logic Chains. It should be called periodically by the 'Chassis' object

        The 'currentTickUsec' argument represents the current elapsed time
        in microseconds since power-up of the application.  For given execution
        cycle,

        This method return Fxt::Type::Err_T::SUCCESS if the ExecutionSet completed
        ALL of its logic for the current processing cycle; else if error occurred
        then an error code is returned. Once the ExecutionSet has encountered
        an error, subsequence calls to this method will always fail.
     */
    virtual Fxt::Type::Error execute( int64_t currentTickUsec ) noexcept = 0;

public:
    /** This method returns the current error state of the ExecutionSet.  A value
        of Fxt::Type::Err_T::SUCCESS indicates the card is operating properly
     */
    virtual Fxt::Type::Error getErrorCode() const noexcept = 0;


public:
    /** This method attempts to parse the provided JSON Object that represents
        a ExecutionSet and create the contained Logic Chains.  If there is an error
        (e.g. Component not supported, missing key/value pairs, etc.) the
        method returns nullptr; else a pointer to the newly created ExecutionSet
        object is returned.  When an error occurs, details about the specific
        ExecutionSet error is returned via 'executionSetErrorode' argument.
     */
    static Api* createExecutionSetfromJSON( JsonVariant                         executionSetJsonObject,
                                            Cpl::Memory::ContiguousAllocator&   generalAllocator,
                                            Cpl::Memory::ContiguousAllocator&   haStatefulDataAllocator,
                                            Fxt::Point::FactoryDatabaseApi&     pointFactoryDb,
                                            Fxt::Point::DatabaseApi&            dbForPoints,
                                            Fxt::Type::Error&                   executionSetErrorode ) noexcept;
public:
    /// Virtual destructor
    virtual ~ExecutionSetApi(){};
};


};      // end namespaces
};
#endif  // end header latch
