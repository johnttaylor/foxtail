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
#include "Fxt/Point/CreateFuncDatabaseApi.h"
#include "Fxt/Point/BankApi.h"
#include "Fxt/Point/Api.h"
#include "Cpl/Memory/ContiguousAllocator.h"
#include "Cpl/Json/Arduino.h"
#include "Fxt/Component/FactoryDatabaseApi.h"
#include <stdint.h>



///
namespace Fxt {
///
namespace LogicChain {


/** This abstract class defines operations that can be on Logic Chain

    NOTE: Logic Chain semantics are NOT thread-safe.

    NOTE: Connector Points are used to 'connect' Components together.  In general
          every Component Input/Output points reference a connector point.  The
          exception are:
            - A Component Input can reference a Virtual (input) Point from an IO Card 
            - A Component Output can reference a Virtual (output) Point to a an IO Card

   \code
    Required/Defined JSON fields/structure:

    "logicChains": [
            {
              "name":           "<human readable name for the Logic Chain - not required to be unique>",
              "id":             <Local ID for the Logic Chain.  Range: 0-64K.  NOTE: Not consumed by the firmware>,
              "components":[    <array of components>  
                {....},
                ...
              ],
              "numPtSetters":   <number of connection points that have a default value. Range: 0-64K>,
              "connectionPts":[ <Array of connector Points for the LC>
                {
                  "name":           "<human readable name for the connector point">,
                  "type":           "<OPTIONAL: human readable point type>",
                  "id": 4294967295  <Point ID>,
                  "initial": {           // OPTIONAL initial value/state specifier for the Point
                    "valid": true|false  // Initial valid state for the internal point
                    "val":               <point value as defined by the Point type's fromJSON syntax - only required when 'valid' is 'true'>
                    "id":                <The ID of the internal 'setter' point that is used store the initial value in binary form>
                 }
                },
                ...
              ]
            },
            ...
    ]

    \endcode
 */
class Api
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
    /** This method is used to add Component to the logic chain.  The order a
        component is added to the LC, is the order that the components execute
        in.  If the add is successful then Fxt::Type::Err_T::SUCCESS is returned;
        else and error code is returned.
     */
    virtual Fxt::Type::Error add( Fxt::Component::Api& componentToAdd ) noexcept = 0;

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
    static Api* createLogicChainfromJSON( JsonVariant                         logicChainObject,
                                          Fxt::Component::FactoryDatabaseApi& componentFactory,
                                          Fxt::Point::BankApi&                statePointBank,
                                          Cpl::Memory::ContiguousAllocator&   generalAllocator,
                                          Cpl::Memory::ContiguousAllocator&   statefulDataAllocator,
                                          Fxt::Point::DatabaseApi&            dbForPoints,
                                          Fxt::Point::CreateFuncDatabaseApi&  pointCreateFuncDb,
                                          Fxt::Type::Error&                   logicChainErrorode ) noexcept;
public:
    /// Virtual destructor to make the compiler happy
    virtual ~Api() {}
};


};      // end namespaces
};
#endif  // end header latch