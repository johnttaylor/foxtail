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
#include "Fxt/Card/Error.h"
#include "Fxt/Component/Error.h"

///
using namespace Fxt::Type;

const char* Error::toText( Cpl::Text::String& buffer ) const noexcept
{
    // Level 1
    better_enums::optional<Err_T> maybe = Err_T::_from_integral_nothrow( (errVal&0xFF) );
    if ( !maybe )
    {
        buffer = UNKNOWN_TEXT;
    }
    else
    {
        buffer = maybe->_to_string();

        // Level 2
        switch ( *maybe )
        {
        case Err_T::CARD:
            Fxt::Card::errorCodetoText_( buffer, *this, 1 );
            break;

        case Err_T::COMPONENT:
            Fxt::Component::errorCodetoText_( buffer, *this, 1 );
            break;

        case Err_T::CHASSIS:
        case Err_T::LOGIC_CHAIN:

            // Not a nested 'sub-level' - all done
        default:
            break;
        }
    }
 
    return buffer.getString();
}


