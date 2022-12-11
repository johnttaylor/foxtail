#ifndef Fxt_Point_Uint32_h_
#define Fxt_Point_Uint32_h_
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
    uint32_t.

    The toJSON()/fromJSON format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:"hex-value" }

        \endcode

 */
class Uint32 : public BasicInteger_<uint32_t>
{
public:
    /// Type ID for the point
    static constexpr const char* GUID_STRING = "99f91433-b8e4-4480-9203-8fcab5d9f20f";

    /// Type name for the card
    static constexpr const char* TYPE_NAME   = "Fxt::Point::Uint32";

public:
    /** Constructor. Invalid Point.
     */
    Uint32( DatabaseApi&                        db, 
            uint32_t                            pointId, 
            Cpl::Memory::ContiguousAllocator&   allocatorForPointStatefulData,
            Api*                                setterPoint = nullptr )
        : BasicInteger_<uint32_t>( db, pointId, allocatorForPointStatefulData, setterPoint ) {}

public:
    /// Pull in overloaded methods from base class
    using BasicInteger_<uint32_t>::write;

    /// Updates the MP's data from 'src'. Note: The lock state of 'src' is NOT-USED/IGNORED
    void write( Uint32& src, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        updateFrom_( &(((Basic_<uint32_t>::Stateful_T*)(src.m_state))->data), sizeof( uint32_t ), src.isNotValid(), lockRequest );
    }

    ///  See Fxt::Point::Api
    void updateFromSetter() noexcept { if ( m_setter ) { write( *((Uint32*) m_setter) ); } }

public:
    ///  See Fxt::Point::Api
    const char* getTypeGuid() const noexcept { return GUID_STRING; }

    ///  See Fxt::Point::Api
    const char* getTypeName() const noexcept { return TYPE_NAME; }
};



};      // end namespaces
};
#endif  // end header latch
