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
                     Cpl::Memory::ContiguousAllocator& allocatorForPoints,
                     Cpl::Point::DatabaseApi&          dbForPoints,
                     uint32_t&                         pointIdValue ) noexcept
{
    CPL_SYSTEM_ASSERT( listOfDescriptorPointers );

    m_memSize           = 0;
    m_memStart          = nullptr;
    Descriptor* itemPtr = *listOfDescriptorPointers;
    while ( itemPtr  )
    {
        // Create the next point
        if ( !itemPtr->createPoint( allocatorForPoints, pointIdValue ) )
        {
            // Error: allocator is out-of-space
            m_memSize  = 0;
            m_memStart = nullptr;
            return false;
        }

        // Add the point to the database
        if ( !dbForPoints.add( itemPtr->getPointId(), itemPtr->getPointInfo() ) )
        {
            // Error: database is out-of-space
            m_memSize  = 0;
            m_memStart = nullptr;
            return false;
        }

        // Trap the 1st allocate address
        if ( m_memStart == nullptr )
        {
            m_memStart = itemPtr->getPoint();
        }
        
        // Keep track of the allocated size
        m_memSize += itemPtr->getPoint()->getTotalSize();

        // Get the next descriptor in the list
        pointIdValue++;
        listOfDescriptorPointers++;
        itemPtr = *listOfDescriptorPointers;
    }

    // If I get here the bank was successfully populated
    return true;
}

size_t Bank::getAllocatedSize() const noexcept
{
    return m_memSize;
}

const void* Bank::getStartOfMemory() const noexcept
{
    return m_memStart;
}

bool Bank::copyTo( void* dst, size_t maxDstSizeInBytes ) noexcept
{
    // Fail if the destination is too small
    if ( maxDstSizeInBytes < m_memSize )
    {
        return false;
    }

    memcpy( dst, m_memStart, m_memSize );
    return true;
}

bool Bank::copyFrom( const void* src, size_t srcSizeInBytes ) noexcept
{
    // Fail if the source is too large
    if ( srcSizeInBytes > m_memSize )
    {
        return false;
    }

    memcpy( m_memStart, src, m_memSize );
    return true;
}

bool Bank::copyFrom( BankApi& src ) noexcept
{
    size_t srcSizeInBytes = src.getAllocatedSize();
    if ( srcSizeInBytes != m_memSize )
    {
        return false;
    }
    memcpy( m_memStart, src.getStartOfMemory(), m_memSize );
    return true;
}
