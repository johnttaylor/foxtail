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
    @param COMPONENT_FAILURE                One or more components failed during execution
    @param TOO_MANY_COMPONENTS              Attempted to add more components that what was specified when the LC was constructed
    @param MISSING_COMPONENTS               At least one or more components where not added to the LC (as defined by the number specified in the LC constructor)
    @param FAILED_START                     Unable to start one or more components
    @param PARSE_COMPONENT_ARRAY            JSON input does not contain "components"/array 
    @param NO_COMPONENTS                    JSON Input has an empty array of "components"
    @param NO_MEMORY_LOGIC_CHAIN            Unable to allocate memory for the Logic Chain instance
    @param FAILED_CREATE_COMPONENT          Failed to create one or more components
    @param FAILED_POINT_RESOLVE             One or more Components failed when resolving their' point references
 */
BETTER_ENUM( Err_T, uint8_t
             , SUCCESS = 0
             , NO_MEMORY_COMPONENT_LIST
             , COMPONENT_FAILURE
             , TOO_MANY_COMPONENTS
             , MISSING_COMPONENTS
             , FAILED_START
             , PARSE_COMPONENT_ARRAY
             , NO_COMPONENTS
             , NO_MEMORY_LOGIC_CHAIN
             , FAILED_CREATE_COMPONENT
             , FAILED_POINT_RESOLVE
);

/** This method has 'PACKAGE Scope' in that is should only be called by
    other classes in the Fxt namespace.  It is ONLY public to avoid
    the tight coupling of C++ friend mechanism.
        
    This function is used to help with convert a full qualified error code
    into text
 */
void errorCodetoText_( Cpl::Text::String& buffer, Fxt::Type::Error errorCode, uint8_t levelIndex ) noexcept;

/// Helper method to build a 'complete/full' error from a card specific error code
inline Fxt::Type::Error fullErr( Err_T localErr )
{
    return Fxt::Type::Error( Fxt::Type::Err_T::CARD, localErr );
}



};      // end namespaces
};
#endif  // end header latch
