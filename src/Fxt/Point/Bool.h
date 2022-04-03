#ifndef Fxt_Point_Bool_h_
#define Fxt_Point_Bool_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet atH
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
    bool.

    The toJSON()/fromJSON format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:true|false }

        \endcode

 */
class Bool : public Basic_<bool>
{
public:
    /// Simplify access the stateful data
    typedef typename Basic_<bool>::Stateful_T StateBlock_T;

public:
    /** Constructor. Invalid Point.
     */
    Bool( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData ) : Basic_<bool>(pointId, pointName, allocatorForPointStatefulData ) {}

    /// Constructor. Valid Point.  Requires an initial value
    Bool( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData, bool initialValue ) : Basic_<bool>( pointId, pointName, allocatorForPointStatefulData, initialValue ) {}

public:
    /// Type safe write. See Fxt::Point::Api
    virtual void write( bool newValue, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        Fxt::Point::PointCommon_::write( &newValue, sizeof( bool ), lockRequest );
    }

    /// Short-cut for writing true
    inline void set( Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept { write( true, lockRequest ); }

    /// Short-cut for writing false
    inline void clear( Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept { write( false, lockRequest ); }

    /// Updates the MP's data from 'src'. Note: The lock state of 'src' is NOT-USED/IGNORED
    virtual void write( Bool& src, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept 
    {
        updateFrom( &(((StateBlock_T*)(src.m_state))->data), sizeof( bool ), src.isNotValid(), lockRequest );
    }

public:
    ///  See Fxt::Point::Api
    const char* getType() const noexcept { return "Fxt::Point::Bool"; }

public:
    /// Creates a concrete instance in the invalid state
    static Api* create( Cpl::Memory::Allocator&             allocatorForPoints,
                        uint32_t                            pointId,
                        const char*                         pointName,
                        Cpl::Memory::ContiguousAllocator&   allocatorForPointStatefulData ) 
    { 
        return PointCommon_::create<Bool>( allocatorForPoints, pointId, pointName, allocatorForPointStatefulData );
    }

public:
    bool toJSON_( JsonDocument& doc, bool verbose ) noexcept
    {
        // Construct the 'val' key/value pair (as a HEX string)
        doc["val"] = ((StateBlock_T*) PointCommon_::m_state)->data;
        return true;
    }

    bool fromJSON_( JsonVariant& src, Fxt::Point::Api::LockRequest_T lockRequest, Cpl::Text::String* errorMsg ) noexcept
    {
        // Attempt to parse the value key/value pair
        if ( src.is<bool>() == false )
        {
            if ( errorMsg )
            {
                *errorMsg = "Invalid syntax for the 'val' key/value pair";
            }
            return false;
        }

        write( src.as<bool>(), lockRequest );
        return true;
    }
};



};      // end namespaces
};
#endif  // end header latch
