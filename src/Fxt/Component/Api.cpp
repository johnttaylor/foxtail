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


#include "Api.h"
#include <stdint.h>

///
using namespace Fxt::Component;


const char* Api::getErrorText( Err_T errCode ) noexcept
{
    switch ( errCode )  
    {
    case FXT_COMPONENT_ERR_NO_ERROR:                    return "FXT_COMPONENT_ERR_NO_ERROR";
    case FXT_COMPONENT_ERR_TOO_MANY_INPUT_REFS:         return "FXT_COMPONENT_ERR_TOO_MANY_INPUT_REFS";
    case FXT_COMPONENT_ERR_BAD_INPUT_REFERENCE:         return "FXT_COMPONENT_ERR_BAD_INPUT_REFERENCE";
    case FXT_COMPONENT_ERR_TOO_MANY_OUTPUT_REFS:        return "FXT_COMPONENT_ERR_TOO_MANY_OUTPUT_REFS";
    case FXT_COMPONENT_ERR_BAD_OUTPUT_REFERENCE:        return "FXT_COMPONENT_ERR_BAD_OUTPUT_REFERENCE";
    case FXT_COMPONENT_ERR_UNRESOLVED_INPUT_REFRENCE:   return "FXT_COMPONENT_ERR_UNRESOLVED_INPUT_REFRENCE";
    case FXT_COMPONENT_ERR_UNRESOLVED_OUTPUT_REFRENCE:  return "FXT_COMPONENT_ERR_UNRESOLVED_OUTPUT_REFRENCE";
    case FXT_COMPONENT_ERR_UNKNOWN_GUID:                return "FXT_COMPONENT_ERR_UNKNOWN_GUID";
    case FXT_COMPONENT_ERR_MISSING_INVALID_EXE_ORDER:   return "FXT_COMPONENT_ERR_MISSING_INVALID_EXE_ORDER";
    case FXT_COMPONENT_ERR_OUT_OF_MEMORY:               return "FXT_COMPONENT_ERR_OUT_OF_MEMORY";
    case FXT_COMPONENT_ERR_INPUT_REFRENCE_BAD_TYPE:     return "FXT_COMPONENT_ERR_INPUT_REFRENCE_BAD_TYPE";
    case FXT_COMPONENT_ERR_OUTPUT_REFRENCE_BAD_TYPE:    return "FXT_COMPONENT_ERR_OUTPUT_REFRENCE_BAD_TYPE";
    default:
        return nullptr;
    }
}
