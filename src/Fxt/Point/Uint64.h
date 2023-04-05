#ifndef Fxt_Point_Uint64_h_
#define Fxt_Point_Uint64_h_
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
    uint64_t.

    The toJSON()/fromJSON format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:"hex-value" }

        \endcode

 */
class Uint64 : public BasicInteger_<uint64_t>
{
public:
    /// Type ID for the point
    static constexpr const char* GUID_STRING = "20deff73-0f58-495e-a30e-166ee10df46f";

    /// Type name for the card
    static constexpr const char* TYPE_NAME   = "Fxt::Point::Uint64";

public:
    /** Constructor. Invalid Point.
     */
    Uint64( DatabaseApi&                        db, 
            uint32_t                            pointId, 
            Cpl::Memory::ContiguousAllocator&   allocatorForPointStatefulData,
            Api*                                setterPoint = nullptr )
        : BasicInteger_<uint64_t>( db, pointId, allocatorForPointStatefulData, setterPoint ) {}

public:
    /// Pull in overloaded methods from base class
    using BasicInteger_<uint64_t>::write;

    /// Updates the MP's data from 'src'. Note: The lock state of 'src' is NOT-USED/IGNORED
    void write( Uint64& src, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        updateFrom_( &(((Basic_<uint64_t>::Stateful_T*)(src.m_state))->data), sizeof( uint64_t ), src.isNotValid(), lockRequest );
    }

    ///  See Fxt::Point::Api
    void updateFromSetter() noexcept { if ( m_setter ) { write( *((Uint64*) m_setter) ); } }

    ///  See Fxt::Point::Api
    void updateFromSetter( const Api&    srcPt,
                           LockRequest_T lockRequest = eNO_REQUEST,
                           bool          beSafe      = true ) noexcept
    {
        if ( !beSafe || isSameType( srcPt ) ) { write( *((Uint64*) &srcPt), lockRequest ); }
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
