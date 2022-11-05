#ifndef Fxt_Point_Int64_h_
#define Fxt_Point_Int64_h_
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


#include "Fxt/Point/Basic_.h"
#include "Cpl/Memory/Allocator.h"

///
namespace Fxt {
///
namespace Point {


/** This class provides a concrete implementation for a Point who's data is a
    int64_t.

    The toJSON()/fromJSON format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:"hex-value" }

        \endcode

 */
class Int64 : public BasicInteger_<int64_t>
{
public:
    /** Constructor. Invalid Point.
     */
    Int64( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData ) : BasicInteger_<int64_t>( pointId, pointName, allocatorForPointStatefulData ) {}

    /// Constructor. Valid Point.  Requires an initial value
    Int64( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData, int64_t initialValue ) : BasicInteger_<int64_t>( pointId, pointName, allocatorForPointStatefulData, initialValue ) {}

public:
    /// Pull in overloaded methods from base class
    using BasicInteger_<int64_t>::write;

    /// Updates the MP's data from 'src'. Note: The lock state of 'src' is NOT-USED/IGNORED
    void write( Int64& src, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        updateFrom( &(((Basic_<int64_t>::Stateful_T*)(src.m_state))->data), sizeof( int64_t ), src.isNotValid(), lockRequest );
    }

public:
    ///  See Fxt::Point::Api
    const char* getType() const noexcept { return "Fxt::Point::Int64"; }

public:
    /// Creates a concrete instance in the invalid state
    static Api* create( Cpl::Memory::Allocator&             allocatorForPoints,
                        uint32_t                            pointId,
                        const char*                         pointName,
                        Cpl::Memory::ContiguousAllocator&   allocatorForPointStatefulData )
    {
        return PointCommon_::create<Int64>( allocatorForPoints, pointId, pointName, allocatorForPointStatefulData );
    }
};



};      // end namespaces
};
#endif  // end header latch
