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
    /// Simplify access to the stateful data
    typedef typename Basic_<ELEMTYPE>::Stateful_T StateBlock_T;


protected:
    /// Constructor: Invalid MP
    Basic_( DatabaseApi& db, uint32_t pointId, size_t stateSize, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData )
        :Fxt::Point::PointCommon_( db, pointId, pointName, stateSize )
    {
        m_state = allocatorForPointStatefulData.allocate( sizeof( StateBlock_T ) );
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
    Basic_( DatabaseApi& db, uint32_t pointId, size_t stateSize, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData, ELEMTYPE initialValue )
        :Fxt::Point::PointCommon_( db, pointId, pointName, stateSize )
    {
        m_state = allocatorForPointStatefulData.allocate( sizeof( StateBlock_T ) );
        if ( m_state )
        {
            ((StateBlock_T*) m_state)->data = initialValue;
            finishInit( true );
        }
        else
        {
            CPL_SYSTEM_TRACE_MSG( FXT_POINT_TRACE_SECT_, ("State Memory allocation failed for pointID: %lu", pointId) );
        }
    }

public:
    /// Type safe read. See Fxt::Point::Api
    bool read( ELEMTYPE& dstData ) const noexcept
    {
        return Fxt::Point::PointCommon_::readData( &dstData, sizeof( ELEMTYPE ) );
    }

    /// Type safe write. See Fxt::Point::Api
    void write( ELEMTYPE newValue, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        Fxt::Point::PointCommon_::writeData( &newValue, sizeof( ELEMTYPE ), lockRequest );
    }

protected:
    /** See Fxt::Point::Api.  It is the caller's responsibility to
        NOT call this method if m_data is null.

        Note: The PointCommon_.read() will not call this method if the m_state
              is null.
     */
    void copyDataTo_( void* dstData, size_t dstSize ) const noexcept
    {
        CPL_SYSTEM_ASSERT( dstSize == sizeof( ELEMTYPE ) );
        *((ELEMTYPE*) dstData) = ((StateBlock_T*) m_state)->data;
    }

    /** See Fxt::Point::Api.  It is the caller's responsibility to
        NOT call this method if m_data is null.

        Note: The PointCommon_.write() will not call this method if the m_state
              is null.
     */
    void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
    {
        CPL_SYSTEM_ASSERT( srcSize == sizeof( ELEMTYPE ) );
        ((StateBlock_T*) m_state)->data = *((ELEMTYPE*) srcData);
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
    /// Simplify access to the stateful data
    typedef typename Basic_<ELEMTYPE>::Stateful_T StateBlock_T;

protected:
    /// Constructor: Invalid MP
    BasicInteger_( DatabaseApi& db, uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData )
        :Basic_<ELEMTYPE>( db, pointId, sizeof( StateBlock_T ), pointName, allocatorForPointStatefulData )
    {
    }

    /// Constructor: Valid MP (requires initial value)
    BasicInteger_( DatabaseApi& db, uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData, ELEMTYPE initialValue )
        :Basic_<ELEMTYPE>( db, pointId, sizeof( StateBlock_T ), pointName, allocatorForPointStatefulData, initialValue )
    {
    }

public:
    /// Pull in overloaded methods from base class
    using Basic_<ELEMTYPE>::write;

    /// Atomic increment
    void increment( ELEMTYPE incSize = 1, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        write( ((StateBlock_T*) PointCommon_::m_state)->data + incSize, lockRequest );
    }

    /// Atomic decrement
    void decrement( ELEMTYPE decSize = 1, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        write( ((StateBlock_T*) PointCommon_::m_state)->data - decSize, lockRequest );
    }

    /// Atomic 'word' AND
    void maskAnd( ELEMTYPE mask, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        write( ((StateBlock_T*) PointCommon_::m_state)->data & mask, lockRequest );
    }

    /// Atomic 'word' OR
    void maskOr( ELEMTYPE mask, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        write( ((StateBlock_T*) PointCommon_::m_state)->data | mask, lockRequest );
    }

    /// Atomic 'word' XOR
    void maskXor( ELEMTYPE mask, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        write( ((StateBlock_T*) PointCommon_::m_state)->data ^ mask, lockRequest );
    }

    /// Atomic bit toggle.  'bitNum==0' is the Least significant bit
    void bitToggle( unsigned bitNum, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        write( ((StateBlock_T*) PointCommon_::m_state)->data ^ (((ELEMTYPE) 1) << bitNum), lockRequest );
    }

    /// Atomic bit clear.  'bitNum==0' is the Least significant bit
    void bitClear( unsigned bitNum, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        write( ((StateBlock_T*) PointCommon_::m_state)->data & (~(((ELEMTYPE) 1) << bitNum)), lockRequest );
    }

    /// Atomic bit set.  'bitNum==0' is the Least significant bit
    void bitSet( unsigned bitNum, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
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
        if ( src.is<const char*>() )
        {
            const char*        val = src;
            unsigned long long value;
            if ( Cpl::Text::a2ull( value, val, 0 ) == false )
            {
                if ( errorMsg )
                {
                    *errorMsg = "Invalid syntax for the 'val' key/value pair";
                }
                return false;
            }

            newValue = (ELEMTYPE) value;
        }

        // Attempt to parse a numeric decimal
        else if ( src.is<ELEMTYPE>() )
        {
            newValue = src.as<ELEMTYPE>();
        }

        // NOT a hex string or an integer
        else
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

/** This template class extends the implementation of Basic_<ELEMTYPE> to support
    the toJSON() and fromJSON_() methods for read/floating point types.
 */
template<class ELEMTYPE>
class BasicReal_ : public Basic_<ELEMTYPE>
{
public:
    /// Simplify access the stateful data
    typedef typename Basic_<ELEMTYPE>::Stateful_T StateBlock_T;

protected:
    /// Constructor: Invalid MP
    BasicReal_( DatabaseApi& db, uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData )
        :Basic_<ELEMTYPE>( db, pointId, sizeof( StateBlock_T ), pointName, allocatorForPointStatefulData )
    {
    }

    /// Constructor: Valid MP (requires initial value)
    BasicReal_( DatabaseApi& db, uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData, ELEMTYPE initialValue )
        :Basic_<ELEMTYPE>( db, pointId, sizeof( StateBlock_T ), pointName, allocatorForPointStatefulData, initialValue )
    {
    }


public:
    /// Pull in overloaded methods from base class
    using Basic_<ELEMTYPE>::write;

    /// Atomic increment
    void increment( ELEMTYPE incSize = 1.0, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        write( ((StateBlock_T*) PointCommon_::m_state)->data + incSize, lockRequest );
    }

    /// Atomic decrement
    void decrement( ELEMTYPE decSize = 1.0, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
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
        if ( src.is<ELEMTYPE>() == false )
        {
            if ( errorMsg )
            {
                *errorMsg = "Invalid syntax for the 'val' key/value pair";
            }
            return false;
        }

        write( src.as<ELEMTYPE>(), lockRequest );
        return true;
    }
};

};      // end namespaces
};
#endif  // end header latch
