#ifndef Fxt_Point_Double_h_
#define Fxt_Point_Double_h_
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
    double.

    The toJSON()/fromJSON format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:numericVal }

        \endcode

 */
class Double : public BasicReal_<double>
{
public:
    /** Constructor. Invalid Point.
     */
    Double( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData ) : BasicReal_<double>(pointId, pointName, allocatorForPointStatefulData ) {}

    /// Constructor. Valid Point.  Requires an initial value
    Double( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData, double initialValue ) : BasicReal_<double>( pointId, pointName, allocatorForPointStatefulData, initialValue ) {}

public:
    /// Pull in overloaded methods from base class
    using BasicReal_<double>::write;

    /// Updates the MP's data from 'src'. Note: The lock state of 'src' is NOT-USED/IGNORED
    void write( Double& src, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        updateFrom( &(((Basic_<double>::Stateful_T*)(src.m_state))->data), sizeof( double ), src.isNotValid(), lockRequest );
    }


public:
    ///  See Fxt::Point::Api
    const char* getType() const noexcept { return "Fxt::Point::Double"; }

public:
    /// Creates a concrete instance in the invalid state
    static Api* create( Cpl::Memory::Allocator&             allocatorForPoints,
                        uint32_t                            pointId,
                        const char*                         pointName,
                        Cpl::Memory::ContiguousAllocator&   allocatorForPointStatefulData ) 
    { 
        return PointCommon_::create<Double>( allocatorForPoints, pointId, pointName, allocatorForPointStatefulData );
    }
};



};      // end namespaces
};
#endif  // end header latch
