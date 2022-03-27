#ifndef Fxt_Point_Basic_h_
#define Fxt_Point_Basic_h_
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
#include "Cpl/System/Assert.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/FString.h"
#include <string.h>

/// Trace Section label for Point infrastructure
#define FXT_POINT_TRACE_SECT_     "Fxt::Point"

///
namespace Fxt {
///
namespace Point {


/** This template class provides a mostly concrete implementation for a Model
    Point who's data is a C primitive type (or struct) of type: 'ELEMTYPE'.
 */
template<class ELEMTYPE>
class Basic_ : public PointCommon_
{
protected:
    /// The Point's Stateful data
    struct Stateful_T
    {
        Metadata_T  meta;           //!< The Point's meta-data. THIS MUST BE THE FIRST ELEMENT in the structure
        ELEMTYPE    data;           //!< The Point's 'data'
    };

public:
    /// Constructor: Invalid MP
    Basic_( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData )
        :Fxt::Point::PointCommon_( pointId, pointName )
    {
        m_state = allocatorForPointStatefulData.allocate( sizeof( Basic_<ELEMTYPE>::Stateful_T ) );
        if ( m_state )
        {
            memset( m_state, 0, sizeof( ELEMTYPE ) );
            finishInit( false );
        }
        else
        {
            CPL_SYSTEM_TRACE_MSG( FXT_POINT_TRACE_SECT_, ("State Memory allocation failed for pointID: %lu", pointId) );
        }
    }

    /// Constructor: Valid MP (requires initial value)
    Basic_( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData, ELEMTYPE initialValue )
        :Fxt::Point::PointCommon_( pointId, pointName )
    {
        m_state = allocatorForPointStatefulData.allocate( sizeof( Basic_<ELEMTYPE>::Stateful_T ) );
        if ( m_state )
        {
            ((Basic_<ELEMTYPE>::Stateful_T*) m_state)->data = initialValue;
            finishInit( true );
        }
        else
        {
            CPL_SYSTEM_TRACE_MSG( FXT_POINT_TRACE_SECT_, ("State Memory allocation failed for pointID: %lu", pointId) );
        }
    }

public:
    /// Type safe read. See Fxt::Point::Api
    virtual bool read( ELEMTYPE& dstData ) const noexcept
    {
        return Fxt::Point::PointCommon_::read( &dstData, sizeof( ELEMTYPE ) );
    }

public:
    /// See Fxt::Point::Api.  
    size_t getStatefulMemorySize() const noexcept
    {
        return sizeof( Basic_<ELEMTYPE>::Stateful_T );
    }

public:
    /// See Fxt::Point::Api. Sets the value to zero - for deterministic behavior of the increment/decrement methods
    void setInvalid( LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        PointCommon_::setInvalid( lockRequest );
        if ( m_state )
        {
            memset( m_state, 0, sizeof( ELEMTYPE ) );
        }
    }

protected:
    /** See Fxt::Point::Api.  It is the caller's responsibility to
        NOT call this method if m_data is null.

        Note: The PointCommon_.read() will not call this method if the m_data
              is null.
     */
    void copyDataTo_( void* dstData, size_t dstSize ) const noexcept
    {
        CPL_SYSTEM_ASSERT( dstSize == sizeof( ELEMTYPE ) );
        *((ELEMTYPE*) dstData) = ((Basic_<ELEMTYPE>::Stateful_T*) m_state)->data;
    }

    /** See Fxt::Point::Api.  It is the caller's responsibility to
        NOT call this method if m_data is null.

        Note: The PointCommon_.write() will not call this method if the m_data
              is null.
     */
    void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
    {
        CPL_SYSTEM_ASSERT( srcSize == sizeof( ELEMTYPE ) );
        ((Basic_<ELEMTYPE>::Stateful_T*) m_state)->data = *((ELEMTYPE*) srcData);
    }
};



/** This template class extends the implementation of Basic_<ELEMTYPE> to support
    the toJSON() and fromJSON_() methods for integer types.  For the 'val' key/value
    pair - the value is displayed in Hexadecimal.
 */
template<class ELEMTYPE>
class BasicInteger_ : public Basic_<ELEMTYPE>
{
public:
    /// Simplify access the stateful data
    typedef typename Basic_<ELEMTYPE>::Stateful_T StateBlock_T;

    /// Constructor: Invalid MP
    BasicInteger_( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData )
        :Basic_<ELEMTYPE>( pointId, pointName, allocatorForPointStatefulData )
    {
    }

    /// Constructor: Valid MP (requires initial value)
    BasicInteger_( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData, ELEMTYPE initialValue )
        :Basic_<ELEMTYPE>( pointId, pointName, allocatorForPointStatefulData, initialValue )
    {
    }

    /// Type safe write. See Fxt::Point::Api
    virtual void write( ELEMTYPE newValue, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        Fxt::Point::PointCommon_::write( &newValue, sizeof( ELEMTYPE ), lockRequest );
    }

    /// Atomic increment
    virtual void increment( ELEMTYPE incSize = 1, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        write( ((StateBlock_T*) PointCommon_::m_state)->data + incSize, lockRequest );
    }

    /// Atomic decrement
    virtual void decrement( ELEMTYPE decSize = 1, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        write( ((StateBlock_T*) PointCommon_::m_state)->data - decSize, lockRequest );
    }

    /// Atomic 'word' AND
    virtual void maskAnd( ELEMTYPE mask, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        write( ((StateBlock_T*) PointCommon_::m_state)->data & mask, lockRequest );
    }

    /// Atomic 'word' OR
    virtual void maskOr( ELEMTYPE mask, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        write( ((StateBlock_T*) PointCommon_::m_state)->data | mask, lockRequest );
    }

    /// Atomic 'word' XOR
    virtual void maskXor( ELEMTYPE mask, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        write( ((StateBlock_T*) PointCommon_::m_state)->data ^ mask, lockRequest );
    }

    /// Atomic bit toggle.  'bitNum==0' is the Least significant bit
    virtual void bitToggle( unsigned bitNum, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        write( ((StateBlock_T*) PointCommon_::m_state)->data ^ (((ELEMTYPE) 1) << bitNum), lockRequest );
    }
    /// Atomic bit clear.  'bitNum==0' is the Least significant bit
    virtual void bitClear( unsigned bitNum, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        write( ((StateBlock_T*) PointCommon_::m_state)->data & (~(((ELEMTYPE) 1) << bitNum)), lockRequest );
    }

    /// Atomic bit set.  'bitNum==0' is the Least significant bit
    virtual void bitSet( unsigned bitNum, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        write( ((StateBlock_T*) PointCommon_::m_state)->data | (((ELEMTYPE) 1) << bitNum), lockRequest );
    }

public:
    /** See Fxt::Point::Api.  Note: This method is NOT called when the MP is 
        invalid (see comments above the MP always being invalid if m_data 
        allocation failed)
     */
    bool toJSON_( JsonDocument& doc, bool verbose = true ) noexcept
    {
        // Construct the 'val' key/value pair (as a HEX string)
        Cpl::Text::FString<20> tmp;
        tmp.format( "0x%llX", (unsigned long long) (((StateBlock_T*) PointCommon_::m_state)->data) );
        doc["val"] = (char*) tmp.getString();
        return true;
    }

    /** See Fxt::Point::Api.  Note: This method is NOT called when the MP is
        invalid (see comments above the MP always being invalid if m_data
        allocation failed)
     */
    bool fromJSON_( JsonVariant& src, Fxt::Point::Api::LockRequest_T lockRequest, Cpl::Text::String* errorMsg=0 ) noexcept
    {
        ELEMTYPE newValue = 0;

        // Attempt to parse the value as HEX string
        const char*        val = src;
        unsigned long long value;
        if ( Cpl::Text::a2ull( value, val, 16 ) == false )
        {
            if ( errorMsg )
            {
                *errorMsg = "Invalid syntax for the 'val' key/value pair";
            }
            return false;
        }

        newValue = (ELEMTYPE) value;

        write( newValue, lockRequest );
        return true;
    }
};

/** This template class extends the implementation of Basic_<ELEMTYPE> to support
    the toJSON() and fromJSON_() methods for read/floating point types.

    NOTES:
    1) All methods in this class are NOT thread Safe unless explicitly
    documented otherwise.
 */
template<class ELEMTYPE>
class BasicReal_ : public Basic_<ELEMTYPE>
{
protected:
    /// Simplify access the stateful data
    typedef typename Basic_<ELEMTYPE>::Stateful_T StateBlock_T;

public:
    /// Constructor: Invalid MP
    BasicReal_( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData )
        :Basic_<ELEMTYPE>(pointId, pointName, allocatorForPointStatefulData )
    {
    }

    /// Constructor: Valid MP (requires initial value)
    BasicReal_( uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData, ELEMTYPE initialValue )
        :Basic_<ELEMTYPE>( pointId, pointName, allocatorForPointStatefulData )
    {
    }


public:
    /// Type safe write. See Fxt::Point::Api
    virtual void write( ELEMTYPE newValue, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        Fxt::Point::PointCommon_::write( &newValue, sizeof( ELEMTYPE ), lockRequest );
    }

    /// Atomic increment
    virtual void increment( ELEMTYPE incSize = 1.0, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        write( ((StateBlock_T*) PointCommon_::m_state)->data + incSize, lockRequest );
    }

    /// Atomic decrement
    virtual void decrement( ELEMTYPE decSize = 1.0, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        write( ((StateBlock_T*) PointCommon_::m_state)->data - decSize, lockRequest );
    }


public:
    /** See Fxt::Point::Api.  Note: This method is NOT called when the MP is
        invalid (see comments above the MP always being invalid if m_data
        allocation failed)
     */
    bool toJSON_( JsonDocument& doc, bool verbose = true ) noexcept
    {
        // Construct the 'val' key/value pair 
        doc["val"] = ((StateBlock_T*) PointCommon_::m_state)->data;
        return true;
    }

    /** See Fxt::Point::Api.  Note: This method is NOT called when the MP is
        invalid (see comments above the MP always being invalid if m_data
        allocation failed)
     */
    bool fromJSON_( JsonVariant& src, Fxt::Point::Api::LockRequest_T lockRequest, Cpl::Text::String* errorMsg=0 ) noexcept
    {
        ELEMTYPE checkForError = src | (ELEMTYPE) 2;
        ELEMTYPE newValue      = src | (ELEMTYPE) 1;
        if ( newValue <= (ELEMTYPE) 1 && checkForError >= (ELEMTYPE) 2 )
        {
            if ( errorMsg )
            {
                *errorMsg = "Invalid syntax for the 'val' key/value pair";
            }
            return false;
        }

        write( newValue, lockRequest );
        return true;
    }
};

};      // end namespaces
};
#endif  // end header latch
