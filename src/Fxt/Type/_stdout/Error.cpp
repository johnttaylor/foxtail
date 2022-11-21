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

static size_t countNeedles( const char* haystack, const char* needle =":" )
{
    size_t count = 0;
    while ( haystack && *haystack != '\0' )
    {
        haystack = Cpl::Text::stripNotChars( haystack, needle );
        if ( *haystack == '\0' )
        {
            return count;
        }
        haystack = Cpl::Text::stripChars( haystack, needle );
        count++;
    }
    return count;
}

size_t Error::printAllErrorCodes() noexcept
{
    Cpl::Text::FString<Error::MAX_TEXT_LEN> stringBuf;
    size_t errCount = 0;
    for ( uint32_t idx0=1; idx0 < 0x100; idx0++ )
    {
        Error errCode = idx0;
        const char* text = errCode.toText( stringBuf );
        if ( strstr( text, UNKNOWN_TEXT ) != nullptr )
        {
            return errCount;
        }
        if ( strstr( text, "SUCCESS" ) == nullptr )
        {
            printf( "%10u,  %08x,  %s\n", errCode.errVal, errCode.errVal, text );
            errCount++;
        }

        // 2nd level
        for ( uint32_t idx1=1; idx1 < 0x100; idx1++ )
        {
            Error errCode2;
            errCode2.errVal = errCode.errVal | (idx1 << 8);
            text = errCode2.toText( stringBuf );
            if ( strstr( text, UNKNOWN_TEXT ) != nullptr ||
                 countNeedles( text ) < 1 )
            {
                break;
            }

            if ( strstr( text, "SUCCESS" ) == nullptr )
            {
                printf( "%10u,  %08x,  %s\n", errCode2.errVal, errCode2.errVal, text );
                errCount++;
            }

            // 3rd level
            for ( uint32_t idx2=1; idx2 < 0x100; idx2++ )
            {
                Error errCode3;
                errCode3.errVal = errCode2.errVal | (idx2 << 16);
                text = errCode3.toText( stringBuf );
                if ( strstr( text, UNKNOWN_TEXT ) != nullptr ||
                     countNeedles( text ) < 2 )
                {
                    break;
                }
                if ( strstr( text, "SUCCESS" ) == nullptr )
                {
                    printf( "%10u,  %08x,  %s\n", errCode3.errVal, errCode3.errVal, text );
                    errCount++;
                }

                // 4th level
                for ( uint32_t idx3=1; idx3 < 0x100; idx3++ )
                {
                    Error errCode4;
                    errCode4.errVal = errCode3.errVal | (idx3 << 24);
                    text = errCode4.toText( stringBuf );
                    if ( strstr( text, UNKNOWN_TEXT ) != nullptr ||
                         countNeedles( text ) < 3 )
                    {
                        break;
                    }
                    if ( strstr( text, "SUCCESS" ) == nullptr )
                    {
                        printf( "%10u,  %08x,  %s\n", errCode4.errVal, errCode4.errVal, text );
                        errCount++;
                    }
                }
            }
        }
    }

    // I sure never get here -->so return 0 as an error indication
    return 0;
}


