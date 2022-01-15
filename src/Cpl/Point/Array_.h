#ifndef Cpl_Point_Array_h_
#define Cpl_Point_Array_h_
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
#include "Cpl/Point/Database.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/FString.h"
#include <string.h>
#include <type_traits>


///
namespace Cpl {
///
namespace Point {

/** This template class provides the basic infrastructure for Array of of types.

    Template args :
        N        - Number of elements in the Array
        ELEMTYPE - Type of the array element
 */
template<int N, class ELEMTYPE>
class Array_ : public PointCommon_
{
protected:
    /// The Point's Internal Data container 
    typedef struct
    {
        ELEMTYPE* elemPtr;        //!< Pointer to the element in the array to read/write
        size_t    numElements;    //!< Number of element to read/write
        size_t    elemIndex;      //!< Starting array index
    } InternalData;

protected:
    /// Constructor. Invalid Point.
    Array_()
        : PointCommon_( false )
    {
        // Do not allow empty arrays
        static_assert(N > 0, "Cannot create zero element array");

        // Initialize the array to all zero - so as to have deterministic 'invalid' value 
        memset( m_array, 0, N * sizeof( ELEMTYPE ) );
    }

    /// Constructor. Valid Point.  
    Array_( const ELEMTYPE* initialSrcData )
        : PointCommon_( true )
    {
        // Do not allow empty arrays
        static_assert(N > 0, "Cannot create zero element array");

        // Zero the array if no data provide 
        if ( initialSrcData == 0 )
        {
            memset( m_array, 0, N * sizeof( ELEMTYPE ) );
        }

        // Initialize the array to the provided values
        else
        {
            memcpy( m_array, initialSrcData, N * sizeof( ELEMTYPE ) );
        }
    }


public:
    /** Type safe read.  The caller can read a subset of array starting from
         the specified index in the Model Point's array.
         Note: if srcIndex + dstNumElements exceeds the size of the Point's data
               then the read operation will be truncated.
     */
    virtual bool read( ELEMTYPE* dstData, size_t dstNumElements, size_t srcIndex = 0 ) const noexcept
    {
        InternalData dst ={ dstData, dstNumElements, srcIndex };
        return PointCommon_::read( &dst, sizeof( dst ) );
    }

    /// Returns the Point's value and its meta-data.  'dstData' has no meaning when the method returns false.
    virtual bool get( ELEMTYPE* dstData, size_t dstNumElements, bool& isValid, bool& isLocked, size_t srcIndex = 0 ) const noexcept
    {
        isValid  = m_valid;
        isLocked = m_locked;
        return read( dstData, dstNumElements, srcIndex );
    }

    /** Type safe write. The caller can write a subset of array starting from
        the specified index in the Model Point's array.
        Note: if dstIndex + srcNumElements exceeds the size of the Point's data
              then the write operation will be truncated

        The application/caller is responsible for what a 'partial write'
        means to the integrity of the Point's data.  WARNING: Think before
        doing a partial write!  For example, if the Point is in the invalid
        state and a partial write is done - then the Point's data/array is
        only partially initialized AND then Point is now in the valid
        state!
    */
    virtual void write( ELEMTYPE* srcData, size_t srcNumElements, LockRequest_T lockRequest = eNO_REQUEST, size_t dstIndex = 0 ) noexcept
    {
        InternalData src ={ srcData, srcNumElements, dstIndex };
        PointCommon_::write( &src, sizeof( src ), lockRequest );
    }

    /// See Cpl::Point::Api.  
    size_t getSize() const noexcept { return sizeof( ELEMTYPE ) * N; }

    /// Returns the number of elements in the Array
    size_t getNumElements() const noexcept { return N; }

    /// See Cpl::Point::Api. Sets the array memory to all zeros - for deterministic behavior of partial read/write methods
    void setInvalid( LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        PointCommon_::setInvalid( lockRequest );
        memset( m_array, 0, N * sizeof( ELEMTYPE ) );
    }


public:
    /// See Cpl::Point::Api
    void copyDataTo_( void* dstData, size_t dstSize ) const noexcept
    {
        CPL_SYSTEM_ASSERT( dstSize == sizeof( m_data ) );
        InternalData* dstInfo = (InternalData*)dstData;

        // Make sure we don't read past the m_data storage
        if ( dstInfo->elemIndex + dstInfo->numElements > N )
        {
            dstInfo->numElements = N - dstInfo->elemIndex;
        }

        // Copy the data to 'dst'
        memcpy( dstInfo->elemPtr, &(m_array[dstInfo->elemIndex]), dstInfo->numElements * sizeof( ELEMTYPE ) );
    }


    /// See Cpl::Point::Api
    void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
    {
        CPL_SYSTEM_ASSERT( srcSize == sizeof( m_data ) );
        InternalData* srcInfo = (InternalData*)srcData;

        // Make sure we don't write past the m_data storage
        if ( srcInfo->elemIndex + srcInfo->numElements > N )
        {
            srcInfo->numElements = N - srcInfo->elemIndex;
        }

        // Copy the data to 'src'
        memcpy( &(m_array[srcInfo->elemIndex]), srcInfo->elemPtr, srcInfo->numElements * sizeof( ELEMTYPE ) );
    }

protected:
    /// Array memory
    ELEMTYPE m_array[N];
};


/** This template class extends the implementation of Array_<ELEMTYPE> to support
    the toJSON() and fromJSON_() methods for integer element types.

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:{startIdx:nn, elems:[n,m,...]}" }
    \endcode

    Template args :
        N        - Number of elements in the Array
        ELEMTYPE - Type of the array element
 */
template<int N, class ELEMTYPE>
class IntegerArray_ : public Array_<N, ELEMTYPE>
{
protected:
    /// Constructor. Invalid Point.
    IntegerArray_() : Array_<N, ELEMTYPE>(){}

    /// Constructor. Valid Point.  
    IntegerArray_( const ELEMTYPE* initialSrcData ) :Array_<N, ELEMTYPE>( initialSrcData ){}

public:
    /// See Cpl::Point::Api.  
    bool toJSON_( JsonDocument& doc, bool verbose = true ) noexcept
    {
        JsonObject obj  = doc.createNestedObject( "val" );
        JsonArray arr   = obj.createNestedArray( "elems" );
        for ( size_t i = 0; i < N; i++ )
        {
            Cpl::Text::FString<20> s;
            s.format( "0x%llX", (unsigned long long)  Array_<N, ELEMTYPE>::m_array[i] );
            arr.add( (char*)s.getString() );
        }
        return true;
    }

    /// See Cpl::Point::Api.  
    bool fromJSON_( JsonVariant & src, Cpl::Point::Api::LockRequest_T lockRequest, Cpl::Text::String * errorMsg=0 ) noexcept
    {
        // Check for embedded array
        JsonArray elems = src["elems"];
        if ( elems.isNull() )
        {
            if ( errorMsg )
            {
                *errorMsg = "'val' key/value pair is missing the embedded 'elems' array";
            }
            return false;
        }

        // Get starting index (note: if not present a default of '0' will be returned)
        size_t startIdx = src["startIdx"] | 0;

        // Check for exceeding array limits
        size_t numElements = elems.size();
        if ( numElements > N )
        {
            if ( errorMsg )
            {
                errorMsg->format( "Number of array elements (%lu) exceeds the MP's element count (%lu)", src.size(), N );
            }
            return false;
        }

        // Attempt to parse the value key/value pair (as a simple numeric)
        ELEMTYPE* tempArray = (ELEMTYPE*)Database::g_tempBuffer_;
        for ( size_t i = 0; i < numElements; i++ )
        {
            // Attempt to parse the value as HEX string
            const char*        val = elems[i];
            unsigned long long value;
            if ( Cpl::Text::a2ull( value, val, 16 ) == false )
            {
                if ( errorMsg )
                {
                    *errorMsg = "Invalid syntax for the 'val' key/value pair";
                }
                return false;
            }
            tempArray[i] = (ELEMTYPE)value;
        }

        Array_<N, ELEMTYPE>::write( tempArray, numElements, lockRequest, startIdx );
        return true;
    }
};


/** This template class extends the implementation of Array_<ELEMTYPE> to support
    the toJSON() and fromJSON_() methods for floating-point/real-number element types.

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:{startIdx:nn, elems:[n,m,...]}" }
    \endcode

    Template args :
        N        - Number of elements in the Array
        ELEMTYPE - Type of the array element
 */
template<int N, class ELEMTYPE>
class RealArray_ : public Array_<N, ELEMTYPE>
{
protected:
    /// Constructor. Invalid Point.
    RealArray_() : Array_<N, ELEMTYPE>(){}

    /// Constructor. Valid Point.  
    RealArray_( const ELEMTYPE* initialSrcData ) :Array_<N, ELEMTYPE>( initialSrcData ){}

public:
    /// See Cpl::Point::Api.  
    bool toJSON_( JsonDocument& doc, bool verbose = true ) noexcept
    {
        JsonObject obj  = doc.createNestedObject( "val" );
        JsonArray arr   = obj.createNestedArray( "elems" );
        for ( size_t i = 0; i < N; i++ )
        {
            arr.add( Array_<N, ELEMTYPE>::m_array[i] );
        }
        return true;
    }

    /// See Cpl::Point::Api.  
    bool fromJSON_( JsonVariant & src, Cpl::Point::Api::LockRequest_T lockRequest, Cpl::Text::String * errorMsg=0 ) noexcept
    {
        // Check for embedded array
        JsonArray elems = src["elems"];
        if ( elems.isNull() )
        {
            if ( errorMsg )
            {
                *errorMsg = "'val' key/value pair is missing the embedded 'elems' array";
            }
            return false;
        }

        // Get starting index (note: if not present a default of '0' will be returned)
        size_t startIdx = src["startIdx"] | 0;

        // Check for exceeding array limits
        size_t numElements = elems.size();
        if ( numElements > N )
        {
            if ( errorMsg )
            {
                errorMsg->format( "Number of array elements (%lu) exceeds the MP's element count (%lu)", src.size(), N );
            }
            return false;
        }

        // Attempt to parse the value key/value pair (as a simple numeric)
        ELEMTYPE* tempArray = (ELEMTYPE*)Database::g_tempBuffer_;
        for ( size_t i = 0; i < numElements; i++ )
        {
            ELEMTYPE checkForError = elems[i] | (ELEMTYPE)2;
            ELEMTYPE newValue      = elems[i] | (ELEMTYPE)1;
            if ( newValue <= (ELEMTYPE)1 && checkForError >= (ELEMTYPE)2 )
            {
                if ( errorMsg )
                {
                    *errorMsg = "Invalid syntax for the 'val' key/value pair";
                }
                return false;
            }
            tempArray[i] = newValue[i];
        }

        Array_<N, ELEMTYPE>::write( tempArray, numElements, lockRequest, startIdx );
        return true;
    }
};


};      // end namespaces
};
#endif  // end header latch
