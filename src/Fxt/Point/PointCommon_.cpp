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
#include "Cpl/System/Trace.h"

#define SECT_   "Fxt::Point"

///
using namespace Fxt::Point;

#define METAPTR     ((PointCommon_::Metadata_T*)(m_state))

constexpr uint32_t Fxt::Point::Api::INVALID_ID;

////////////////////////
PointCommon_::PointCommon_( DatabaseApi&                        db,
                            uint32_t                            pointId,
                            size_t                              stateSize,
                            Cpl::Memory::ContiguousAllocator&   allocatorForPointStatefulData,
                            Api*                                setterPoint )
    : m_id( pointId )
    , m_state( allocatorForPointStatefulData.allocate( stateSize ) )
    , m_stateSize( allocatorForPointStatefulData.allocatedSizeForNBytes( stateSize ) )
    , m_setter( setterPoint )
{
    if ( m_state )
    {
        memset( m_state, 0, stateSize );
    }
    else
    {
        m_setter = nullptr;
        m_id     = INVALID_ID;
        CPL_SYSTEM_TRACE_MSG( FXT_POINT_TRACE_SECT_, ("State Memory allocation failed for pointID: %lu", pointId) );
    }

    // Auto register with the database
    if ( !db.add( *this ) )
    {
        // DB Error -->put the point in a non-functioning/error state
        m_state  = nullptr;
        m_setter = nullptr;
        m_id     = INVALID_ID;
        CPL_SYSTEM_TRACE_MSG( FXT_POINT_TRACE_SECT_, ("Failed DB add (duplicate ID?) pointID: %lu", pointId) );
    }
}

/////////////////
void* PointCommon_::getStartOfStatefulMemory_() const noexcept
{
    return m_state;
}

size_t PointCommon_::getStatefulMemorySize() const noexcept
{
    return m_stateSize;
}

uint32_t PointCommon_::getId() const noexcept
{
    return m_id;
}


void PointCommon_::getMetadata( bool& isValid, bool& isLocked ) const noexcept
{
    if ( METAPTR )
    {
        isValid  = METAPTR->valid;
        isLocked = METAPTR->locked;
    }
    else
    {
        isValid  = false;
        isLocked = false;
    }
}

bool PointCommon_::isNotValid( void ) const noexcept
{
    return METAPTR == nullptr || METAPTR->valid == false;
}

void PointCommon_::setInvalid( LockRequest_T lockRequest ) noexcept
{

    if ( METAPTR && testAndUpdateLock( lockRequest ) )
    {
        METAPTR->valid = false;
        if ( m_state )
        {
            memset( m_state, 0, m_stateSize );
        }
    }
}

bool PointCommon_::hasSetter() const noexcept
{
    return m_setter != nullptr;
}

bool PointCommon_::readData( void* dstData, size_t dstSize ) const noexcept
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

void PointCommon_::writeData( const void* srcData, size_t srcSize, LockRequest_T lockRequest ) noexcept
{
    // Skip if the memory allocation failed
    if ( METAPTR )
    {
        if ( srcData && testAndUpdateLock( lockRequest ) )
        {
            copyDataFrom_( srcData, srcSize );
            METAPTR->valid = true;
        }
    }
}

void PointCommon_::updateFrom_( const void*                    srcData,
                                size_t                         srcSize,
                                bool                           srcNotValid,
                                Fxt::Point::Api::LockRequest_T lockRequest ) noexcept
{
    if ( srcNotValid )
    {
        setInvalid( lockRequest );
    }
    else
    {
        writeData( srcData, srcSize, lockRequest );
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
        result          = true;
    }
    else if ( lockRequest == eLOCK )
    {
        METAPTR->locked = true;
        result          = true;
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

/////////////////
bool Api::validatePointTypes( Fxt::Point::Api* arrayOfPoints[], uint16_t numPoints, const char* expectedGUID )
{
    for ( uint16_t i=0; i < numPoints; i++ )
    {
        if ( arrayOfPoints[i] != nullptr )
        {
            if ( strcmp( arrayOfPoints[i]->getTypeGuid(), expectedGUID ) != 0 )
            {
                return false;
            }
        }
    }

    return true;
}
