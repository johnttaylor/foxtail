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

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:{maxLen:<n>,text:"<value>" }}

    \endcode


 */
class String : public Fxt::Point::PointCommon_
{
public:
    /// Type ID for the point
    static constexpr const char* GUID_STRING = "e2c8172c-fca3-4023-bb9f-79f92dec7c34";

    /// Type name for the card
    static constexpr const char* TYPE_NAME   = "Fxt::Point::String";

public:
    /** Constructor. Invalid Point.
     */
    String( DatabaseApi&                        db, 
            uint32_t                            pointId, 
            Cpl::Memory::ContiguousAllocator&   allocatorForPointStatefulData, 
            size_t                              stringLenInBytesWithoutNullTerminator,
            Api*                                setterPoint = nullptr );

public:
    /// Type safe read. See Fxt::Point::Api.
    bool read( Cpl::Text::String& dstData ) const noexcept;

    /// Type safe read. See Fxt::Point::Api. Note: 'dstSizeInBytes' is assumed to include the storage for the null terminator
    bool read( char* dstData, size_t dstSizeInBytes ) const noexcept;

    /// Type safe write. See Fxt::Point::Api
    void write( const char* srcString, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept;

    /// Updates the MP's data from 'src'. 
    void write( String& src, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept;

    ///  See Fxt::Point::Api
    void updateFromSetter() noexcept { if ( m_setter ) { write( *((String*) m_setter) ); } }

    ///  See Fxt::Point::Api
    void updateFromSetter( const Api&    srcPt,
                           LockRequest_T lockRequest = eNO_REQUEST,
                           bool          beSafe      = true ) noexcept
    {
        if ( !beSafe || isSameType( srcPt ) ) { write( *((String*) &srcPt), lockRequest ); }
    }

    /// Returns the maximum size WITHOUT the null terminator of the string storage
    size_t getMaxLength() const noexcept;

    /// See Fxt::Point::Api.
    size_t getDataSize_() noexcept;

    /// See Fxt::Point::Api.
    void* getDataPointer_() noexcept;

    ///  See Fxt::Point::Api
    const char* getTypeGuid() const noexcept { return GUID_STRING; }

    ///  See Fxt::Point::Api
    const char* getTypeName() const noexcept { return TYPE_NAME; }

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
    struct StringStateful_T
    {
        Fxt::Point::PointCommon_::Metadata_T  meta;
        char                                  data[1];
    };

};




};      // end namespaces
};
#endif  // end header latch