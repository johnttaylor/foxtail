#if 0
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


#include "String_.h"
#include <string.h>


///
using namespace Cpl::Point;

///////////////////////////////////////////////////////////////////////////////
StringBase_::StringBase_()
    : PointCommon_( false )
{
}

/// Constructor. Valid Point.  Requires an initial value
StringBase_::StringBase_( const char* initialValue )
    : PointCommon_( true )
{
}

///////////////////////////////////////////////////////////////////////////////
bool StringBase_::read( Cpl::Text::String& dstData ) const noexcept
{
    int   dstLen;
    char* dstPtr = dstData.getBuffer( dstLen ); // Note: 'dstLen' does NOT include the null terminator, i.e. the actual available storage is dstLen+1
    return Cpl::Point::PointCommon_::read( dstPtr, dstLen + 1 );
}

bool StringBase_::get( Cpl::Text::String& dstData, bool& isValid, bool& isLocked ) const noexcept
{
    isValid  = m_valid;
    isLocked = m_locked;
    return read( dstData );
}

bool StringBase_::read( char* dstData, size_t dstSizeInBytes ) const noexcept
{
    return Cpl::Point::PointCommon_::read( dstData, dstSizeInBytes );
}

bool StringBase_::get( char* dstData, size_t dstSizeInBytes, bool& isValid, bool& isLocked ) const noexcept
{
    isValid  = m_valid;
    isLocked = m_locked;
    return read( dstData, dstSizeInBytes );
}

const char* StringBase_::write( const char* srcString, Cpl::Point::Api::LockRequest_T lockRequest ) noexcept
{
    // Note: The copyToFrom_() method ensures that there are no buffer/data overruns
    Cpl::Point::PointCommon_::write( srcString, strlen( srcString ), lockRequest );
    return getDataPtr();
}

size_t StringBase_::getSize() const noexcept
{
    return getMaxLength() + 1;
}

///////////////////////////////////////////////////////////////////////////////
bool StringBase_::toJSON_( JsonDocument& doc, bool verbose ) noexcept
{
    // Construct the 'val' key/value pair 
    doc["val"] = (char*)getDataPtr();
    return true;
}

bool StringBase_::fromJSON_( JsonVariant & src, Cpl::Point::Api::LockRequest_T lockRequest, Cpl::Text::String * errorMsg ) noexcept
{
    // Get the string
    const char* newValue = src;
    if ( newValue == nullptr )
    {
        if ( errorMsg )
        {
            *errorMsg = "Invalid syntax for the 'val' key/value pair";
        }
        return false;
    }

    write( newValue, lockRequest );
    return true;
}


void StringBase_::copyDataTo_( void* dstData, size_t dstSize ) const noexcept
{
    char* dstPtr = (char*)dstData;
    dstPtr[0]    = '\0';
    strncat( dstPtr, getConstDataPtr(), dstSize );
}

void StringBase_::copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
{
    char*  dstPtr = getDataPtr();
    size_t dstLen = getMaxLength();
    strncpy( dstPtr, (const char*)srcData, dstLen );
    dstPtr[dstLen] = '\0';
}
#endif