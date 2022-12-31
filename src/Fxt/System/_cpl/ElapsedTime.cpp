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

/* This file provides implementation of the Foxtail Elapsed time interface using
   ONLY milliseconds resolution, (i.e. it calls into the CPL Elapsed time
   interface
 */

#include "Fxt/System/ElapsedTime.h"
#include "Cpl/System/ElapsedTime.h"

///
using namespace Fxt::System;


uint64_t Fxt::System::ElapsedTime::now() noexcept
{
    return millisecondsToMicroseconds( Cpl::System::ElapsedTime::milliseconds() );
}

uint64_t Fxt::System::ElapsedTime::nowInRealTime() noexcept
{
    return millisecondsToMicroseconds( Cpl::System::ElapsedTime::millisecondsInRealTime() );
}

