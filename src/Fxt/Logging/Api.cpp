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

#include "Api.h"

 
void Fxt::Logging::logf( CriticalMsg msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    Cpl::Logging::vlogf<CategoryId, CriticalMsg>( CategoryId::CRITICAL, msgId, msgTextFormat, ap );
    va_end( ap );
}

void Fxt::Logging::logf( WarningMsg msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    Cpl::Logging::vlogf<CategoryId, WarningMsg>( CategoryId::WARNING, msgId, msgTextFormat, ap );
    va_end( ap );
}

void Fxt::Logging::logf( EventMsg msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    Cpl::Logging::vlogf<CategoryId, EventMsg>( CategoryId::EVENT, msgId, msgTextFormat, ap );
    va_end( ap );
}

void Fxt::Logging::logf( ErrCodeMsg msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    Cpl::Logging::vlogf<CategoryId, ErrCodeMsg>( CategoryId::ERRCODE, msgId, msgTextFormat, ap );
    va_end( ap );
}