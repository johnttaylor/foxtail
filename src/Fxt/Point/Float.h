#ifndef Fxt_Point_Float_h_
#define Fxt_Point_Float_h_
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
    float.

    The toJSON()/fromJSON format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:numericVal }

        \endcode

 */
class Float : public BasicReal_<float>
{
public:
    /// Type ID for the point
    static constexpr const char* GUID_STRING = "708745fa-cef6-4364-abad-063a40f35cbc";

    /// Type name for the card
    static constexpr const char* TYPE_NAME   = "Fxt::Point::Float";

public:
    /** Constructor. Invalid Point.
     */
    Float( DatabaseApi&                         db, 
           uint32_t                             pointId, 
           Cpl::Memory::ContiguousAllocator&    allocatorForPointStatefulData,
           Api*                                 setterPoint = nullptr ) 
        : BasicReal_<float>( db, pointId, allocatorForPointStatefulData, setterPoint ) {}

public:
    /// Pull in overloaded methods from base class
    using BasicReal_<float>::write;

    /// Updates the MP's data from 'src'. Note: The lock state of 'src' is NOT-USED/IGNORED
    void write( Float& src, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        updateFrom_( &(((Basic_<float>::Stateful_T*)(src.m_state))->data), sizeof( float ), src.isNotValid(), lockRequest );
    }

    ///  See Fxt::Point::Api
    void updateFromSetter() noexcept { if ( m_setter ) { write( *((Float*) m_setter) ); } }

public:
    ///  See Fxt::Point::Api
    const char* getTypeGuid() const noexcept { return GUID_STRING; }

    ///  See Fxt::Point::Api
    const char* getTypeName() const noexcept { return TYPE_NAME; }
};



};      // end namespaces
};
#endif  // end header latch
