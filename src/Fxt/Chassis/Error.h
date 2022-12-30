#ifndef Fxt_LogicChain_Error_h_
#define Fxt_LogicChain_Error_h_
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
namespace LogicChain {

/** This enumeration defines error codes that are unique to the Card Sub-system

        Note: Each error code symbol must be less than 32 characters
           123456789 123456789 12345678 12

    @param Err_T                            Enum

    @param SUCCESS                          NO ERROR
    @param NO_MEMORY_COMPONENT_LIST         Unable to allocate memory for list of contained components
    @param NO_MEMORY_AUTO_POINT_LIST        Unable to allocate memory for list of Auto Points
    @param COMPONENT_FAILURE                One or more components failed during execution
    @param TOO_MANY_COMPONENTS              Attempted to add more components that what was specified when the LC was constructed
    @param TOO_MANY_COMPONENTS              Attempted to add more Auto Points that what was specified when the LC was constructed
    @param MISSING_COMPONENTS               At least one or more components where not added to the LC (as defined by the number specified in the LC constructor)
    @param MISSING_AUTO_POINTS              At least one or more Auto Points where not added to the LC (as defined by the number specified in the LC constructor)
    @param FAILED_START                     Unable to start one or more components
    @param PARSE_COMPONENT_ARRAY            JSON input does not contain "components"/array 
    @param NO_COMPONENTS                    JSON Input has an empty array of "components"
    @param NO_MEMORY_LOGIC_CHAIN            Unable to allocate memory for the Logic Chain instance
    @param FAILED_CREATE_COMPONENT          Failed to create one or more components
    @param FAILED_CREATE_POINTS             Failed to create one or more connection points
    @param FAILED_CREATE_AUTO_POINTS        Failed to create one or more auto points
    @param NO_INITIAL_VAL_AUTO_POINT        One or more auto points are missing the required initial value specification
    @param COMPONENT_CREATE_ERROR           One or more components were not successfully created
    @param POINT_CREATE_ERROR               One or more connector points were not successfully created
    @param AUTO_POINT_CREATE_ERROR          One or more connector points were not successfully created
    @param FAILED_POINT_RESOLVE             One or more Components failed when resolving their' point references
 */
BETTER_ENUM( Err_T, uint8_t
             , SUCCESS = 0
             , NO_MEMORY_COMPONENT_LIST
             , NO_MEMORY_AUTO_POINT_LIST
             , COMPONENT_FAILURE
             , TOO_MANY_COMPONENTS
             , TOO_MANY_AUTO_POINTS
             , MISSING_COMPONENTS
             , MISSING_AUTO_POINTS
             , FAILED_START
             , PARSE_COMPONENT_ARRAY
             , NO_COMPONENTS
             , NO_MEMORY_LOGIC_CHAIN
             , FAILED_CREATE_COMPONENT
             , FAILED_CREATE_POINTS
             , FAILED_CREATE_AUTO_POINTS
             , NO_INITIAL_VAL_AUTO_POINT
             , COMPONENT_CREATE_ERROR
             , POINT_CREATE_ERROR
             , AUTO_POINT_CREATE_ERROR
             , FAILED_POINT_RESOLVE
);

/** This concrete class defines the Error Category for the Logic Chain namespace.
    This class is designed as SINGLETON.
 */
class ErrCategory : public Fxt::Type::ErrorCategory<Err_T>
{
public:
    /// Constructor
    ErrCategory( Fxt::Type::ErrorBase& parent = Fxt::Type::Error::getErrorCategoriesRoot() )
        :Fxt::Type::ErrorCategory<Err_T>( "LOGIC_CHAIN", 1, parent )
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
