#ifndef Fxt_Point_Bank_h_
#define Fxt_Point_Bank_h_
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


#include "Fxt/Point/BankApi.h"

///
namespace Fxt {
///
namespace Point {


/** This concrete class implements the BankApi

    NOTE: This class is NOT thread safe
 */
class Bank : public BankApi
{
public:
    /// Constructor
    Bank();

public:
    /// See Fxt::Point::BankApi
    bool populate( Descriptor*                       listOfDescriptorPointers[],
                   Cpl::Memory::ContiguousAllocator& allocatorForPoints,
                   Fxt::Point::DatabaseApi&          dbForPoints,
                   Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData ) noexcept;

    /// See Fxt::Point::BankApi
    bool copyStatefulMemoryTo( void* dst, size_t maxDstSizeInBytes ) noexcept;

    /// See Fxt::Point::BankApi
    bool copyStatefulMemoryFrom( const void* src, size_t srcSizeInBytes ) noexcept;

    /// See Fxt::Point::BankApi
    bool copyStatefulMemoryFrom( BankApi& src ) noexcept;

    /// See Fxt::Point::BankApi
    size_t getStatefulAllocatedSize() const noexcept;

    /// See Fxt::Point::BankApi
    const void* getStartOfStatefulMemory() const noexcept;

protected:
    /// Start of allocated memory
    void*    m_memStart;

    /// Size of allocated memory
    size_t   m_memSize;
};



};      // end namespaces
};
#endif  // end header latch
