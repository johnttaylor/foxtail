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
#include "Cpl/System/FatalError.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/FString.h"
#include <string.h>


///
namespace Fxt {
///
namespace Point {


/** This template class provides a mostly concrete implementation for a Model
	Point who's data is a C primitive type of type: 'ELEMTYPE'.

	NOTES:
		1) All methods in this class are NOT thread Safe unless explicitly
		documented otherwise.
 */
template<class ELEMTYPE>
class Basic_ : public Fxt::Point::PointCommon_
{
protected:
	/// The element's value
	ELEMTYPE    m_data;

public:
	/// Constructor: Invalid MP
	Basic_()
		:Fxt::Point::PointCommon_( false )
	{
	}

	/// Constructor: Valid MP (requires initial value)
	Basic_( ELEMTYPE initialValue )
		:Fxt::Point::PointCommon_( true )
	{
		m_data = initialValue;
	}

public:
	/// Type safe read. See Fxt::Point::Api
	virtual bool read( ELEMTYPE& dstData ) const noexcept
	{
		return Fxt::Point::PointCommon_::read( &dstData, sizeof( ELEMTYPE ) );
	}

	/// Returns the Point's value and its meta-data.  'dstData' has no meaning when the method returns false.
	virtual bool get( ELEMTYPE& dstData, bool& isValid, bool& isLocked ) const noexcept
	{
		isValid = m_valid;
		isLocked = m_locked;
		return read( dstData );
	}

public:
	/// See Fxt::Point::Api.  
	size_t getSize() const noexcept
	{
		return sizeof( ELEMTYPE );
	}

public:
	/// See Fxt::Point::Api
	void copyDataTo_( void* dstData, size_t dstSize ) const noexcept
	{
		CPL_SYSTEM_ASSERT( dstSize == sizeof( ELEMTYPE ) );
		*((ELEMTYPE*)dstData) = m_data;
	}

	/// See Fxt::Point::Api
	void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
	{
		CPL_SYSTEM_ASSERT( srcSize == sizeof( ELEMTYPE ) );
		m_data = *((ELEMTYPE*)srcData);
	}
};



/** This template class extends the implementation of Basic_<ELEMTYPE> to support
	the toJSON() and fromJSON_() methods for integer types.

	NOTES:
		1) All methods in this class are NOT thread Safe unless explicitly
		   documented otherwise.
 */
template<class ELEMTYPE>
class BasicInteger_ : public Fxt::Point::Basic_<ELEMTYPE>
{
public:
	/// Constructor: Invalid MP
	BasicInteger_( bool decimalFormat = true )
		:Basic_<ELEMTYPE>()
		, m_decimal( decimalFormat )
	{
	}

	/// Constructor: Valid MP (requires initial value)
	BasicInteger_( ELEMTYPE initialValue, bool decimalFormat = true )
		:Basic_<ELEMTYPE>( initialValue )
		, m_decimal( decimalFormat )
	{
	}

	/// Type safe write. See Fxt::Point::Api
	virtual void write( ELEMTYPE newValue, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
	{
		return Fxt::Point::PointCommon_::write( &newValue, sizeof( ELEMTYPE ), lockRequest );
	}

	/// Atomic increment
	virtual void increment( ELEMTYPE incSize = 1, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
	{
		write( Basic_<ELEMTYPE>::m_data + incSize, lockRequest );
	}

	/// Atomic decrement
	virtual void decrement( ELEMTYPE decSize = 1, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
	{
		write( Basic_<ELEMTYPE>::m_data - decSize, lockRequest );
	}


public:
	/// See Cpl::Dm::Point.  
	bool toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose = true ) noexcept
	{
		// Get a snapshot of the my data and state
		ELEMTYPE value;
		bool     valid;
		bool     locked;
		get( value, valid, locked );

		// Start the conversion
		JsonDocument& doc = Fxt::Point::PointCommon_::beginJSON( valid, locked, seqnum, verbose );

		// Construct the 'val' key/value pair (as a simple numeric)
		if ( valid )
		{
			if ( m_decimal )
			{
				doc["val"] = value;
			}

			// Construct the 'val' key/value pair (as a HEX string)
			else
			{
				Cpl::Text::FString<20> tmp;
				tmp.format( "0x%llX", (unsigned long long) value );
				doc["val"] = (char*)tmp.getString();
			}
		}

		// End the conversion
		Fxt::Point::PointCommon_::endJSON( dst, dstSize, truncated, verbose );
		return true;
	}

	/// See Cpl::Dm::Point.  
	bool fromJSON_( JsonVariant& src, Fxt::Point::Api::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
	{
		ELEMTYPE newValue = 0;

		// Attempt to parse the value key/value pair (as a simple numeric)
		if ( m_decimal )
		{
			ELEMTYPE checkForError = src | (ELEMTYPE)2;
			newValue = src | (ELEMTYPE)1;
			if ( newValue == (ELEMTYPE)1 && checkForError == (ELEMTYPE)2 )
			{
				if ( errorMsg )
				{
					*errorMsg = "Invalid syntax for the 'val' key/value pair";
				}
				return false;
			}
		}

		// Attempt to parse the value as HEX string
		else
		{
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
		}

		retSequenceNumber = Basic_<ELEMTYPE>::write( newValue, lockRequest );
		return true;
	}

protected:
	/// Flag for to/from json() methods
	bool        m_decimal;
};

/** This template class extends the implementation of Basic_<ELEMTYPE> to support
	the toJSON() and fromJSON_() methods for read/floating point types.

	NOTES:
	1) All methods in this class are NOT thread Safe unless explicitly
	documented otherwise.
 */
template<class ELEMTYPE>
class BasicReal_ : public Cpl::Dm::Mp::Basic_<ELEMTYPE>
{
public:
	/// Constructor: Invalid MP
	BasicReal_( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, bool decimalFormat = true )
		:Basic_<ELEMTYPE>( myModelBase, staticInfo )
	{
	}

	/// Constructor: Valid MP (requires initial value)
	BasicReal_( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, ELEMTYPE initialValue, bool decimalFormat = true )
		:Basic_<ELEMTYPE>( myModelBase, staticInfo, initialValue )
	{
	}


public:
	/// Atomic increment
	virtual uint16_t increment( ELEMTYPE incSize = 1, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
	{
		Fxt::Point::PointCommon_::m_modelDatabase.lock_();
		uint16_t result = Basic_<ELEMTYPE>::write( Basic_<ELEMTYPE>::m_data + incSize, lockRequest );
		Fxt::Point::PointCommon_::m_modelDatabase.unlock_();
		return result;
	}

	/// Atomic decrement
	virtual uint16_t decrement( ELEMTYPE decSize = 1, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
	{
		Fxt::Point::PointCommon_::m_modelDatabase.lock_();
		uint16_t result = Basic_<ELEMTYPE>::write( Basic_<ELEMTYPE>::m_data - decSize, lockRequest );
		Fxt::Point::PointCommon_::m_modelDatabase.unlock_();
		return result;
	}


public:
	/// See Cpl::Dm::Point.  
	bool toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose = true ) noexcept
	{
		// Get a snapshot of the my data and state
		Fxt::Point::PointCommon_::m_modelDatabase.lock_();
		ELEMTYPE value = Basic_<ELEMTYPE>::m_data;
		uint16_t seqnum = Basic_<ELEMTYPE>::m_seqNum;
		int8_t   valid = Basic_<ELEMTYPE>::m_validState;
		bool     locked = Basic_<ELEMTYPE>::m_locked;
		Fxt::Point::PointCommon_::m_modelDatabase.unlock_();

		// Start the conversion
		JsonDocument& doc = Fxt::Point::PointCommon_::beginJSON( valid, locked, seqnum, verbose );

		// Construct the 'val' key/value pair (as a simple numeric)
		if ( Fxt::Point::PointCommon_::IS_VALID( valid ) )
		{
			doc["val"] = value;
		}

		// End the conversion
		Fxt::Point::PointCommon_::endJSON( dst, dstSize, truncated, verbose );
		return true;
	}

	/// See Cpl::Dm::Point.  
	bool fromJSON_( JsonVariant& src, Fxt::Point::Api::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
	{
		ELEMTYPE checkForError = src | (ELEMTYPE)2;
		ELEMTYPE newValue = src | (ELEMTYPE)1;
		if ( newValue <= (ELEMTYPE)1 && checkForError >= (ELEMTYPE)2 )
		{
			if ( errorMsg )
			{
				*errorMsg = "Invalid syntax for the 'val' key/value pair";
			}
			return false;
		}

		retSequenceNumber = Basic_<ELEMTYPE>::write( newValue, lockRequest );
		return true;
	}
};

};      // end namespaces
};
};
#endif  // end header latch
