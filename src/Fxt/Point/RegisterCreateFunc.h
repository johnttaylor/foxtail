#ifndef Fxt_Point_CreateFunc_h_
#define Fxt_Point_CreateFunc_h_
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


#include "Fxt/Point/Descriptor.h"
#include "Fxt/Point/CreateFuncDatabaseApi.h"

///
namespace Fxt {
///
namespace Point {


/** This concrete template class implements is used to self-register a Point 
    Create function with the Create Function Data base.  For a given Point type, 
    there should only be one instance of this class created

    Template Args:
        MPTYPE   - The model point type

  */
template <class MPTYPE>
class RegisterCreateFunc
{
public:
    /// Constructor.  
    RegisterCreateFunc( CreateFuncDatabaseApi&   createDb ) noexcept
    {
        createDb.insert_( MPTYPE::create, MPTYPE::GUID_STRING );
    }

};



};      // end namespaces
};
#endif  // end header latch
