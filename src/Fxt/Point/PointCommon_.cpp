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

///
using namespace Fxt::Point;


////////////////////////
PointCommon_::PointCommon_( bool isValid )
    : m_locked( false )
    , m_valid( isValid )
{
}

/////////////////
bool PointCommon_::isNotValid( void ) const noexcept
{
    return !m_valid;
}

void PointCommon_::setInvalid( LockRequest_T lockRequest ) noexcept
{
    if ( testAndUpdateLock( lockRequest ) )
    {
        m_valid = false;
    }
}

bool PointCommon_::read( void* dstData, size_t dstSize ) const noexcept
{
    if ( dstData && m_valid )
    {
        copyDataTo_( dstData, dstSize );
    }
    return m_valid;
}

void PointCommon_::write( const void* srcData, size_t srcSize, LockRequest_T lockRequest ) noexcept
{
    if ( srcData && testAndUpdateLock( lockRequest ) )
    {
        copyDataFrom_( srcData, srcSize );
        m_valid = true;
    }
}


/////////////////
void PointCommon_::setLockState( LockRequest_T lockRequest ) noexcept
{
    if ( lockRequest == eLOCK )
    {
        m_locked = true;
    }
    else if ( lockRequest == eUNLOCK )
    {
        m_locked = false;
    }
}

bool PointCommon_::isLocked() const noexcept
{
    return m_locked;
}

bool PointCommon_::testAndUpdateLock( LockRequest_T lockRequest ) noexcept
{
    bool result = false;
    if ( lockRequest == eUNLOCK )
    {
        m_locked = false;
        result = true;
    }
    else if ( lockRequest == eLOCK )
    {
        m_locked = true;
        result = true;
    }
    else
    {
        if ( m_locked == false )
        {
            result = true;
        }
    }

    return result;
}
