#ifndef Fxt_Point_String_h_
#define Fxt_Point_String_h_
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

#include "Fxt/Point/PointCommon_.h"
#include "Cpl/Text/String.h"


///
namespace Fxt {
///
namespace Point {

/** This partially concrete class provide common, non-size dependent implementation
    for a Point who's data is a null terminated string

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:"<newvalue>" }

    \endcode


 */
class StringBase_ : public Fxt::Point::PointCommon_
{
protected:
    /** Constructor. Invalid Point.
     */
    StringBase_( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData, size_t sizeofData );

    /// Constructor. Valid Point.  Requires an initial value
    StringBase_( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData, size_t sizeofData, const char* initialValue );


public:
    /// Type safe read. See Fxt::Point::Api.
    virtual bool read( Cpl::Text::String& dstData ) const noexcept;

    /// Type safe read. See Fxt::Point::Api. Note: 'dstSizeInBytes' is assumed to include the storage for the null terminator
    virtual bool read( char* dstData, size_t dstSizeInBytes ) const noexcept;

    /// Type safe write. See Fxt::Point::Api
    virtual void write( const char* srcString, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept;

    /// Returns the size WITHOUT the null terminator of the string storage
    virtual size_t getMaxLength() const noexcept = 0;

public:
    /// See Fxt::Point::Api.  
    bool toJSON_( JsonDocument& doc, bool verbose = true ) noexcept;

    /// See Fxt::Point::Api.  
    bool fromJSON_( JsonVariant & src, Fxt::Point::Api::LockRequest_T lockRequest, Cpl::Text::String * errorMsg=0 ) noexcept;

protected:

    /** See Fxt::Point::Api.  It is the caller's responsibility to
    NOT call this method if m_data is null.

    Note: The PointCommon_.read() will not call this method if the m_state
          is null.
     */
    void copyDataTo_( void* dstData, size_t dstSize ) const noexcept;

    /** See Fxt::Point::Api.  It is the caller's responsibility to
        NOT call this method if m_data is null.

        Note: The PointCommon_.write() will not call this method if the m_state
              is null.
     */
    void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept;

private:
    /// Typedef that represents the 'final' stateful data allocation
    struct BaseStateful_T
    {
        Fxt::Point::PointCommon_::Metadata_T  meta;
        char                                  data[1];
    };

};


/** This mostly concrete template class provides the storage for a Point
    who's data is a null terminated string.  The template parameter specifies
    the size of string storage. The child classes must provide the following
    methods:

        write( MyChildStringPointClass& src, ... )
        getType()
        create(...)

    Template Args:  S:=  Max Size of the String WITHOUT the null
                         terminator!
 */
template<int S>
class String_ : public StringBase_
{
protected:
    /** Constructor. Invalid Point.
     */
    String_( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData )
        : StringBase_( pointId, pointName, allocatorForPointStatefulData, sizeof( StateBlock_T) )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value
    String_( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData, const char* initialValue )
        : StringBase_( pointId, pointName, allocatorForPointStatefulData, sizeof( StateBlock_T ), initialValue )
    {
    }

public:
    /// Updates the MP's data from 'src'. 
    void write( MyString& src, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST ) noexcept
    {
        if ( src.isNotValid() )
        {
            setInvalid();
        }
        else
        {
            String_<STR_LEN>::write( src.m_data, lockRequest );
        }
    }

    /// See Fxt::Point::StringBase_
    size_t getMaxLength() const noexcept { return S; }

    /// See Fxt::Point::Api.  
    size_t getStatefulMemorySize() const noexcept { return sizeof( StateBlock_T ); }


public:
    /// The Point's Stateful data
    struct StateBlock_T
    {
        Metadata_T  meta;           //!< The Point's meta-data. THIS MUST BE THE FIRST ELEMENT in the structure
        char        data[S + 1];    //!< The Point's 'data'
    };
};



};      // end namespaces
};
#endif  // end header latch
