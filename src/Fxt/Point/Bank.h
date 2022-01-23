#ifndef Fxt_Point_Bamk_h_
#define Fxt_Point_Bamk_h_
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
    /// Set Fxt::Point::BankApi
    bool populate( Descriptor*                       listOfDescriptors,
                   Cpl::Memory::ContiguousAllocator& allocatorForPoints,
                   Cpl::Point::DatabaseApi&          dbForPoints,
                   uint32_t&                         pointIdValue ) noexcept;

    /// Set Fxt::Point::BankApi
    bool copyTo( void* dst, size_t maxDstSizeInBytes ) noexcept;

    /// Set Fxt::Point::BankApi
    bool copyFrom( const void* src, size_t srcSizeInBytes ) noexcept;

protected:
    /// Start of allocated memory
    void*    m_memStart;

    /// Size of allocated memory
    size_t   m_memSize;
};



};      // end namespaces
};
#endif  // end header latch
