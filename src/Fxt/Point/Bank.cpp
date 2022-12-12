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
    , m_memSize( 0 )
    , m_error( false )
{
}



///////////////////////////////////////////////////////////////////////////////
bool Bank::createPoint( FactoryDatabaseApi&                pointFactoryDb,
                        JsonObject&                        pointObject,
                        Fxt::Type::Error&                  pointErrorCode,
                        Cpl::Memory::ContiguousAllocator&  generalAllocator,
                        Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                        Fxt::Point::DatabaseApi&           dbForPoints,
                        const char*                        pointIdKeyName ) noexcept
{
    // Fail - if I have previously failed
    if ( m_error )
    {
        pointErrorCode = fullErr( Err_T::BANK_CONT_ERROR );
        return false;
    }

    // Create the point
    Api* point = pointFactoryDb.createPointfromJSON( pointObject,
                                                     pointErrorCode,
                                                     generalAllocator,
                                                     statefulDataAllocator,
                                                     dbForPoints,
                                                     pointIdKeyName );
    if ( point == nullptr )
    {
        // Error: Failed to create the point
        m_memSize  = 0;
        m_memStart = nullptr;
        m_error    = true;
        return false;
    }

    // Trap the 1st allocated address
    if ( m_memStart == nullptr )
    {
        m_memStart = point->getStartOfStatefulMemory_();
    }

    // Keep track of the allocated size (account for the 'Setter' when it is present)
    m_memSize += point->getStatefulMemorySize();
    if ( point->hasSetter() )
    {
        m_memSize += point->getStatefulMemorySize();
    }

    // If I get here the Point was successfully added to the bank
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
