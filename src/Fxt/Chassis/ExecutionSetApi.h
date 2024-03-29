#ifndef Fxt_Chassis_ExecutionSetApi_h_
#define Fxt_Chassis_ExecutionSetApi_h_
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
#include "Fxt/LogicChain/Api.h"
#include "Fxt/Type/Error.h"
#include "Fxt/System/PeriodApi.h"
#include "Cpl/Json/Arduino.h"
#include "Cpl/Memory/ContiguousAllocator.h"

///
namespace Fxt {
///
namespace Chassis {


/** This abstract class defines the operations that can be performed on
    a ExecutionSet. A ExecutionSet is collection of Logic Chains where all of
    the contained Logic Chains are executed at the same frequency/rate.

    NOTE: ExecutionSet semantics are NOT thread-safe.

   \code
    Required/Defined JSON fields/structure:
        {
          "name":                   "*<human readable name for the ExecutionSet - not required to be unique>",
          "id":                     <*Local ID for the ExecutionSet.  Range: 0-64K>,
          "exeRateMultipler": 1,    <Execution Rate Multiplier (i.e. the ExecutionSet executes every: (multiplier * chassis.fer) microseconds>,
          "logicChains": [          // List of Logic Chains  (must be at least one). The Logic Chains are executed in the order listed
            {...},
            ...
          ]
        }

    *The field is NOT parsed/used by the firmware

    \endcode
 */
class ExecutionSetApi: public Fxt::System::PeriodApi
{
public:
    /// This method returns the ExecutionSet's Execution Rate Multiplier
    virtual size_t getExecutionRateMultiplier() const noexcept = 0;

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
        ExecutionSet fails to be started the method returns an Error code; else 
        Fxt::Type::Error::SUCESS() is returned.  Each Scanner is responsible for 
        starting its contained Logic Chains

        The 'currentElapsedTimeUsec' argument represents the current elapsed
        time in microseconds since power-up of the application.

        A ExecutionSet can be started/stopped multiple times. When a ExecutionSet
        is created it is in the 'stopped' state.
     */
    virtual Fxt::Type::Error start( uint64_t currentElapsedTimeUsec ) noexcept = 0;

    /** This method is used to stop/deactivate the ExecutionSet.  If the ExecutionSet
        fails to be stopped the method returns false; else true is returned.
        Each ExecutionSet is responsible for stopping its contained Logic Chains

        A ExecutionSet MUST be in the 'stopped state' before it can be deleted/destroyed
     */
    virtual void stop() noexcept = 0;

    /** This method returns true if the ExecutionSet is in the started state
     */
    virtual bool isStarted() const noexcept = 0;

public:
    /** This method returns the current error state of the ExecutionSet.  A value
        of Fxt::Type::Err_T::SUCCESS indicates the card is operating properly
     */
    virtual Fxt::Type::Error getErrorCode() const noexcept = 0;

public:
    /** This method is used to add a Logic Chain to the ExecutionSet.  If the
        add is successful then Fxt::Type::Err_T::SUCCESS is returned; else and
        error code is returned.
     */
    virtual Fxt::Type::Error add( Fxt::LogicChain::Api& logiChainToAdd ) noexcept = 0;

public:
    /** Returns the total number of Logic Chains instances. If the ExecutionSet 
        is in an error state, then zero is returned;
     */
    virtual uint16_t getNumLogicChains() const noexcept = 0;

    /** Returns a pointer to the specified Logic Chain.  If not a valid index or
        the ExecutionSet is in an error state, then nullptr is returned.
     */
    virtual Fxt::LogicChain::Api* getLogicChain( uint16_t logicChainIndex ) noexcept = 0;

public:
    /** This method attempts to parse the provided JSON Object that represents
        a ExecutionSet and create the contained Logic Chains.  If there is an error
        (e.g. Component not supported, missing key/value pairs, etc.) the
        method returns nullptr; else a pointer to the newly created ExecutionSet
        object is returned.  When an error occurs, details about the specific
        ExecutionSet error is returned via 'executionSetErrorode' argument.
     */
    static ExecutionSetApi* createExecutionSetfromJSON( JsonVariant                         executionSetObject,
                                                        Fxt::Component::FactoryDatabaseApi& componentFactory,
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
