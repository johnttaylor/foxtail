#ifndef Cpl_Logging_LoggerMsg_h_
#define Cpl_Logging_LoggerMsg_h_
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

/** This enumeration defines Message identifiers for the Logger Category.  

        Note: Recommended that each message ID must be less than 32 characters
           123456789 123456789 12345678 12

    @param LoggerMsg          Enum

    @param QUEUE_OVERFLOW     The entry queue overflowed and at least one log entry was discarded
 */
BETTER_ENUM( LoggerMsg, uint16_t
             , QUEUE_OVERFLOW
);

};      // end namespaces
};
#endif  // end header latch
