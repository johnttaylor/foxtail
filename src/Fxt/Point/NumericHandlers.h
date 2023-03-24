#ifndef Fxt_Point_NumericHandlers_h_
#define Fxt_Point_NumericHandlers_h_
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


#include "Fxt/Point/Api.h"

///
namespace Fxt {
///
namespace Point {


/** This concrete class provide a set of methods that allow the application
    to look-up attributes and generic read/write access method to point based on
    a Point's GUID.
 */
class NumericHandlers
{
public:
    /// Function signature for writing a Integer Point value
    typedef void (*WriteIntegerFunc_T)(Fxt::Point::Api* genericPt, uint64_t value);

    /// Function signature for reading a Integer Point value. Returns false if the Point is invalid
    typedef bool (*ReadIntegerFunc_T)(Fxt::Point::Api* genericPt, uint64_t& dstValue);

    /// Point attributes
    struct IntegerAttributes_T
    {
        WriteIntegerFunc_T  writeFunc;      //!< Generic write function
        ReadIntegerFunc_T   readFunc;       //!< Generic read function
        uint8_t             numBits;        //!< Number of bits in the integer
        bool                isSigned;       //!< Set to true if the integer is a signed integer
    };

public:
    /// Function signature for writing a floating Point value
    typedef void (*WriteRealFunc_T)(Fxt::Point::Api* genericPt, double value);

    /// Function signature for reading a floating Point value. Returns false if the Point is invalid
    typedef bool (*ReadRealFunc_T)(Fxt::Point::Api* genericPt, double& dstValue);

    /// Point attributes
    struct FloatAttributes_T
    {
        WriteRealFunc_T  writeFunc;      //!< Generic write function
        ReadRealFunc_T   readFunc;       //!< Generic read function
        uint8_t          numBits;        //!< Number of bits in the floating poitn value
    };

public:
    /** This method returns a pointer to a constant instance of Integer Attributes 
        for the specified 'typeGuid'.  If the 'typeGuid' is not a integer type,
        then nullptr is returned
     */
    static const IntegerAttributes_T* getIntegerPointAttributes( const char* typeGuid );

    /** This method returns a pointer to a constant instance of Float Attributes
        for the specified 'typeGuid'.  If the 'typeGuid' is not a floating type,
        then nullptr is returned
     */
    static const FloatAttributes_T* getFloatPointAttributes( const char* typeGuid );

};



};      // end namespaces
};
#endif  // end header latch
