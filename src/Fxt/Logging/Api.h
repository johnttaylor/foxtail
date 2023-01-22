#ifndef Fxt_Logging_Api_h_
#define Fxt_Logging_Api_h_
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

#include "Cpl/Logging/Api.h"
#include "Fxt/Logging/CategoryId.h"
#include "Fxt/Logging/CriticalMsg.h"
#include "Fxt/Logging/ErrCodeMsg.h"
#include "Fxt/Logging/EventMsg.h"
#include "Fxt/Logging/WarningMsg.h"


///
namespace Fxt {
///
namespace Logging {


/*---------------------------------------------------------------------------*/
/// This method logs a CRITICAL log entry
void logf( CriticalMsg msgId, const char* msgTextFormat, ... ) noexcept;

/// This method logs a WARNING log entry
void logf( WarningMsg msgId, const char* msgTextFormat, ... ) noexcept;

/// This method logs a EVENT log entry
void logf( EventMsg msgId, const char* msgTextFormat, ... ) noexcept;

/// This method logs a ERRCODE log entry
void logf( ErrCodeMsg msgId, const char* msgTextFormat, ... ) noexcept;

};      // end namespaces
};
#endif  // end header latch
