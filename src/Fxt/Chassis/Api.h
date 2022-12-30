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
    /** This method is used to add a Scanner to the Chassis.  If the add is
        successful then Fxt::Type::Err_T::SUCCESS is returned; else and error
        code is returned.
     */
    virtual Fxt::Type::Error add( ScannerApi& componentToAdd ) noexcept = 0;

    /** This method is used to add a Execution set to the Chassis.  If the add
        is successful then Fxt::Type::Err_T::SUCCESS is returned; else and error
        code is returned.
     */
    virtual Fxt::Type::Error add( ExecutionSetApi& autoPointToAdd ) noexcept = 0;


public:
    /** This method returns the current error state of the Chassis.  A value
        of Fxt::Type::Err_T::SUCCESS indicates the Chassis is operating
        properly
     */
    virtual Fxt::Type::Error getErrorCode() const noexcept = 0;


public:
    /** This method attempts to parse the provided JSON Object that represents
        a Chassis and create the contained components.  If there is an error
        (e.g. component not supported, missing key/value pairs, etc.) the
        method returns nullptr; else a pointer to the newly created Chassis
        object is returned.  When an error occurs, details about the specific
        card error is returned via 'chassisErrorode' argument.
     */
    static Api* createChassisfromJSON( JsonVariant                         chassisJsonObject,
                                       Fxt::Component::FactoryDatabaseApi& componentFactory,
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
