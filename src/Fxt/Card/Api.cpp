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

const char* Api::getErrorText( uint32_t errCode ) const noexcept
{
    switch ( errCode )
    {
    case FXT_CARD_ERR_MEMORY_POINT_BANKS:        return "Unable to allocate memory for the card's Point Banks.";
    case FXT_CARD_ERR_MEMORY_INPUT_DESCRIPTORS:  return "Unable to allocate memory for the card's Input Point Descriptors.";
    case FXT_CARD_ERR_MEMORY_OUTPUT_DESCRIPTORS: return "Unable to allocate memory for the card's Output Point Descriptors.";
    case FXT_CARD_ERR_GUID_WRONG_TYPE:           return "Configuration contains the wrong GUID (i.e. the JSON object calls out a different card type).";
    case FXT_CARD_ERR_CARD_MISSING_ID:           return "Configuration does NOT contain a ID value for the card.";
    case FXT_CARD_ERR_MEMORY_CARD_NAME:          return "Unable to allocate memory for the card's name.";
    case FXT_CARD_ERR_POINT_MISSING_ID:          return "Configuration does NOT contain a LocalId value for one of it Points.";
    case FXT_CARD_ERR_TOO_MANY_INPUT_POINTS:     return "Configuration contains TOO many input Points.";
    case FXT_CARD_ERR_TOO_MANY_OUTPUT_POINTS:    return "Configuration contains TOO many output Points.";
    case FXT_CARD_ERR_BAD_CHANNEL_ASSIGNMENTS:   return "Configuration contains duplicate or our-of-range Channel IDs.";
    case FXT_CARD_ERR_CARD_MISSING_SLOT_ID:      return "Configuration does NOT contain a SlotID value for the card.";
    case FXT_CARD_ERR_CARD_MISSING_NAME:         return "Configuration does NOT contain a text Name/label for the card.";
    default:
        return nullptr;
    }
}
