#ifndef Cpl_Logging_CategoryId_h_
#define Cpl_Logging_CategoryId_h_
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


#include "Cpl/Type/enum.h"

///
namespace Cpl {
///
namespace Logging {

/** This enumeration defines logging category identifiers.  A maximum of 32
    symbols can be defined

    Note: Recommended that each symbol must be less than 16 characters
           123456789 123456

    @param CategoryId         Enum

    @param CRITICAL           A subsystem or module encountered an unrecoverable error – but the overall system can still provide some level of functionality.
    @param SECURITY           A suspected security breach and/or suspicious behavior detected
    @param ALARM              A user visible alarm was raised or lowered
    @param WARNING            A recoverable error and/or an unexpected condition occurred
    @param EVENT              An ‘interesting’ event or action occurred
    @param INFO               Informational and/or debugging information
    @param METRIC             Metrics information
    @param LOGGER             Used by the Logger framework itself (e.g. logging an QUEUE_OVERFLOW conditions)
 */
BETTER_ENUM( CategoryId, uint32_t
             , CRITICAL    = 0x00000001
             , WARNING     = 0x00000002
             , EVENT       = 0x00000004
             , ALARM       = 0x00000008
             , SECURITY    = 0x00000010
             , INFO        = 0x00000020
             , METRICS     = 0x00000040
             , LOGGER      = 0x00000080
);

};      // end namespaces
};
#endif  // end header latch
