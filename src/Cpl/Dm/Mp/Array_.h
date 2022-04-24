#ifndef Cpl_Dm_Mp_Array_h_
#define Cpl_Dm_Mp_Array_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */


#include "Cpl/Dm/ModelPointCommon_.h"

///
namespace Cpl {
///
namespace Dm {
///
namespace Mp {



/** This a mostly concrete class provides 'common' implementation for a Model
    Point who's data is a array of elements

 */
class ArrayBase_ : public Cpl::Dm::ModelPointCommon_
{
protected:
    /// Meta data for read/write/copy operations
    struct MetaData_T
    {
        uint8_t*  elemPtr;          //!< Pointer to the element in the array to read/write
        size_t    numElements;      //!< Number of element to read/write
        size_t    elemIndex;        //!< Starting array index
        size_t    elemSize;         //!< Size, in bytes, of an element
    };

protected:
    /// Number of elements in the array
    size_t  m_numElements;

    /// Size, in bytes, of an element
    size_t  m_elementSize;

public:
    /// Constructor: Invalid MP
    ArrayBase_( Cpl::Dm::ModelDatabase& myModelBase,
                const char*             symbolicName,
                void*                   myDataPtr,
                size_t                  numElements,
                size_t                  elementSize );


    /** Constructor.  Valid MP.  Requires an initial value.  If the 'initialValueSrcPtr'
        pointer is set to zero, then the entire array will be initialized to
        zero.   Note: The array that 'initialValueSrcPtr' points to ' MUST contain
        at least 'numElements' elements.
     */
    ArrayBase_( Cpl::Dm::ModelDatabase& myModelBase,
                const char*             symbolicName,
                void*                   myDataPtr,
                size_t                  numElements,
                size_t                  elementSize,
                void*                   initialValueSrcPtr );

protected:
    /** The caller can read a subset of array starting from the specified index
        in the Model Point's array.  Note: if srcIndex + dstNumElements exceeds
        the size of the MP's data then the read operation will be truncated.
    */
    virtual bool read( void* dstData, size_t dstNumElements, size_t srcIndex = 0, uint16_t* seqNumPtr = 0 ) const noexcept;

    /** The caller can write a subset of array starting from the specified index
        in the Model Point's array.  Note: if dstIndex + srcNumElements exceeds
        the size of the MP's data then the write operation will be truncated

        NOTE: The application/caller is responsible for what a 'partial write'
        means to the integrity of the MP's data.  WARNING: Think before
        doing a partial write!  For example, if the MP is in the invalid
        state and a partial write is done - then the MP's data/array is
        only partially initialized AND then MP is now in the valid
        state!
    */
    virtual uint16_t write( void* srcData, size_t srcNumElements, LockRequest_T lockRequest = eNO_REQUEST, size_t dstIndex = 0 ) noexcept;

    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    virtual uint16_t copyFrom( ArrayBase_& src, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

public:
    /// Returns the number of element in the array. This method IS thread safe.
    inline size_t getNumElements() const noexcept
    {
        return m_numElements;
    }

public:
    /// See Cpl::Dm::ModelPoint
    void copyDataTo_( void* dstData, size_t dstSize ) const noexcept;

    /// See Cpl::Dm::ModelPoint
    void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept;

    /// See Cpl::Dm::ModelPoint.  
    bool isDataEqual_( const void* otherData ) const noexcept;


    /// See Cpl::Dm::Point.  
    size_t getInternalDataSize_() const noexcept;

    /// See Cpl::Dm::ModelPoint.  
    bool importMetadata_( const void* srcDataStream, size_t& bytesConsumed ) noexcept;

    /// See Cpl::Dm::ModelPoint.  
    bool exportMetadata_( void* dstDataStream, size_t& bytesAdded ) const noexcept;
};

/** This template class extends the implementation of BasicArray<ELEMTYPE> to support
    the toJSON() and fromJSON_() methods for integer element types.

    NOTES:
    1) All methods in this class are NOT thread Safe unless explicitly
    documented otherwise.
*/
template<class ELEMTYPE>
class BasicIntegerArray : public BasicArray<ELEMTYPE>
{
protected:
    /// Flag for to/from json() methods
    bool        m_decimal;


public:
    /// Constructor: Invalid MP
    BasicIntegerArray( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, size_t numElements, bool decimalFormat = true )
        :BasicArray<ELEMTYPE>( myModelBase, staticInfo, numElements )
        , m_decimal( decimalFormat )
    {
    }


    /** Constructor.  Valid MP.  Requires an initial value.  If the 'srcData'
    pointer is set to zero, then the entire array will be initialized to
    zero.   Note: 'srcData' MUST contain at least 'numElements' elements.
    */
    BasicIntegerArray( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, size_t numElements, const ELEMTYPE* srcData, bool decimalFormat = true )
        :BasicArray<ELEMTYPE>( myModelBase, staticInfo, numElements, srcData )
        , m_decimal( decimalFormat )
    {
    }


public:
    /// See Cpl::Dm::Point.  
    bool toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose=true ) noexcept
    {
        // Lock the Model Point
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.lock_();
        uint16_t seqnum = BasicArray<ELEMTYPE>::m_seqNum;
        int8_t   valid  = BasicArray<ELEMTYPE>::m_validState;
        bool     locked = BasicArray<ELEMTYPE>::m_locked;

        // Start the conversion
        JsonDocument& doc = Cpl::Dm::ModelPointCommon_::beginJSON( valid, locked, seqnum, verbose );

        // Construct the 'val' array object
        if ( Cpl::Dm::ModelPointCommon_::IS_VALID( valid ) )
        {
            JsonObject obj = doc.createNestedObject( "val" );
            obj["start"] = 0;
            JsonArray arr = obj.createNestedArray( "elems" );
            for ( size_t i = 0; i < BasicArray<ELEMTYPE>::m_data.numElements; i++ )
            {
                if ( m_decimal )
                {
                    arr.add( BasicArray<ELEMTYPE>::m_data.elemPtr[i] );
                }
                else
                {
                    Cpl::Text::FString<20> s;
                    s.format( "0x%llX", (unsigned long long) BasicArray<ELEMTYPE>::m_data.elemPtr[i] );
                    arr.add( (char*) s.getString() );
                }
            }
        }

        // End the conversion
        Cpl::Dm::ModelPointCommon_::endJSON( dst, dstSize, truncated, verbose );

        // unlock the database
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.unlock_();
        return true;
    }

    /// See Cpl::Dm::Point.  
    bool fromJSON_( JsonVariant& src, Cpl::Dm::ModelPoint::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
    {
        // Check for object
        if ( src.is<JsonObject>() == false )
        {
            if ( errorMsg )
            {
                *errorMsg = "'val' key/value pair is NOT an JSON object";
            }
            return false;
        }

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
        size_t startIdx = src["start"] | 0;

        // Check for exceeding array limits
        size_t numElements = elems.size();
        if ( numElements > BasicArray<ELEMTYPE>::m_data.numElements )
        {
            if ( errorMsg )
            {
                errorMsg->format( "Number of array elements (%lu) exceeds the MP's element count (%lu)", src.size(), BasicArray<ELEMTYPE>::m_data.numElements );
            }
            return false;
        }

        // Attempt to parse the value key/value pair (as a simple numeric)
        ELEMTYPE* tempArray = (ELEMTYPE*) ModelDatabase::g_tempBuffer_;
        for ( size_t i = 0; i < numElements; i++ )
        {
            // Attempt to parse the value as simple numeric
            if ( m_decimal )
            {
                tempArray[i] = elems[i];
            }

            // Attempt to parse the value as HEX string
            else
            {
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
                tempArray[i] = (ELEMTYPE) value;
            }
        }

        // Update the Model Point 
        retSequenceNumber = BasicArray<ELEMTYPE>::write( tempArray, numElements, lockRequest, startIdx );
        return true;
    }
};

};      // end namespaces
};
};
#endif  // end header latch
