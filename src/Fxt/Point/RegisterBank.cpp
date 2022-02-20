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


#include "RegisterBank.h"

///
using namespace Fxt::Point;


///////////////////////////////////////////////////////////////////////////////

bool RegisterBank::copyTo( void* dst, size_t maxDstSizeInBytes ) noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
    return Bank::copyTo( dst, maxDstSizeInBytes );
}

bool RegisterBank::copyFrom( const void* src, size_t srcSizeInBytes ) noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
    return Bank::copyFrom( src, srcSizeInBytes );
}

