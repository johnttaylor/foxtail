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


#include "Bank.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/Trace.h"

#define SECT_ "Fxt::Point::Bank"

///
using namespace Fxt::Point;

///////////////////////////////////////////////////////////////////////////////
Bank::Bank()
    : m_memStart( nullptr )
    , m_memSize(0 )
{
}



///////////////////////////////////////////////////////////////////////////////
bool Bank::populate( Descriptor*                       listOfDescriptorPointers[],
                     Cpl::Memory::ContiguousAllocator& generalAllocator,
                     Fxt::Point::DatabaseApi&          dbForPoints,
                     Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData ) noexcept
{
    CPL_SYSTEM_ASSERT( listOfDescriptorPointers );

    m_memSize           = 0;
    m_memStart          = nullptr;
    Descriptor* itemPtr = *listOfDescriptorPointers;
    while ( itemPtr  )
    {
        // Fail if the point already exists
        if ( dbForPoints.lookupById( itemPtr->getPointId() ) != nullptr )
        {
            // Error: Duplicate POINT ID
            m_memSize  = 0;
            m_memStart = nullptr;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Point ALREADY exists in the database. pointID: %lu", itemPtr->getPointId()) );
            return false;
        }

        // Create the next point
        Api* point = itemPtr->createPoint( dbForPoints, generalAllocator, allocatorForPointStatefulData );
        if ( point == nullptr )
        {
            // Error: allocator is out-of-space
            m_memSize  = 0;
            m_memStart = nullptr;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Point Memory allocation failed for pointID: %lu", itemPtr->getPointId() ) );
            return false;
        }

        // Validate that the point was added to the database
        if ( dbForPoints.lookupById( itemPtr->getPointId() ) == nullptr )
        {
            // Error: database out-of-space
            m_memSize  = 0;
            m_memStart = nullptr;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Point was NOT added to the DB (i.e. DB out-of-space). pointID: %lu", itemPtr->getPointId()) );
            return false;
        }

        // Trap the 1st allocate address
        if ( m_memStart == nullptr )
        {
            m_memStart = point->getStartOfStatefulMemory_();
        }
        
        // Keep track of the allocated size
        m_memSize += point->getStatefulMemorySize();

        // Get the next descriptor in the list
        listOfDescriptorPointers++;
        itemPtr = *listOfDescriptorPointers;
    }

    // If I get here the bank was successfully populated
    return true;
}

size_t Bank::getStatefulAllocatedSize() const noexcept
{
    return m_memSize;
}

const void* Bank::getStartOfStatefulMemory() const noexcept
{
    return m_memStart;
}

bool Bank::copyStatefulMemoryTo( void* dst, size_t maxDstSizeInBytes ) noexcept
{
    // Fail if the destination is too small
    if ( maxDstSizeInBytes < m_memSize )
    {
        return false;
    }

    memcpy( dst, m_memStart, m_memSize );
    return true;
}

bool Bank::copyStatefulMemoryFrom( const void* src, size_t srcSizeInBytes ) noexcept
{
    // Fail if the source is too large
    if ( srcSizeInBytes > m_memSize )
    {
        return false;
    }

    memcpy( m_memStart, src, m_memSize );
    return true;
}

bool Bank::copyStatefulMemoryFrom( BankApi& src ) noexcept
{
    size_t srcSizeInBytes = src.getStatefulAllocatedSize();
    if ( srcSizeInBytes != m_memSize )
    {
        return false;
    }
    memcpy( m_memStart, src.getStartOfStatefulMemory(), m_memSize );
    return true;
}
