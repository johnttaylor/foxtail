#ifndef Fxt_Logging_ErrCodeMsg_h_
#define Fxt_Logging_ErrCodeMsg_h_
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
namespace Fxt {
///
namespace Logging {

/** This enumeration defines Logging message code for the ERRCODE category

        Note: Each error code symbol must be less than 32 characters
           123456789 123456789 12345678 12

    @param ErrCodeMsg                       Enum

    @param ERRVALUE                         Logs the error code and text string for an error
 */
BETTER_ENUM( ErrCodeMsg, uint16_t
             , ERRVALUE
);


};      // end namespaces
};
#endif  // end header latch
