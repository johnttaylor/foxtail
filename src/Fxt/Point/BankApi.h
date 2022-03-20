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
#include "Fxt/Point/DatabaseApi.h"
#include "Cpl/Memory/ContiguousAllocator.h"
#include <stdint.h>
#include <stdlib.h>

///
namespace Fxt {
///
namespace Point {


/** This abstract class defines the interface for a point Bank.  A Point bank
    is responsible for creating a collection of points and managing the memory
    for the allocated points.  A point Bank supports being able to block-up its
    point memory.  This allows for fast 'double buffering' of Points as well
    as HA transfers.
 */
class BankApi
{
public:
    /** This method takes a list of point Descriptors and 'populates' the bank.
        Populates means that Point instances are created for each Descriptors.
        
        The listOfDescriptors is variable length array of Descriptor pointers,
        where the end-of-list is a null pointer.

        The method should only be called once per 'setup'.

        The method returns true when successful; else false (e.g. out-of-memory)
        is returned.
     */
    virtual bool populate( Descriptor*                       listOfDescriptorPointers[], 
                           Cpl::Memory::ContiguousAllocator& allocatorForPoints, 
                           Fxt::Point::DatabaseApi&          dbForPoints ) noexcept = 0;


public:
    /** This method returns the amount of memory currently allocated by the Bank
     */
    virtual size_t getAllocatedSize() const noexcept = 0;

    /** This method returns a pointer to the start of the Bank's memory.
        USE WITH CAUTION!!
     */
    virtual const void* getStartOfMemory() const noexcept = 0;

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


    /** This method copies the data from 'src' into this instance.  If the
        size of the 'src' bank is not the same as this error then false is
        returned; else true is returned.
     */
    virtual bool copyFrom( BankApi& src ) noexcept = 0;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~BankApi() {}
};


};      // end namespaces
};
#endif  // end header latch
