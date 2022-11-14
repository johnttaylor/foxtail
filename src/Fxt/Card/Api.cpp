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
using namespace Fxt::Card;

constexpr uint16_t Fxt::Card::Api::INVALID_ID;


const char* Api::getErrorText( Err_T errCode ) noexcept
{
    switch ( errCode )
    {
    case FXT_CARD_ERR_NO_ERROR:                  return "FXT_CARD_ERR_NO_ERROR";
    case FXT_CARD_ERR_MEMORY_DESCRIPTORS:        return "FXT_CARD_ERR_MEMORY_DESCRIPTORS";
    case FXT_CARD_ERR_UNKNOWN_GUID:              return "FXT_CARD_ERR_UNKNOWN_GUID";
    case FXT_CARD_ERR_CARD_MISSING_ID:           return "FXT_CARD_ERR_CARD_MISSING_ID";
    case FXT_CARD_ERR_POINT_MISSING_ID:          return "FXT_CARD_ERR_POINT_MISSING_ID";
    case FXT_CARD_ERR_TOO_MANY_INPUT_POINTS:     return "FXT_CARD_ERR_TOO_MANY_INPUT_POINTS";
    case FXT_CARD_ERR_TOO_MANY_OUTPUT_POINTS:    return "FXT_CARD_ERR_TOO_MANY_OUTPUT_POINTS";
    case FXT_CARD_ERR_BAD_CHANNEL_ASSIGNMENTS:   return "FXT_CARD_ERR_BAD_CHANNEL_ASSIGNMENTS";
    case FXT_CARD_ERR_CARD_SETTER_ERROR:         return "FXT_CARD_ERR_CARD_SETTER_ERROR";
    case FXT_CARD_ERR_CARD_MEMORY:               return "FXT_CARD_ERR_CARD_MEMORY";
    case FXT_CARD_ERR_MEMORY_DESCRIPTOR_NAME:    return "FXT_CARD_ERR_MEMORY_DESCRIPTOR_NAME";
    case FXT_CARD_ERR_CARD_INVALID_ID:           return "FXT_CARD_ERR_CARD_INVALID_ID";
    default:
        return nullptr;
    }
}
