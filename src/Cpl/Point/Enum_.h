#ifndef Cpl_Point_Enum_h_
#define Cpl_Point_Enum_h_
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


#include "Cpl/Point/PointCommon_.h"
#include "Cpl/System/Assert.h"

///
namespace Cpl {
///
namespace Point {


/** This class provides a MOSTLY concrete implementation for a Point who's data
    is a an a "Better Enum", a.k.a a "enum" defined using the BETTER_ENUM macro
    defined in Cpl/Type/enum.h.  A child class is still needed to provide the
    following methods:

        getId()
        getTypeAsText()

    The toJSON()/fromJSON format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:"enum" }

        \endcode

 */
template<class BETTERENUM_TYPE>
class Enum_ : public PointCommon_
{
protected:
    /** Constructor. Invalid Point.
     */
    Enum_() : PointCommon_( false ), m_data( BETTERENUM_TYPE::_values()[0] ) {}

    /// Constructor. Valid Point.  Requires an initial value
    Enum_( BETTERENUM_TYPE initialValue ) : PointCommon_( true ), m_data( initialValue ) {}

public:
    /// Type safe read. See Cpl::Point::Api
    virtual bool read( BETTERENUM_TYPE& dstData ) const noexcept
    {
        return Cpl::Point::PointCommon_::read( &dstData, sizeof( BETTERENUM_TYPE ) );
    }

    /// Type safe write. See Cpl::Dm::ModelPoint
    virtual BETTERENUM_TYPE write( BETTERENUM_TYPE newValue, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        PointCommon_::write( &newValue, sizeof( BETTERENUM_TYPE ), lockRequest );
        return newValue;
    }

    /// Returns the Point's value and its meta-data.  'dstData' has no meaning when the method returns false.
    virtual bool get( BETTERENUM_TYPE& dstData, bool& isValid, bool& isLocked ) const noexcept
    {
        isValid  = m_valid;
        isLocked = m_locked;
        return read( dstData );
    }

public:
    /// See Cpl::Point::Api.  
    size_t getSize() const noexcept
    {
        return sizeof( BETTERENUM_TYPE );
    }

public:
    /// See Cpl::Dm::Point.  
    bool toJSON_( JsonDocument & doc, bool verbose = true ) noexcept
    {
        // Construct the 'val' key/value pair 
        doc["val"] = (char*) m_data._to_string();
        return true;
    }

    /// See Cpl::Dm::Point.  
    bool fromJSON_( JsonVariant& src, Cpl::Point::Api::LockRequest_T lockRequest, Cpl::Text::String* errorMsg=0 ) noexcept
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

protected:
    /// See Cpl::Point::Api
    void copyDataTo_( void* dstData, size_t dstSize ) const noexcept
    {
        CPL_SYSTEM_ASSERT( dstSize == sizeof( BETTERENUM_TYPE ) );
        *((BETTERENUM_TYPE*)dstData) = m_data;
    }

    /// See Cpl::Point::Api
    void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
    {
        CPL_SYSTEM_ASSERT( srcSize == sizeof( BETTERENUM_TYPE ) );
        m_data = *((BETTERENUM_TYPE*)srcData);
    }

protected:
    /// My data
    BETTERENUM_TYPE m_data;
};



};      // end namespaces
};
#endif  // end header latch
