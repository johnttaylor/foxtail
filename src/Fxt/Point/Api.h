#ifndef Fxt_Point_Api_h_
#define Fxt_Point_Api_h_
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

#include "colony_config.h"
#include "Cpl/Text/String.h"
#include "Cpl/Json/Arduino.h"
#include <stdint.h>
#include <stdlib.h>

///
namespace Fxt {
///
namespace Point {


/** This mostly abstract class defines the interface for a Point.  A
    Point contains an atomic (i.e. thread-safe), managed, type-safe 'chunk' of 
    data.  In addition to the data - a point has meta data and state. A Point 
    has the following features:

        o Read/write operations from/to a Point's data are atomic operations,
        o A Point has inherit valid/invalid state respect to its data.  The
          valid/invalid state is meta data and not associated with a specific
          value of its data
        o A Point can be locked.  Any write operations to a locked point will
          silently fail (i.e. its current value when locked will not be
          changed).
        o A Point can serialize/de-serialize to/from a JSON string.
        o A Point is type safe.  This means that child classes - per 'data
          type' - are required.  Each Point type can have method unique to its
          type.
        o Each point has unique (to the application) numeric identifier.  A
          'named' look-up is also supported.  Instances of the numeric identifier
          are 'managed types', i.e. each Point child class defines a unique
          leaf type for its identifier.
        o Points are NOT THREAD SAFE (see the namespace README.txt file for 
          details on the threading model).

    
 */
class Api
{
public:
    /// Options related to the Point's locked state
    enum LockRequest_T
    {
        eNO_REQUEST,            //!< No change in the Point's lock state is requested
        eLOCK,                  //!< Request to lock the Point.  If the Point is already lock - the request is ignored and the update operation silent fails
        eUNLOCK,                //!< Request to unlock the Point.  If the Point is already unlocked - the request is ignored and the update operation is completed
    };

public:
    /** This method returns the RAM size, in bytes, of the Point's data.
      */
    virtual size_t getSize() const noexcept = 0;

    /** This method returns a string identifier for the Point's data type.
        This value IS GUARANTEED to be unique (within an Application).  The
        format of the string is the Point's fully qualified namespace and
        class type as a string. For example, the for Fxt::Point::Uint32 Model
        the function would return "Fxt::Point::Uint32"

        Note: The type string can contain additional information, but adding
              a '-' character followed by the extra info.  For example:
              "Fxt::Point::Uint32-hex"
     */
    virtual const char* getTypeAsText() const noexcept = 0;

    /// This method returns the Point's meta-data
    virtual void getMetadata( bool& isValid, bool& isLocked ) const noexcept = 0;


public:
    /** This method sets the Point to the invalid state.

        Notes:
        1) Any write operation will set the Point's state to valid.
        2) If the Point is locked then the invalidate operation...
           a) If lockRequest == eNO_REQUEST, the operation silently fails, i.e.
              nothing is done. OR
           b) If lockRequest != eNO_REQUEST, the operation is performed and the
              the new lock state is applied

     */
    virtual void setInvalid( LockRequest_T lockRequest = eNO_REQUEST ) noexcept = 0;

    /// This method returns true when the Point data is invalid.
    virtual bool isNotValid() const noexcept = 0;

public:
    /** This method returns true if the Point is in the locked state.
        In the locked state - ALL WRITE/UPDATE OPERATIONS (except for changing
        the locked state) are silently ignored/dropped.
     */
    virtual bool isLocked() const noexcept = 0;

    /** This updates the lock state of the Point. Model Points support
        the concept of a client 'locking' the Point's data value.  When a Point's
        data has been locked - any attempted writes/updated operation to the
        Point will SILENTLY fail.  The Application uses the 'eUNLOCK' lock request
        to remove the locked state from the Point's data.
     */
    virtual void setLockState( LockRequest_T lockRequest ) noexcept = 0;

    /// Short hand for unconditionally removing the lock from the Point
    inline void removeLock() noexcept { setLockState( eUNLOCK ); }

    /// Short hand for putting the Point into the locked state
    inline void applyLock() noexcept { setLockState( eLOCK ); }


protected:
    /** This method copies the Point's content to the caller's memory buffer.
        The method returns the Point's valid state.

        If false is returned (i.e. the point is invalid), then contents of
        'dstData' is meaningless.

        Notes:
        1) The assumption is that Point's internal data and 'dstData' are
           of the same type.
        2) The data size of the 'dstSize' is ALWAYS honored when coping the
           data from the Point
     */
    virtual bool read( void* dstData, size_t dstSize ) const noexcept = 0;

    /** This method writes the caller Point memory buffer to the Point's
        internal data.

        Point supports the concept of a client 'locking' the Point's data
        value.  When a Point's data has been locked - all attempted writes to the
        Point - with lockRequest == eNO_REQUEST - will SILENTLY fail.  The
        Application uses the removeLock() method or the 'eUNLOCK' lock request
        to remove the locked state from the Point's data.  The application can
        also updated the value of Point when it is locked state by setting
        lockRequest == eLOCK.

        Notes:
        1) The assumption is that Point's internal data and 'srcData' are
           of the same type.
        2) The data size of the Model Points data instance is ALWAYS honored
           when coping the data from 'srcData'
     */
    virtual void write( const void*   srcData,
                        size_t        srcSize,
                        LockRequest_T lockRequest = eNO_REQUEST ) noexcept = 0;

 

public:
    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Fxt::Point namespace.  The Application should
        NEVER call this method.

        This method is used to unconditionally update the Point's data.

        This method is NOT Thread Safe.

        Notes:
        1) The assumption is that Point Data instance and 'src' are the
           of the same type.
        2) The internal data size of the Point instance is ALWAYS honored
           when coping the data from 'src'
        3) The Point's sequence number is not changed.
    */
    virtual void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept = 0;


    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Fxt::Point namespace.  The Application should
        NEVER call this method.

        This method is used update's the caller's 'Point Data' with the Model
        Point's data.

        This method is NOT Thread Safe.

        Notes:
        1) The assumption is that Point Data instance and 'dst' are the
           of the same type.
        2) The 'dstSize' of the destination ALWAYS honored when coping the
           Point's data to 'dst'
        3) The Point's sequence number is not changed.
    */
    virtual void copyDataTo_( void* dstData, size_t dstSize ) const noexcept = 0;


    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Fxt::Point namespace.  The Application should
        NEVER call this method.

        This method attempts to convert JSON object 'src' to its binary format
        and copies the result to the Point's internal data. The expected
        format of the JSON string is specific to the concrete leaf class.

        See Fxt::Point::ModelDatabaseApi::fromJSON() method for JSON format.
     */
    virtual bool fromJSON_( JsonVariant&        src,
                            LockRequest_T       lockRequest,
                            Cpl::Text::String*  errorMsg ) noexcept = 0;


    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Fxt::Point namespace.  The Application should
        NEVER call this method.

        This method converts the Points data to a JSON object.  If the point
        does not support serializing to a JSON object then false is returned;
        else true is returned.  This method is ONLY called if the Point is in
        the valid state.

        See Fxt::Point::ModelDatabaseApi::fromJSON() method for JSON format.
     */   
    virtual bool toJSON_( JsonDocument& doc, bool verbose = true ) noexcept = 0;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~Api() {}
};


};      // end namespaces
};
#endif  // end header latch
