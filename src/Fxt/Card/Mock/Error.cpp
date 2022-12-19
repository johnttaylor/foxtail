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


#include "Error.h"

using namespace Fxt::Card::Mock;

void Fxt::Card::Mock::errorCodetoText_( Cpl::Text::String& buffer, Fxt::Type::Error errorCode, uint8_t levelIndex ) noexcept
{
    buffer.formatAppend( ":%s", Fxt::Type::Error::UNKNOWN_TEXT );
}