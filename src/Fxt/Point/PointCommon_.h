#ifndef Fxt_Point_PointCommon_h_
#define Fxt_Point_PointCommon_h_
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
#include "Cpl/Memory/ContiguousAllocator.h"


/// Trace Section label for Point infrastructure
#define FXT_POINT_TRACE_SECT_     "Fxt::Point"

///
namespace Fxt {
///
namespace Point {


/** This concrete class provides common infrastructure for a Point.

    NOTE: The class dynamically allocates memory using a 'Contiguous
          allocator.  As such this memory is NOT freed in the destructor
          (due to the 'global free' nature of the Contiguous Allocator).
 */
class PointCommon_ : public Fxt::Point::Api
{
protected:
    /// Constructor
    PointCommon_( uint32_t pointId, const char* pointName  );

    /// Helper method to 'completed' the initialization after the Stateful memory has been allocated
    virtual void finishInit( bool isValid ) noexcept;

public:
    /// See Fxt::Point::Api
    uint32_t getId() const noexcept;
    
    /// See Fxt::Point::Api
    const char* getName() const noexcept;

    /// See Fxt::Point::Api
    void getMetadata( bool& isValid, bool& isLocked ) const noexcept;

    /// See Fxt::Point::Api
    void setInvalid( LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// See Fxt::Point::Api
    bool isNotValid( void ) const noexcept;

    /// See Fxt::Point::Api
    bool isLocked() const noexcept;

    /// See Fxt::Point::Api
    void setLockState( LockRequest_T lockRequest ) noexcept;

public:
    void* getStartOfStatefulMemory_() const noexcept;

protected:
    /// See Fxt::Point::Api
    bool read( void* dstData, size_t dstSize ) const noexcept;

    /// See Fxt::Point::Api
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
    static Api* create( Cpl::Memory::Allocator&             allocatorForPoints, 
                        uint32_t                            pointId, 
                        const char*                         pointName,
                        Cpl::Memory::ContiguousAllocator&   allocatorForPointStatefulData )
    {
        void* memPtr = allocatorForPoints.allocate( sizeof( T ) );
        if ( memPtr )
        {
            return new(memPtr) T( pointId, pointName, allocatorForPointStatefulData );
        }
        return nullptr;
    }

    /// Helper function to Update the MP's data from 'src'
    void updateFrom( const void* srcData, size_t srcSize, bool isNotValid, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept;

protected:
    /// Structure for meta-data
    struct Metadata_T
    {
        bool valid;     //!< The point's valid/not-valid state
        bool locked;    //!< The point's locked state
    };

    /// The Point's unique numeric ID
    uint32_t    m_id;

    /// The point's 'data' and meta-data that is dynamically allocated. The Meta-data is REQUIRED to be first elements in the allocate chunk of memory
    void*       m_state;

    /// The point's 'label. The constructor ensures that a null name is 'converted' to an empty string.
    const char* m_name;
};

};      // end namespaces
};
#endif  // end header latch
