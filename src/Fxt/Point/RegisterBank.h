#ifndef Fxt_Point_RegisterBank_h_
#define Fxt_Point_RegisterBank_h_
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


#include "Fxt/Point/Bank.h"
#include "Cpl/System/Mutex.h"

///
namespace Fxt {
///
namespace Point {


/** This concrete class extends the Bank implementation to have thread-safe
    copy operations
 */
class RegisterBank : public Bank
{
public:
    /// Constructor
    RegisterBank(){};

    /// Provides a thread-safe copy operation (See Fxt::Point::BankApi)
    bool copyTo( void* dst, size_t maxDstSizeInBytes ) noexcept;

    /// Provides a thread-safe copy operation (See Fxt::Point::BankApi)
    bool copyFrom( const void* src, size_t srcSizeInBytes ) noexcept;

protected:
    /// Mutex for critical sections
    Cpl::System::Mutex  m_lock;
};



};      // end namespaces
};
#endif  // end header latch
