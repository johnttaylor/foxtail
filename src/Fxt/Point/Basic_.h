#ifndef Cpl_Point_Basic_h_
#define Cpl_Point_Basic_h_
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
#include "Cpl/System/FatalError.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/FString.h"
#include <string.h>


///
namespace Cpl {
///
namespace Point {


/** This template class provides a mostly concrete implementation for a Model
    Point who's data is a C primitive type of type: 'ELEMTYPE'.
 */
template<class ELEMTYPE>
class Basic_ : public PointCommon_
{
protected:
    /// The element's value
    ELEMTYPE    m_data;

public:
    /// Constructor: Invalid MP
    Basic_()
        :Cpl::Point::PointCommon_( false )
    {
        m_data = 0;
    }

    /// Constructor: Valid MP (requires initial value)
    Basic_( ELEMTYPE initialValue )
        :Cpl::Point::PointCommon_( true )
    {
        m_data = initialValue;
    }

public:
    /// Type safe read. See Cpl::Point::Api
    virtual bool read( ELEMTYPE& dstData ) const noexcept
    {
        return Cpl::Point::PointCommon_::read( &dstData, sizeof( ELEMTYPE ) );
    }

    /// Returns the Point's value and its meta-data.  'dstData' has no meaning when the method returns false.
    virtual bool get( ELEMTYPE& dstData, bool& isValid, bool& isLocked ) const noexcept
    {
        isValid  = m_valid;
        isLocked = m_locked;
        return read( dstData );
    }

public:
    /// See Cpl::Point::Api.  
    size_t getSize() const noexcept
    {
        return sizeof( ELEMTYPE );
    }

public:
    /// See Cpl::Point::Api. Sets the value to zero - for deterministic behavior of the increment/decrement methods
    void setInvalid( LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        PointCommon_::setInvalid( lockRequest );
        m_data = 0;
    }


protected:
    /// See Cpl::Point::Api
    void copyDataTo_( void* dstData, size_t dstSize ) const noexcept
    {
        CPL_SYSTEM_ASSERT( dstSize == sizeof( ELEMTYPE ) );
        *((ELEMTYPE*)dstData) = m_data;
    }

    /// See Cpl::Point::Api
    void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
    {
        CPL_SYSTEM_ASSERT( srcSize == sizeof( ELEMTYPE ) );
        m_data = *((ELEMTYPE*)srcData);
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
    /// Constructor: Invalid MP
    BasicInteger_()
        :Basic_<ELEMTYPE>()
    {
    }

    /// Constructor: Valid MP (requires initial value)
    BasicInteger_( ELEMTYPE initialValue )
        :Basic_<ELEMTYPE>( initialValue )
    {
    }

    /// Type safe write. See Cpl::Point::Api
    virtual ELEMTYPE write( ELEMTYPE newValue, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST ) noexcept
    {
        Cpl::Point::PointCommon_::write( &newValue, sizeof( ELEMTYPE ), lockRequest );
        return Basic_<ELEMTYPE>::m_data;
    }

    /// Atomic increment
    virtual ELEMTYPE increment( ELEMTYPE incSize = 1, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST ) noexcept
    {
        write( Basic_<ELEMTYPE>::m_data + incSize, lockRequest );
        return Basic_<ELEMTYPE>::m_data;
    }

    /// Atomic decrement
    virtual ELEMTYPE decrement( ELEMTYPE decSize = 1, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST ) noexcept
    {
        write( Basic_<ELEMTYPE>::m_data - decSize, lockRequest );
        return Basic_<ELEMTYPE>::m_data;
    }

    /// Atomic 'word' AND
    virtual ELEMTYPE maskAnd(ELEMTYPE mask, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST) noexcept
    {
        write( Basic_<ELEMTYPE>::m_data & mask, lockRequest );
        return Basic_<ELEMTYPE>::m_data;
    }

    /// Atomic 'word' OR
    virtual ELEMTYPE maskOr(ELEMTYPE mask, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST) noexcept
    {
        write( Basic_<ELEMTYPE>::m_data | mask, lockRequest );
        return Basic_<ELEMTYPE>::m_data;
    }

    /// Atomic 'word' XOR
    virtual ELEMTYPE maskXor(ELEMTYPE mask, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST) noexcept
    {
        write( Basic_<ELEMTYPE>::m_data ^ mask, lockRequest );
        return Basic_<ELEMTYPE>::m_data;
    }

    /// Atomic bit toggle.  'bitNum==0' is the Least significant bit
    virtual ELEMTYPE bitToggle( unsigned bitNum, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST ) noexcept
    {
        write( Basic_<ELEMTYPE>::m_data ^ (((ELEMTYPE)1) << bitNum), lockRequest );
        return Basic_<ELEMTYPE>::m_data;
    }
    /// Atomic bit clear.  'bitNum==0' is the Least significant bit
    virtual ELEMTYPE bitClear( unsigned bitNum, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST ) noexcept
    {
        write( Basic_<ELEMTYPE>::m_data & (~(((ELEMTYPE)1) << bitNum)), lockRequest );
        return Basic_<ELEMTYPE>::m_data;
    }

    /// Atomic bit set.  'bitNum==0' is the Least significant bit
    virtual ELEMTYPE bitSet( unsigned bitNum, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST ) noexcept
    {
        write( Basic_<ELEMTYPE>::m_data | (((ELEMTYPE)1) << bitNum), lockRequest );
        return Basic_<ELEMTYPE>::m_data;
    }

public:
    /// See Cpl::Point::Api.  
    bool toJSON_( JsonDocument& doc, bool verbose = true ) noexcept
    {
        // Construct the 'val' key/value pair (as a HEX string)
        Cpl::Text::FString<20> tmp;
        tmp.format( "0x%llX", (unsigned long long) Basic_<ELEMTYPE>::m_data );
        doc["val"] = (char*)tmp.getString();
        return true;
    }

    /// See Cpl::Point::Api.  
    bool fromJSON_( JsonVariant& src, Cpl::Point::Api::LockRequest_T lockRequest, Cpl::Text::String* errorMsg=0 ) noexcept
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

        newValue = (ELEMTYPE)value;

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
public:
    /// Constructor: Invalid MP
    BasicReal_()
        :Basic_<ELEMTYPE>()
    {
    }

    /// Constructor: Valid MP (requires initial value)
    BasicReal_(ELEMTYPE initialValue)
        :Basic_<ELEMTYPE>( initialValue )
    {
    }


public:
    /// Type safe write. See Cpl::Point::Api
    virtual ELEMTYPE write( ELEMTYPE newValue, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST ) noexcept
    {
        Cpl::Point::PointCommon_::write( &newValue, sizeof( ELEMTYPE ), lockRequest );
        return Basic_<ELEMTYPE>::m_data;
    }

    /// Atomic increment
    virtual ELEMTYPE increment( ELEMTYPE incSize = 1.0, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST ) noexcept
    {
        write( Basic_<ELEMTYPE>::m_data + incSize, lockRequest );
        return Basic_<ELEMTYPE>::m_data;
    }

    /// Atomic decrement
    virtual ELEMTYPE decrement( ELEMTYPE decSize = 1.0, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST ) noexcept
    {
        write( Basic_<ELEMTYPE>::m_data - decSize, lockRequest );
        return Basic_<ELEMTYPE>::m_data;
    }


public:
    /// See Cpl::Point::Api.  
    bool toJSON_( JsonDocument& doc, bool verbose = true ) noexcept
    {
        // Construct the 'val' key/value pair 
        doc["val"] = Basic_<ELEMTYPE>::m_data;
        return true;
    }

    /// See Cpl::Point.  
    bool fromJSON_( JsonVariant& src, Cpl::Point::Api::LockRequest_T lockRequest, Cpl::Text::String* errorMsg=0 ) noexcept
    {
        ELEMTYPE checkForError = src | (ELEMTYPE)2;
        ELEMTYPE newValue      = src | (ELEMTYPE)1;
        if ( newValue <= (ELEMTYPE)1 && checkForError >= (ELEMTYPE)2 )
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
