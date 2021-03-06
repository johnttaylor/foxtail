#ifndef Cpl_Point_PointCommon_h_
#define Cpl_Point_PointCommon_h_
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


#include "Cpl/Point/Api.h"
#include "Cpl/Memory/Allocator.h"


///
namespace Cpl {
///
namespace Point {


/** This concrete class provides common infrastructure for a Point.
 */
class PointCommon_ : public Cpl::Point::Api
{
protected:
    /// Constructor
    PointCommon_( bool isValid = false );

public:
    /// See Cpl::Point::Api
    void setInvalid( LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// See Cpl::Point::Api
    bool isNotValid( void ) const noexcept;

    /// See Cpl::Point::Api
    bool isLocked() const noexcept;

    /// See Cpl::Point::Api
    void setLockState( LockRequest_T lockRequest ) noexcept;

    /// See Cpl::Point::Api
    void getMetadata( bool& isValid, bool& isLocked ) const noexcept;

protected:
    /// See Cpl::Point::Api
    bool read( void* dstData, size_t dstSize ) const noexcept;

    /// See Cpl::Point::Api
    void write( const void* srcData, size_t srcSize, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;


protected:
    /** Internal helper method that manages testing and updating the locked
        state.

        Rules:
        1) If 'lockRequest' is eNO_REQUEST, the method only returns true if
           the Point is in the unlocked state
        2) If 'lockRequest' is eLOCK, the method only returns if the Point is in
           the unlocked state.  In addition, when true is returned the Point is
           put into the locked state.
        3) If 'lockRequest' is eUNLOCK, the method always returns true and
           the Point is left in the unlocked state.

        This method is NOT thread safe.
     */
    virtual bool testAndUpdateLock( LockRequest_T lockRequest ) noexcept;


protected:
    /// Creates a concrete instance in the invalid state
    template <class T>
    static Api* create( Cpl::Memory::Allocator& allocatorForPoints, uint32_t pointId )
    {
        void* memPtr = allocatorForPoints.allocate( sizeof( T ) );
        if ( memPtr )
        {
            return new(memPtr) T( pointId );
        }
        return nullptr;
    }


protected:
    /// Locked state
    bool    m_locked;

    /// Internal valid/invalid state
    bool    m_valid;
};

};      // end namespaces
};
#endif  // end header latch
