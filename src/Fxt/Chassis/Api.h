#ifndef Fxt_Chassis_Api_h_
#define Fxt_Chassis_Api_h_
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
#include "Fxt/Point/FactoryDatabaseApi.h"
#include "Fxt/Component/FactoryDatabaseApi.h"
#include "Fxt/Chassis/ServerApi.h"
#include "Fxt/Chassis/ScannerApi.h"
#include "Fxt/Chassis/ExecutionSetApi.h"
#include "Cpl/Memory/ContiguousAllocator.h"
#include "Cpl/Json/Arduino.h"
#include <stdint.h>



///
namespace Fxt {
///
namespace Chassis {


/** This abstract class defines operations that can be performed on Chassis

    NOTE: Chassis semantics are NOT thread-safe.

   \code
    Required/Defined JSON fields/structure:
        {
          "name":                   "*<human readable name for the Chassis - not required to be unique>",
          "id":                     <*Local ID for the Chassis.  Range: 0-255>,
          "fer": 1,                 <Fundemental execution rate in micro seconds>,
          "sharedPts": [            // OPTIONAL list of shared Points (data that is accessible across logic chains)
            {...},
            ...
          ],
          "scanners": [             // List of IO Card scanners (must be at least one).
            {...},
            ...
          ],
          "executionSets": [        // List of Execution sets (must be at least one)
            {...},
            ...
          ]
        }

    *The field is NOT parsed/used by the firmware

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
    /** This method is used to start/activate the Chassis. If the Chassis fails
        to be started the method returns false; else true is returned. The Chassis 
        can be  started/stopped multiple times. When a Chassis is created it is 
        in the 'stopped' state.

        The 'currentElapsedTimeUsec' argument represents the current elapsed
        time in microseconds since power-up of the application.
    */
    virtual bool start( uint64_t currentElapsedTimeUsec ) noexcept = 0;

    /** This method is used to stop/deactivate the Chassis.  A Chassis MUST be 
        in the 'stopped state' before it can be deleted/destroyed
     */
    virtual void stop() noexcept = 0;

    /** This method returns true if the Chassis is in the started state
     */
    virtual bool isStarted() const noexcept = 0;


public:
    /** This method is used to add a Scanner to the Chassis.  If the add is
        successful then Fxt::Type::Err_T::SUCCESS is returned; else and error
        code is returned.
     */
    virtual Fxt::Type::Error add( ScannerApi& scannerToAdd ) noexcept = 0;

    /** This method is used to add a Execution set to the Chassis.  If the add
        is successful then Fxt::Type::Err_T::SUCCESS is returned; else and error
        code is returned.
     */
    virtual Fxt::Type::Error add( ExecutionSetApi& executionSetToAdd ) noexcept = 0;

    /** This method instructs the Chassis to 'build' it schedulers. If the operation
        is successful then Fxt::Type::Err_T::SUCCESS is returned; else and error
        code is returned.

        Note: The 'schedule' is built such that entities with smaller 'Rate 
              Multipliers' will execute BEFORE entities with larger Rate Multipliers
              when the interval boundary is such that multiple periods execute in
              the same iteration of the main loop.
     */
    virtual Fxt::Type::Error buildSchedule() noexcept = 0;

public:
    /** This method returns the Chassis Fundamental Execution Rate (FER) in
        microseconds.
     */
    virtual uint64_t getFER() const noexcept = 0;

    /** This method returns the current error state of the Chassis.  A value
        of Fxt::Type::Err_T::SUCCESS indicates the Chassis is operating
        properly
     */
    virtual Fxt::Type::Error getErrorCode() const noexcept = 0;


public:
    /** This method attempts to parse the provided JSON Object that represents
        a Chassis and create the contained elements.  If there is an error
        (e.g. component not supported, missing key/value pairs, etc.) the
        method returns nullptr; else a pointer to the newly created Chassis
        object is returned.  When an error occurs, details about the specific
        chassis error is returned via 'chassisErrorode' argument.
     */
    static Api* createChassisfromJSON( JsonVariant                         chassisJsonObject,
                                       ServerApi&                          chassisServer,
                                       Fxt::Component::FactoryDatabaseApi& componentFactory,
                                       Fxt::Card::FactoryDatabaseApi&      cardFactoryDb,
                                       Cpl::Memory::ContiguousAllocator&   generalAllocator,
                                       Cpl::Memory::ContiguousAllocator&   cardStatefulDataAllocator,
                                       Cpl::Memory::ContiguousAllocator&   haStatefulDataAllocator,
                                       Fxt::Point::FactoryDatabaseApi&     pointFactoryDb,
                                       Fxt::Point::DatabaseApi&            dbForPoints,
                                       Fxt::Type::Error&                   chassisErrorode ) noexcept;
public:
    /// Virtual destructor to make the compiler happy
    virtual ~Api() {}
};


};      // end namespaces
};
#endif  // end header latch
