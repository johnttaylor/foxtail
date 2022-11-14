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
#include "Cpl/Type/Guid.h"
#include <stdint.h>

///
using namespace Fxt::Card;

constexpr uint16_t Fxt::Card::Api::INVALID_ID;


const char* Api::getErrorText( Err_T errCode ) noexcept
{
    switch ( errCode )
    {
    case FXT_CARD_ERR_NO_ERROR:                  return "Success, no error.";
    case FXT_CARD_ERR_MEMORY_DESCRIPTORS:        return "Unable to allocate memory when parsing/creating Point descriptors.";
    case FXT_CARD_ERR_UNKNOWN_GUID:              return "Configuration contains the unknown/unsupported card type GUID";
    case FXT_CARD_ERR_CARD_MISSING_ID:           return "Configuration does NOT contain a ID value for the card.";
    case FXT_CARD_ERR_POINT_MISSING_ID:          return "Configuration does NOT contain a LocalId value for one of it Points.";
    case FXT_CARD_ERR_TOO_MANY_INPUT_POINTS:     return "Configuration contains TOO many input Points.";
    case FXT_CARD_ERR_TOO_MANY_OUTPUT_POINTS:    return "Configuration contains TOO many output Points.";
    case FXT_CARD_ERR_BAD_CHANNEL_ASSIGNMENTS:   return "Configuration contains duplicate or our-of-range Channel IDs.";
    case FXT_CARD_ERR_CARD_SETTER_ERROR:         return "Unable to parse/create a Setter point.";
    case FXT_CARD_ERR_CARD_MEMORY:               return "Unable to allocate memory for the card";
    case FXT_CARD_ERR_MEMORY_DESCRIPTOR_NAME:    return "Unable to allocate memory for a Descriptor's name";
    case FXT_CARD_ERR_CARD_INVALID_ID:           return "The Card ID is invalid or out-of-range.";
    default:
        return nullptr;
    }
}
