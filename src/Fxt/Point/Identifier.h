#ifndef Fxt_Point_Identifier_h_
#define Fxt_Point_Identifier_h_
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
#include <stdint.h>

///
namespace Fxt {
///
namespace Point {



/// Base class for Point Identifiers
class Identifier_T
{
public:
    /// Default constructor
    constexpr Identifier_T() :val( 0 ) {}

    /// Copy constructor
    constexpr Identifier_T( uint32_t x ) : val( x ) {}

public:

    /// Short hand for casting/returning my value as unsigned integer
    constexpr uint32_t operator+() const { return val; }

    /// Cast to uint32_t integer
    constexpr operator uint32_t() const { return val; }

protected:
    /// Actual value;
    const uint32_t val;
};


};      // end namespaces
};
#endif  // end header latch
