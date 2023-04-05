#ifndef Fxt_Point_Int16_h_
#define Fxt_Point_Int16_h_
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
    int16_t.

    The toJSON()/fromJSON format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:"hex-value" }

        \endcode

 */
class Int16 : public BasicInteger_<int16_t>
{
public:
    /// Type ID for the point
    static constexpr const char* GUID_STRING = "9b747497-eabf-4d11-8ce3-11ad9b048c43";

    /// Type name for the card
    static constexpr const char* TYPE_NAME   = "Fxt::Point::Int16";

public:
    /** Constructor. Invalid Point.
     */
    Int16( DatabaseApi&                         db, 
           uint32_t                             pointId, 
           Cpl::Memory::ContiguousAllocator&    allocatorForPointStatefulData,
           Api*                                 setterPoint = nullptr )
        : BasicInteger_<int16_t>( db, pointId, allocatorForPointStatefulData, setterPoint ) {}


public:
    /// Pull in overloaded methods from base class
    using BasicInteger_<int16_t>::write;

    /// Updates the MP's data from 'src'. Note: The lock state of 'src' is NOT-USED/IGNORED
    void write( Int16& src, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        updateFrom_( &(((Basic_<int16_t>::Stateful_T*)(src.m_state))->data), sizeof( int16_t ), src.isNotValid(), lockRequest );
    }

    ///  See Fxt::Point::Api
    void updateFromSetter() noexcept { if ( m_setter ) { write( *((Int16*) m_setter) ); } }

    ///  See Fxt::Point::Api
    void updateFromSetter( const Api&    srcPt,
                           LockRequest_T lockRequest = eNO_REQUEST,
                           bool          beSafe      = true ) noexcept
    {
        if ( !beSafe || isSameType( srcPt ) ) { write( *((Int16*) &srcPt), lockRequest ); }
    }


public:
    ///  See Fxt::Point::Api
    const char* getTypeGuid() const noexcept { return GUID_STRING; }

    ///  See Fxt::Point::Api
    const char* getTypeName() const noexcept { return TYPE_NAME; }
};



};      // end namespaces
};
#endif  // end header latch
