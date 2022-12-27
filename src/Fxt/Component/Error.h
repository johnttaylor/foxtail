#ifndef Fxt_Component_Error_h_
#define Fxt_Component_Error_h_
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
namespace Component {

/** This enumeration defines error codes that are unique to the Component Sub-system

        Note: Each error code symbol must be less than 32 characters
           123456789 123456789 12345678 12

    @param Err_T                            Enum

    @param SUCCESS                          NO ERROR
    @param UNKNOWN_GUID                     Configuration contains the unknown/unsupported component type GUID 
    @param TOO_MANY_INPUT_REFS              Too many input references
    @param BAD_INPUT_REFERENCE              Bad or Missing Input reference
    @param TOO_MANY_OUTPUT_REFS             Too many output references
    @param BAD_OUTPUT_REFERENCE             Bad or Missing output reference
    @param UNRESOLVED_INPUT_REFRENCE        Failed to resolve input references
    @param UNRESOLVED_OUTPUT_REFRENCE       Failed to resolve output references
    @param OUT_OF_MEMORY                    Insufficient memory to allocate the component
    @param FAILED_START                     Failed to start the component (e.g. already running)
    @param INPUT_REFRENCE_BAD_TYPE          Input Point Reference is the wrong type
    @param OUTPUT_REFRENCE_BAD_TYPE         Output Point Reference is the wrong type
    @param DIGITAL                          A Digital Component sub-system specific error
 */
BETTER_ENUM( Err_T, uint8_t
             , SUCCESS = 0
             , UNKNOWN_GUID
             , TOO_MANY_INPUT_REFS
             , BAD_INPUT_REFERENCE
             , TOO_MANY_OUTPUT_REFS
             , BAD_OUTPUT_REFERENCE
             , UNRESOLVED_INPUT_REFRENCE
             , UNRESOLVED_OUTPUT_REFRENCE
             , OUT_OF_MEMORY
             , FAILED_START
             , INPUT_REFRENCE_BAD_TYPE
             , OUTPUT_REFRENCE_BAD_TYPE
             , DIGITAL
);

/** This method has 'PACKAGE Scope' in that is should only be called by
    other classes in the Fxt namespace.  It is ONLY public to avoid
    the tight coupling of C++ friend mechanism.
        
    This function is used to help with convert a full qualified error code
    into text
 */
void errorCodetoText_( Cpl::Text::String& buffer, Fxt::Type::Error errorCode, uint8_t levelIndex ) noexcept;

/// Helper method to build a 'complete/full' error from a component specific error code
inline Fxt::Type::Error fullErr( Err_T localErr )
{
    return Fxt::Type::Error( Fxt::Type::Err_T::COMPONENT, localErr );
}


};      // end namespaces
};
#endif  // end header latch
