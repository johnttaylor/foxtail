#ifndef Fxt_Logging_CategoryId_h_
#define Fxt_Logging_CategoryId_h_
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


#include "Fxt/Type/Error.h"




///
namespace Fxt {
///
namespace Logging {

/** This enumeration defines the various logging categories.

    Note: Each symbol must be less than 16 characters
           123456789 123456

    @param CategoryId           Enum

    @param CRITICAL             A sub-system/module encountered an unrecoverable error – but the overall System can still provide some level of functionality.
    @param WARNING              A recoverable error and/or an unexpected condition occurred
    @param EVENT                An ‘interesting’ event or action occurred
    @param ERRCODE              A API/function/sub-system encountered a error
 */
BETTER_ENUM( CategoryId, uint32_t
             , CRITICAL    = 0x0001
             , WARNING     = 0x0002
             , EVENT       = 0x0004
             , ERRCODE     = 0x0008
);



};      // end namespaces
};
#endif  // end header latch
