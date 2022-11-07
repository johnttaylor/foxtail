#ifndef Fxt_Point_Source_h_
#define Fxt_Point_Source_h_
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

#include "Fxt/Point/Setter.h"

///
namespace Fxt {
///
namespace Point {


/** This concrete template class implements the Setter.  The 'source' value for
    the setValue() method is separate point instance.

    Template args: class "P" is the type of the 'source point'
 */
template <class P>
class Source : public Setter
{
public:
    /// Constructor
    Source( P& src ) :m_src( &src ){}
    
public:
    /// See Fxt::Point::Setter
    void setValue( Fxt::Point::Api* dstPt ) noexcept
    {
        ((P*) dstPt)->write( m_src );
    }

protected:
    /// Handle to the point that is the 'source' of the set operation
    P& m_src;
};


};      // end namespaces
};
#endif  // end header latch
