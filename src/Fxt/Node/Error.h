#ifndef Fxt_Node_Error_h_
#define Fxt_Node_Error_h_
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




///
namespace Fxt {
///
namespace Node {

/** This enumeration defines error codes that are unique to the Node Sub-system

        Note: Each error code symbol must be less than 32 characters
           123456789 123456789 12345678 12

    @param Err_T                            Enum

    @param SUCCESS                          NO ERROR
    @param FAILED_HEAP_ALLOCATIONS          Failed to allocate the Node's internal/private heaps
    @param NOT_ME                           JSON input specifies a 'different' Node
    @param TOO_MANY_CHASIS                  JSON input specifies more Chassis than the node supports
    @param NO_MEMORY_CHASSIS_LIST           Memory allocation for Chassis list failed
    @param MISSING_CHASSIS                  At least one or more Chassis where not added to the Node (as defined by the number specified in the Node constructor)
    @param CHASSIS_FAILED_START             One or more Chassis failed to start
    @param TOO_MANY_CHASSIS                 Attempted to add more Chassis that what was specified when the Node was constructed
    @param PARSE_CHASSIS_ARRAY              JSON input does not contain "chassis"/array
    @param MAX_CHASSIS_EXCEEDED             JSON input specified MORE chassis than the Node supports
    @param NO_MEMORY_NODE                   Unable to allocate memory for the Node
    @param NODE_CREATE_ERROR                The Node was not successfully created
    @param FAILED_CREATE_CHASSIS_SERVER     Unable to create a Chassis Server instance
    @param FAILED_CREATE_CHASSIS            Failed to create one or more Chassis
    @param CHASSIS_CREATE_ERROR             One or more Chassis were not successfully created

 */
BETTER_ENUM( Err_T, uint8_t
             , SUCCESS = 0
             , FAILED_HEAP_ALLOCATIONS
             , NOT_ME
             , TOO_MANY_CHASIS
             , NO_MEMORY_CHASSIS_LIST
             , MISSING_CHASSIS
             , CHASSIS_FAILED_START
             , TOO_MANY_CHASSIS
             , PARSE_CHASSIS_ARRAY
             , MAX_CHASSIS_EXCEEDED
             , NO_MEMORY_NODE
             , NODE_CREATE_ERROR
             , FAILED_CREATE_CHASSIS_SERVER
             , FAILED_CREATE_CHASSIS
             , CHASSIS_CREATE_ERROR
);

/** This concrete class defines the Error Category for the Logic Chain namespace.
    This class is designed as SINGLETON.
 */
class ErrCategory : public Fxt::Type::ErrorCategory<Err_T>
{
public:
    /// Constructor
    ErrCategory( Fxt::Type::ErrorBase& parent = Fxt::Type::Error::getErrorCategoriesRoot() )
        :Fxt::Type::ErrorCategory<Err_T>( "NODE", 1, parent )
    {
    }

public:
    /// Singleton
    static ErrCategory g_theOne;
};

/// Helper method to build a 'complete/full' error from a specific error code
inline Fxt::Type::Error fullErr( Err_T localErr )
{
    return Fxt::Type::Error( ErrCategory::g_theOne, localErr );
}

};      // end namespaces
};
#endif  // end header latch
