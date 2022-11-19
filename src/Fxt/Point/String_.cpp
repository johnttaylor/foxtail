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
#include "Cpl/System/Trace.h"
#include <string.h>

#define SECT_   "Fxt::Point"

///
using namespace Fxt::Point;


///////////////////////////////////////////////////////////////////////////////
StringBase_::StringBase_( DatabaseApi& db, uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData, size_t sizeofData )
    : Fxt::Point::PointCommon_( db, pointId, pointName, sizeofData )
{
    m_state = allocatorForPointStatefulData.allocate( sizeofData );
    if ( m_state )
    {
        memset( m_state, 0, sizeofData );
        finishInit( false );
    }
    else
    {
        CPL_SYSTEM_TRACE_MSG( FXT_POINT_TRACE_SECT_, ("State Memory allocation failed for pointID: %lu", pointId) );
    }
}

/// Constructor. Valid Point.  Requires an initial value
StringBase_::StringBase_( DatabaseApi& db, uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData, size_t sizeofData, const char* initialValue )
    : Fxt::Point::PointCommon_( db, pointId, pointName, sizeofData )
{
    m_state = allocatorForPointStatefulData.allocate( sizeofData );
    if ( m_state )
    {
        ((BaseStateful_T*) m_state)->data[0] = '\0';
        strncat( ((BaseStateful_T*) m_state)->data, initialValue, sizeofData - 1 );
        finishInit( true );
    }
    else
    {
        CPL_SYSTEM_TRACE_MSG( FXT_POINT_TRACE_SECT_, ("State Memory allocation failed for pointID: %lu", pointId) );
    }
}

///////////////////////////////////////////////////////////////////////////////
bool StringBase_::read( Cpl::Text::String& dstData ) const noexcept
{
    int   dstLen;
    char* dstPtr = dstData.getBuffer( dstLen );  // Note: 'dstLen' does NOT include the null terminator, i.e. the actual available storage is dstLen+1
    return readData( dstPtr, dstLen + 1 );
}

bool StringBase_::read( char* dstData, size_t dstSizeInBytes ) const noexcept
{
    return readData( dstData, dstSizeInBytes );
}

void StringBase_::write( const char* srcString, Fxt::Point::Api::LockRequest_T lockRequest ) noexcept
{
    // Note: The copyDataFrom_() method ensures that there are no buffer/data overruns
    writeData( srcString, strlen( srcString ), lockRequest );
}

void StringBase_::write( StringBase_& src, Fxt::Point::Api::LockRequest_T lockRequest ) noexcept
{
    // Note: The copyDataFrom_() method ensures that there are no buffer/data overruns
    updateFrom_( &(((BaseStateful_T*) (src.m_state))->data),
                 strlen( ((BaseStateful_T*) (src.m_state))->data ),
                 src.isNotValid(),
                 lockRequest );
}

size_t StringBase_::getDataSize_() noexcept
{
    return getMaxLength() + 1;
}

void* StringBase_::getDataPointer_() noexcept
{
    return ((BaseStateful_T*) m_state)->data;
}

///////////////////////////////////////////////////////////////////////////////
bool StringBase_::toJSON_( JsonDocument& doc, bool verbose ) noexcept
{
    // Create value object
    JsonObject valObj = doc.createNestedObject( "val" );

    // Construct the 'val' key/value pair 
    valObj["maxLen"] = getMaxLength();
    valObj["text"]   = (char*) ((BaseStateful_T*) m_state)->data;
    return true;
}

bool StringBase_::fromJSON_( JsonVariant & src, Fxt::Point::Api::LockRequest_T lockRequest, Cpl::Text::String * errorMsg ) noexcept
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


void StringBase_::copyDataTo_( void* dstData, size_t dstSize ) const noexcept
{
    char* dstPtr = (char*) dstData;
    dstPtr[0]    = '\0';
    strncat( dstPtr, ((BaseStateful_T*) m_state)->data, dstSize - 1 );
}

void StringBase_::copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
{
    char* dstPtr   = ((BaseStateful_T*) m_state)->data;
    size_t dstLen  = getMaxLength();
    strncpy( dstPtr, (const char*) srcData, dstLen );
    dstPtr[dstLen] = '\0';
}
