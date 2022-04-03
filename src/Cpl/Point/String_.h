#ifndef Cpl_Point_String_h_
#define Cpl_Point_String_h_
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
#include "Cpl/Text/String.h"


///
namespace Cpl {
///
namespace Point {

/** This partially concrete class provide common, non-size dependent implementation
    for a Point who's data is a null terminated string

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:"<newvalue>" }

    \endcode


 */
class StringBase_ : public Cpl::Point::PointCommon_
{
protected:
    /** Constructor. Invalid Point.
     */
    StringBase_();

    /// Constructor. Valid Point.  Requires an initial value
    StringBase_( const char* initialValue );


public:
    /// Type safe read. See Cpl::Point::Api
    virtual bool read( Cpl::Text::String& dstData ) const noexcept;

    /// Returns the Point's value and its meta-data.  'dstData' has no meaning when the method returns false.
    virtual bool get( Cpl::Text::String& dstData, bool& isValid, bool& isLocked ) const noexcept;

    /// Type safe read. Note: 'dstSizeInBytes' is assumed to include the storage for the null terminator
    virtual bool read( char* dstData, size_t dstSizeInBytes ) const noexcept;

    /// Returns the Point's value and its meta-data.  'dstData' has no meaning when the method returns false.
    virtual bool get( char* dstData, size_t dstSizeInBytes, bool& isValid, bool& isLocked ) const noexcept;

    /// Type safe write. See Cpl::Point::Api
    virtual const char* write( const char* srcString, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST ) noexcept;

    /// See Cpl::Point::Api.  Note: This returns the actual STORAGE size - the max string length size is getSize() - 1.
    size_t getSize() const noexcept;

    /// Returns the size WITHOUT the null terminator of the string storage
    virtual size_t getMaxLength() const noexcept = 0;

public:
    /// See Cpl::Point::Api.  
    bool toJSON_( JsonDocument& doc, bool verbose = true ) noexcept;

    /// See Cpl::Point::Api.  
    bool fromJSON_( JsonVariant & src, Cpl::Point::Api::LockRequest_T lockRequest, Cpl::Text::String * errorMsg=0 ) noexcept;

protected:
    /// Returns a pointer to the point's string storage
    virtual char* getDataPtr() noexcept = 0;

    /// Returns a CONST pointer to the point's string storage
    virtual const char* getConstDataPtr() const noexcept  = 0; 

    /// See Cpl::Point::Api
    void copyDataTo_( void* dstData, size_t dstSize ) const noexcept;

    /// See Cpl::Point::Api
    void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept;
};


/** This mostly concrete template class provides the storage for a Point
    who's data is a null terminated string.  The template parameter specifies
    the size of string storage. The child classes must provide the following
    methods:

        getId()
        getTypeAsText()

    Template Args:  S:=  Max Size of the String_ WITHOUT the null
                         terminator!
 */
template<int S>
class String_ : public StringBase_
{
protected:
    /** Constructor. Invalid Point.
     */
    String_() : StringBase_(){ m_data[0] = '\0';  }

    /// Constructor. Valid Point.  Requires an initial value
    String_( const char* initialValue ) : StringBase_( initialValue ){ copyDataFrom_( initialValue, strlen( initialValue ) ); }

public:
    /// Returns the size WITHOUT the null terminator of the string storage
    size_t getMaxLength() const noexcept { return S; }

protected:
    /// Returns a pointer to the point's string storage
    char* getDataPtr() noexcept { return m_data; }

    /// Returns a CONST pointer to the point's string storage
    const char* getConstDataPtr() const noexcept { return m_data; }

protected:
    /// Storage for the Point data
    char m_data[S + 1];
};



};      // end namespaces
};
#endif  // end header latch
