#ifndef Fxt_Point_BankApi_h_
#define Fxt_Point_BankApi_h_
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
#include "Cpl/Point/DatabaseApi.h"
#include "Cpl/Memory/ContiguousAllocator.h"
#include <stdint.h>
#include <stdlib.h>

///
namespace Fxt {
///
namespace Point {


/** This abstract class defines the interface for a point Bank.  A Point bank
    is responsible for creating a collection of points and managing the memory
    for the allocated points.  A point Bank supports being able to block its
    point memory.  This allows for fast 'double buffering' of Points as well
    as HA transfers.
 */
class BankApi
{
public:
    /** This method takes a list of point Descriptors and 'populates' the bank.
        Populates means that Point instances are created for each Descriptors.
        
        The listOfDescriptors is variable length array of descriptor instances,
        where the end-of-list is an instance of the NullDescrptor.

        The 'pointIdValue' is an INPUT to the function AND it is an 
        OUTPUT.  For input it is the STARTING point ID to use when creating
        points.  For output i returns the 'last/final' PointId assigned by 
        the Bank.  The Bank auto-increments the pointIdValue every time a 
        point is created.

        The method should only be called once per 'setup'.

        The method returns true when successful; else false (e.g. out-of-memory)
        is returned.
     */
    virtual bool populate( Descriptor*                       listOfDescriptors, 
                           Cpl::Memory::ContiguousAllocator& allocatorForPoints, 
                           Cpl::Point::DatabaseApi&          dbForPoints, 
                           uint32_t&                         pointIdValue ) noexcept = 0;


public:
    /** This method copies the Bank's 'point memory' to the specified destination.
        The method returns if the copy operation is successful; else false (e.g.
        'dst' size is less than the Bank's point memory) is returned.
     */
    virtual bool copyTo( void* dst, size_t maxDstSizeInBytes ) noexcept = 0;

    /** This method copies specified source data to the Bank's 'point memory' 
        The method returns if the copy operation is successful; else false (e.g.
        'src' size is greater than the Bank's point memory) is returned.
     */
    virtual bool copyFrom( const void* src, size_t srcSizeInBytes ) noexcept = 0;


public:
    /// Virtual destructor to make the compiler happy
    virtual ~BankApi() {}
};


};      // end namespaces
};
#endif  // end header latch
