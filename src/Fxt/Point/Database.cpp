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

#include "Database.h"

///
using namespace Fxt::Point;

       
//////////////////////////////////////////////
Database::Database( size_t maxNumPoints ) noexcept
    : Cpl::Point::Database( maxNumPoints )
    , m_nextPointId( 0 )
{
}

//////////////////////////////////////////////
const Cpl::Point::Identifier_T Database::getNextAvailablePointId() const noexcept
{
    return m_nextPointId;
}

bool Database::add( const Cpl::Point::Identifier_T numericId, Cpl::Point::Info_T& pointInfo ) noexcept
{
    bool result = Cpl::Point::Database::add( numericId, pointInfo );
    if ( result )
    {
        m_nextPointId++;
    }
    return result;
}
