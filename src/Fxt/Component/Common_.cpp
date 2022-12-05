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
#include "Error.h"


///
using namespace Fxt::Component;

#define INVALID_ELAPSED_TIME        0

//////////////////////////////////////////////////
Common_::Common_( )
    : m_lastExeCycleTimeUsec( INVALID_ELAPSED_TIME )
    , m_error( Fxt::Type::Error::SUCCESS() )
{
}

Common_::~Common_()
{
}

//////////////////////////////////////////////////
Fxt::Type::Error Common_::start( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Use an invalid time marker to indicate the not-started state
    if ( m_lastExeCycleTimeUsec == INVALID_ELAPSED_TIME && m_error == Fxt::Type::Error::SUCCESS() )
    {
        m_lastExeCycleTimeUsec = currentElapsedTimeUsec;
        return Fxt::Type::Error::SUCCESS();
    }
    m_error = fullErr( Err_T::FAILED_START );
    return m_error;
}

void Common_::stop() noexcept
{
    if ( m_lastExeCycleTimeUsec != INVALID_ELAPSED_TIME )
    {
        m_lastExeCycleTimeUsec = INVALID_ELAPSED_TIME;
        m_error                = Fxt::Type::Error::SUCCESS();
    }
}

bool Common_::isStarted() const noexcept
{
    return m_lastExeCycleTimeUsec != INVALID_ELAPSED_TIME;
}

Fxt::Type::Error Common_::getErrorCode() const noexcept
{
    return m_error;
}

/////////////////////////////////////////////
bool Common_::parsePointReferences( size_t              dstReferences[],
                                    unsigned            maxNumReferences,
                                    JsonArray&          arrayObj,
                                    Fxt::Type::Error    errTooMany,
                                    Fxt::Type::Error    errBadRef,
                                    unsigned&           numRefsFound )
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
    for ( unsigned i=0; i < numRefsFound; i++ )
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
    // Fail is component has been started
    if ( m_lastExeCycleTimeUsec != INVALID_ELAPSED_TIME )
    {
        return false;
    }

    for ( unsigned i=0; i < numElements; i++ )
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