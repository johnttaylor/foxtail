#ifndef Fxt_Point_Uint16_h_
#define Fxt_Point_Uint16_h_
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
    uint16_t.

    The toJSON()/fromJSON format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:"hex-value" }

        \endcode

 */
class Uint16 : public BasicInteger_<uint16_t>
{
public:
    /// Type ID for the point
    static constexpr const char* GUID_STRING = "a1c42126-aaae-4e0f-a361-00d06f829759";

    /// Type name for the card
    static constexpr const char* TYPE_NAME   = "Fxt::Point::Uint16";

public:
    /** Constructor. Invalid Point.
     */
    Uint16( DatabaseApi& db, uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData ) 
        : BasicInteger_<uint16_t>(db, pointId, pointName, allocatorForPointStatefulData ) {}

    /// Constructor. Valid Point.  Requires an initial value
    Uint16( DatabaseApi& db, uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData, uint16_t initialValue ) 
        : BasicInteger_<uint16_t>( db, pointId, pointName, allocatorForPointStatefulData, initialValue ) {}

public:
    /// Pull in overloaded methods from base class
    using BasicInteger_<uint16_t>::write;

    /// Updates the MP's data from 'src'. Note: The lock state of 'src' is NOT-USED/IGNORED
    void write( Uint16& src, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept 
    {
        updateFrom_( &(((Basic_<uint16_t>::Stateful_T*)(src.m_state))->data), sizeof( uint16_t ), src.isNotValid(), lockRequest );
    }

public:
    ///  See Fxt::Point::Api
    const char* getTypeGuid() const noexcept { return GUID_STRING; }

    ///  See Fxt::Point::Api
    const char* getTypeName() const noexcept { return TYPE_NAME; }

public:
    /// Creates a concrete instance in the invalid state
    static Api* create( DatabaseApi&                        db, 
                        Cpl::Memory::Allocator&             allocatorForPoints,
                        uint32_t                            pointId,
                        const char*                         pointName,
                        Cpl::Memory::ContiguousAllocator&   allocatorForPointStatefulData ) 
    { 
        return PointCommon_::create<Uint16>( db, allocatorForPoints, pointId, pointName, allocatorForPointStatefulData );
    }
};



};      // end namespaces
};
#endif  // end header latch
