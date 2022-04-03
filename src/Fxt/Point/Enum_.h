#ifndef Fxt_Point_Enum_h_
#define Fxt_Point_Enum_h_
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
#include "Cpl/System/Assert.h"

///
namespace Fxt {
///
namespace Point {


/** This class provides a MOSTLY concrete implementation for a Point who's data
    is a an a "Better Enum", a.k.a a "enum" defined using the BETTER_ENUM macro
    defined in Cpl/Type/enum.h.  A child class is still needed to provide the
    following methods:

        write( MyChildEnumPointClass& src, ... )
        getTypeAsText()
        create(...)

    The toJSON()/fromJSON format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:"enum" }

        \endcode

 */
template<class BETTERENUM_TYPE>
class Enum_ : public Basic_<BETTERENUM_TYPE>
{
public:
    /// Simplify access the stateful data
    typedef typename Basic_<BETTERENUM_TYPE>::Stateful_T StateBlock_T;

protected:
    /// Constructor: Invalid MP
    Enum_( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData )
        :Basic_<BETTERENUM_TYPE>( pointId, pointName, allocatorForPointStatefulData )
    {
    }

    /// Constructor: Valid MP (requires initial value)
    Enum_( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData, BETTERENUM_TYPE initialValue )
        :Basic_<BETTERENUM_TYPE>( pointId, pointName, allocatorForPointStatefulData, initialValue )
    {
    }

public:
    /// Type safe write. See Fxt::Dm::ModelPoint
    virtual void write( BETTERENUM_TYPE newValue, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::LockRequest_T::eNO_REQUEST ) noexcept
    {
        PointCommon_::write( &newValue, sizeof( BETTERENUM_TYPE ), lockRequest );
    }

public:
    /// See Fxt::Dm::Point.  
    bool toJSON_( JsonDocument & doc, bool verbose = true ) noexcept
    {
        // Construct the 'val' key/value pair 
        doc["val"] = (char*) ((StateBlock_T*) PointCommon_::m_state)->data._to_string();
        return true;
    }

    /// See Fxt::Dm::Point.  
    bool fromJSON_( JsonVariant& src, Fxt::Point::Api::LockRequest_T lockRequest, Cpl::Text::String* errorMsg=0 ) noexcept
    {
        // Get the enum string
        const char* newValue = src;
        if ( newValue == nullptr )
        {
            if ( errorMsg )
            {
                *errorMsg = "Invalid syntax for the 'val' key/value pair";
            }
            return false;
        }

        // Convert the text to an enum value
        auto maybeValue = BETTERENUM_TYPE::_from_string_nothrow( newValue );
        if ( !maybeValue )
        {
            if ( errorMsg )
            {
                errorMsg->format( "Invalid enum value (%s)", newValue );
            }
            return false;
        }

        write( *maybeValue, lockRequest );
        return true;
    }
};



};      // end namespaces
};
#endif  // end header latch
