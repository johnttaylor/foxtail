#ifndef Cpl_Point_Api_h_
#define Cpl_Point_Api_h_
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

#include "colony_config.h"
#include "Cpl/Text/String.h"
#include "Cpl/Json/Arduino.h"
#include <stdint.h>
#include <stdlib.h>

///
namespace Fxt {
///
namespace Point {


/** This abstract class defines the interface for a point Bank.  A Point bank
    is responsible for creating a collection point and managing the memory
    for the allocated points.  A point Bank supports being able to block its
    point memory.  This allows for fast 'double buffering' of Points as well
    as HA transfers.
 */
class BankApi
{
public:
    /// Virtual destructor to make the compiler happy
    virtual ~BankApi() {}
};


};      // end namespaces
};
#endif  // end header latch
