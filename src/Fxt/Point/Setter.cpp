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

#include "Setter.h"
#include "PointCommon_.h"
#include "Cpl/Text/FString.h"
#include "Cpl/System/Trace.h"

#define SECT_ "Fxt::Point"

///
using namespace Fxt::Point;

////////////////////////////////////////////////
Setter::Setter( Fxt::Point::Api* srcPoint )
    :m_srcPoint( srcPoint )
{
}

Setter::~Setter()
{
    // Call the destructor on the internal point.  Note: The application's frees the physical memory
    if ( m_srcPoint )
    {
        m_srcPoint->~Api();
    }
}

size_t Setter::getInternalPointId() const noexcept
{
    return m_srcPoint == nullptr ? Fxt::Point::Api::INVALID_ID : m_srcPoint->getId();
}

void Setter::setValue( Fxt::Point::Api& dstPoint ) noexcept
{
    // Do nothing if source point was never supplied
    if ( m_srcPoint )
    {
        dstPoint.updateFrom_( m_srcPoint->getDataPointer_(), m_srcPoint->getDataSize_(), m_srcPoint->isNotValid() );
    }
    else
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("setValue() called on %lu, but no 'srcPoint' was supplied", dstPoint.getId()) );
    }
}

Setter* Setter::create( DatabaseApi&                         db,
                        CreateFunc_T                         pointFactoryMethod,
                        uint32_t                             pointId,
                        const char*                          pointName,
                        Cpl::Memory::Allocator&              allocatorForPoints,
                        Cpl::Memory::ContiguousAllocator&    allocatorForPointStatefulData,
                        const JsonObject&                    valueSrc ) noexcept
{
    // Create the internal point
    Api* srcPoint = (pointFactoryMethod) (db, allocatorForPoints, pointId, pointName, allocatorForPointStatefulData);
    if ( srcPoint == nullptr )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Failed to create internal Setter point (id=%lu)", pointId) );
        return nullptr;
    }

    // Parse the Valid/Invalid state
    JsonVariant valid = valueSrc["valid"];
    if ( valid.isNull() == false && valid.as<bool>() == false )
    {
        // Setter/initial value is the 'invalid-state'
        srcPoint->setInvalid( Api::eLOCK );
    }

    // Permanently set the source point's value
    else
    {
        Cpl::Text::FString<128> errMsg;
        JsonVariant val = valueSrc["val"];
        if ( !srcPoint->fromJSON_( val, Api::eLOCK, &errMsg ) )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Failed to parse 'val' object for the Setter point.id=%lu err=%s", pointId, errMsg.getString()) );
            pointFactoryMethod
            return nullptr;
        }
    }

    // Allocate memory for the Setter
    void* memPtr = allocatorForPoints.allocate( sizeof( Setter ) );
    if ( memPtr == nullptr )
    {
        return nullptr;
    }

    // Create the Setter instance
    return new(memPtr) Setter( srcPoint );
}
