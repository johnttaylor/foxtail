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

#include "PointCommon_.h"
#include 
///
using namespace Fxt::Point;

#define METAPTR     ((PointCommon_::Metadata_T*)(m_state))

////////////////////////
PointCommon_::PointCommon_( uint32_t pointId )
    : Cpl::Container::KeyUinteger32_T( pointId )
    , m_state( nullptr )
{
}

void PointCommon_::finishInit( bool isValid ) noexcept
{
    if ( METAPTR )
    {
        METAPTR->valid  = isValid;
        METAPTR->locked = false;
    }
}

/////////////////
const Cpl::Container::Key& PointCommon_::getKey() const noexcept
{
    return *this;
}

bool PointCommon_::isNotValid( void ) const noexcept
{
    return METAPTR == nullptr || METAPTR->valid;
}

void PointCommon_::setInvalid( LockRequest_T lockRequest ) noexcept
{

    if ( METAPTR && testAndUpdateLock( lockRequest ) )
    {
        METAPTR->valid = false;
    }
}

bool PointCommon_::read( void* dstData, size_t dstSize ) const noexcept
{
    // Return invalid if memory allocation failed
    if ( !METAPTR )
    {
        return false;
    }

    if ( dstData && METAPTR->valid )
    {
        copyDataTo_( dstData, dstSize );
    }
    return METAPTR->valid;
}

void PointCommon_::write( const void* srcData, size_t srcSize, LockRequest_T lockRequest ) noexcept
{
    // Note: testAndUpdateLock() always returns false if METAPTR is NULL
    if ( srcData && testAndUpdateLock( lockRequest ) )
    {
        copyDataFrom_( srcData, srcSize );
        METAPTR->valid = true;
    }
}


/////////////////
void PointCommon_::setLockState( LockRequest_T lockRequest ) noexcept
{
    // Do nothing if memory allocation failed
    if ( !METAPTR )
    {
        return;
    }

    if ( lockRequest == eLOCK )
    {
        METAPTR->locked = true;
    }
    else if ( lockRequest == eUNLOCK )
    {
        METAPTR->locked = false;
    }
}

bool PointCommon_::isLocked() const noexcept
{
    return METAPTR && METAPTR->locked;
}

bool PointCommon_::testAndUpdateLock( LockRequest_T lockRequest ) noexcept
{
    // Fail if memory allocation failed
    if ( !METAPTR )
    {
        return false;
    }

    bool result = false;
    if ( lockRequest == eUNLOCK )
    {
        METAPTR->locked = false;
        result = true;
    }
    else if ( lockRequest == eLOCK )
    {
        METAPTR->locked = true;
        result = true;
    }
    else
    {
        if ( METAPTR->locked == false )
        {
            result = true;
        }
    }

    return result;
}
