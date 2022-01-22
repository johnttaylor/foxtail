#ifndef Fxt_Point_SetterApi_h_
#define Fxt_Point_SetterApi_h_
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

#include "Cpl/Point/Api.h"

///
namespace Fxt {
///
namespace Point {

/** This abstract class defines a partially type safe interface that is used
    to set the value (and valid/invalid state) of Point by using reference
    to a Generic point (instead of concrete, fully type safe reference).

    NOTE: Because the interface is not fully type-safe - the Application is
          RESPONSIBLE for ensuring that the concrete SetterApi instance has
          the same concrete type of the Point being updated/set
 */
class SetterApi
{
public:
    /** This method update the data/state of 'dstPoint'.  The value/state
        being written is dependent on the concrete instance.
     */
    virtual void setValue( Cpl::Point::Api* dstPoint ) noexcept = 0;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~SetterApi(){}
};


};      // end namespaces
};
#endif  // end header latch
