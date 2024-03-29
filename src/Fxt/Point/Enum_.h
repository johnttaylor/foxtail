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

        getTypeGuid()
        getTypeName()

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
    Enum_( DatabaseApi&                         db, 
           uint32_t                             pointId, 
           Cpl::Memory::ContiguousAllocator&    allocatorForPointStatefulData,
           Api*                                 setterPoint )
        :Basic_<BETTERENUM_TYPE>( db, pointId, sizeof(StateBlock_T), allocatorForPointStatefulData, setterPoint )
    {
    }


public:
    /// Pull in overloaded methods from base class
    using Basic_<BETTERENUM_TYPE>::write;

    /// Updates the MP's data from 'src'. Note: The lock state of 'src' is NOT-USED/IGNORED
    void write( Enum_<BETTERENUM_TYPE>& src, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        PointCommon_::updateFrom_( &(((StateBlock_T*) (src.m_state))->data), sizeof( BETTERENUM_TYPE ), src.isNotValid(), lockRequest );
    }

    ///  See Fxt::Point::Api
    void updateFromSetter() noexcept { if ( PointCommon_::m_setter ) { write( *((Enum_<BETTERENUM_TYPE>*) PointCommon_::m_setter) ); } }

    ///  See Fxt::Point::Api
    void updateFromSetter( const Api&    srcPt,
                           LockRequest_T lockRequest = eNO_REQUEST,
                           bool          beSafe      = true ) noexcept
    {
        if ( !beSafe || isSameType( srcPt ) ) { write( *((Enum_<BETTERENUM_TYPE>*) &srcPt), lockRequest ); }
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
