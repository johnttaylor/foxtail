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


#include "String.h"
#include "Cpl/System/Trace.h"
#include <string.h>

#define SECT_   "Fxt::Point"

///
using namespace Fxt::Point;


///////////////////////////////////////////////////////////////////////////////
String::String( DatabaseApi&                        db,
                uint32_t                            pointId,
                Cpl::Memory::ContiguousAllocator&   allocatorForPointStatefulData,
                size_t                              stringLenInBytesWithoutNullTerminator,
                Api*                                setterPoint )
    : Fxt::Point::PointCommon_( db, pointId, sizeof( StringStateful_T ) + stringLenInBytesWithoutNullTerminator, setterPoint )
{
}

///////////////////////////////////////////////////////////////////////////////
bool String::read( Cpl::Text::String& dstData ) const noexcept
{
    int   dstLen;
    char* dstPtr = dstData.getBuffer( dstLen );  // Note: 'dstLen' does NOT include the null terminator, i.e. the actual available storage is dstLen+1
    return readData( dstPtr, dstLen + 1 );
}

bool String::read( char* dstData, size_t dstSizeInBytes ) const noexcept
{
    return readData( dstData, dstSizeInBytes );
}

void String::write( const char* srcString, Fxt::Point::Api::LockRequest_T lockRequest ) noexcept
{
    // Note: The copyDataFrom_() method ensures that there are no buffer/data overruns
    writeData( srcString, strlen( srcString ), lockRequest );
}

void String::write( String& src, Fxt::Point::Api::LockRequest_T lockRequest ) noexcept
{
    // Note: The copyDataFrom_() method ensures that there are no buffer/data overruns
    updateFrom_( &(((StringStateful_T*) (src.m_state))->data),
                 strlen( ((StringStateful_T*) (src.m_state))->data ),
                 src.isNotValid(),
                 lockRequest );
}

size_t String::getDataSize_() noexcept
{
    return (m_stateSize - sizeof( StringStateful_T )) + 1;
}

size_t String::getMaxLength() const noexcept
{
    return m_stateSize - sizeof( StringStateful_T );
}

void* String::getDataPointer_() noexcept
{
    return ((StringStateful_T*) m_state)->data;
}

///////////////////////////////////////////////////////////////////////////////
bool String::toJSON_( JsonDocument& doc, bool verbose ) noexcept
{
    // Create value object
    JsonObject valObj = doc.createNestedObject( "val" );

    // Construct the 'val' key/value pair 
    valObj["maxLen"] = getMaxLength();
    valObj["text"]   = (char*) ((StringStateful_T*) m_state)->data;
    return true;
}

bool String::fromJSON_( JsonVariant & src, Fxt::Point::Api::LockRequest_T lockRequest, Cpl::Text::String * errorMsg ) noexcept
{
    // Note: Max size is ignored, i.e. do NOT support reallocating sizes via JSON

    // Get the string
    const char* newValue = src["text"];
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


void String::copyDataTo_( void* dstData, size_t dstSize ) const noexcept
{
    char* dstPtr = (char*) dstData;
    dstPtr[0]    = '\0';
    strncat( dstPtr, ((StringStateful_T*) m_state)->data, dstSize - 1 );
}

void String::copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
{
    char* dstPtr   = ((StringStateful_T*) m_state)->data;
    size_t dstLen  = getMaxLength();
    strncpy( dstPtr, (const char*) srcData, dstLen );
    dstPtr[dstLen] = '\0';
}
