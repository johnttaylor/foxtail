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
#include "Cpl/Text/FString.h"
#include "Cpl/Text/strip.h"
#include <stdio.h>

///
using namespace Fxt::Type;


static void printCategory( ErrorBase& category, uint32_t& errCount, Cpl::Text::String& buffer )
{
    // Print out local errors
    for ( uint8_t i=1; i < 128; i++ )
    {
        Error errCode( category, i );
        const char* text = Error::toText( errCode, buffer );
        if ( strstr( text, Error::UNKNOWN_TEXT ) != nullptr )
        {
            break;
        }

        // Print error found
        printf( "%10u,  %08x,  %s\n", errCode.errVal, errCode.errVal, text );
        errCount++;
    }

    // Traverse children categories
    ErrorBase* child = category.getFirstChild();
    while ( child )
    {
        printCategory( *child, errCount, buffer );
        child = category.getNextChild( *child );
    }
}


size_t Error::printAllErrorCodes() noexcept
{
    Cpl::Text::FString<Error::MAX_TEXT_LEN> stringBuf;
    size_t errCount = 0;
    printCategory( Error::getErrorCategoriesRoot(), errCount, stringBuf );
    return errCount;
}
    