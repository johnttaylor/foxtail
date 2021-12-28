#ifndef Fxt_Point_Info_h_
#define Fxt_Point_Info_h_
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

#include "Fxt/Point/Api.h"

///
namespace Fxt {
///
namespace Point {


/** This structure defines the non-point data that is associated with every
    Point.
 */
typedef struct
{
    Api*        pointInstance;  //!< Pointer to actual Point instance
    const char* symbolicName;   //!< Text name for the Point
} Info_T;



};      // end namespaces
};
#endif  // end header latch
