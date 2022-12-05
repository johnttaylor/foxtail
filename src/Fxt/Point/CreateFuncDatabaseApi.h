#ifndef Fxt_Point_CreateFuncDatabaseApi_h_
#define Fxt_Point_CreateFuncDatabaseApi_h_
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


///
namespace Fxt {
///
namespace Point {


/** This abstract class defines the interface for the Point Create Function Database.  
    The database contains the list of Create functions instances for all support 
    Point types.

    Note: There is no guaranteed order Create Function list.

    The semantics of the Database interface is NOT thread safe.
 */
class CreateFuncDatabaseApi
{
public:
    /** This method looks-up the Point Create Function by the Point' Type GUID.  
        If Type GUID cannot be found (i.e. point type not supported by
        the platform), THEN the method returns 0.

        The 'guidPointTypeId' is null terminated string contain a 8-4-4-4-12 
        formated GUID.
     */
    virtual Descriptor::CreateFunc_T lookup( const char* guidPointTypeId ) noexcept = 0;

public:
    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Fxt::Card namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        NOTE: There is NO checks/protection against adding two factories with
              the same card GUID

        This method inserts a IO Card Factory instance into the Database.
     */
    virtual void insert_( Descriptor::CreateFunc_T createFunctionToAdd, const char* guidPointTypeId ) noexcept = 0;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~CreateFuncDatabaseApi() {}
};


};      // end namespaces
};
#endif  // end header latch
