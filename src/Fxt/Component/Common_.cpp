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


#include "Common_.h"


///
using namespace Fxt::Component;

#define INVALID_ELAPSED_TIME    0

//////////////////////////////////////////////////
Common_::Common_( uint16_t exeOrder )
    : m_lastExeCycleTimeUsec( INVALID_ELAPSED_TIME )
    , m_error( FXT_COMPONENT_ERR_NO_ERROR )
    , m_exeOrder( exeOrder )
{
}

Common_::~Common_()
{
}

//////////////////////////////////////////////////
bool Common_::start( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Use an invalid time marker to indicate the not-started state
    if ( m_lastExeCycleTimeUsec == INVALID_ELAPSED_TIME && m_error == FXT_COMPONENT_ERR_NO_ERROR )
    {
        m_lastExeCycleTimeUsec = currentElapsedTimeUsec;
        return true;
    }
    return false;
}

bool Common_::stop() noexcept
{
    if ( m_lastExeCycleTimeUsec != INVALID_ELAPSED_TIME )
    {
        m_lastExeCycleTimeUsec = INVALID_ELAPSED_TIME;
        m_error                = FXT_COMPONENT_ERR_NO_ERROR;
        return true;
    }
    return false;
}

bool Common_::isStarted() const noexcept
{
    return m_lastExeCycleTimeUsec != INVALID_ELAPSED_TIME;
}

uint16_t Common_::getExecutionOrder() const noexcept
{
    return m_exeOrder;
}

Api::Err_T Common_::getErrorCode() const noexcept
{
    return m_error;
}

/////////////////////////////////////////////
bool Common_::parsePointReferences( size_t      dstReferences[],
                                    unsigned    maxNumReferences,
                                    JsonArray&  arrayObj,
                                    Err_T       errTooMany,
                                    Err_T       errBadRef,
                                    unsigned&   numRefsFound )
{
    // Validate supported number of input signals
    numRefsFound = arrayObj.size();
    if ( numRefsFound > maxNumReferences )
    {
        m_error      = errTooMany;
        numRefsFound = 0;
        return false;
    }

    // Extract the Point references
    for ( int i=0; i < numRefsFound; i++ )
    {
        uint32_t pointRef = arrayObj[i]["idRef"] | Fxt::Point::Api::INVALID_ID;
        if ( pointRef == Fxt::Point::Api::INVALID_ID )
        {
            m_error = errBadRef;
            return false;
        }
        dstReferences[i] = pointRef;
    }

    return true;
}

bool Common_::resolveReferences( Fxt::Point::DatabaseApi& pointDb,
                                 Fxt::Point::Api*         srcIdsAndDstPointers[],
                                 unsigned                 numElements )
{
    for ( int i=0; i < numElements; i++ )
    {
        uint32_t pointId = (size_t) (srcIdsAndDstPointers[i]);
        Fxt::Point::Api* pointPtr = pointDb.lookupById( pointId );
        if ( pointPtr == nullptr )
        {
            return false;
        }
        srcIdsAndDstPointers[i] = pointPtr;
    }

    return true;
}