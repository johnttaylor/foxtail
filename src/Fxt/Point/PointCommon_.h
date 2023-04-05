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
#include "Fxt/Point/DatabaseApi.h"
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
    PointCommon_( DatabaseApi&                      db, 
                  uint32_t                          pointId, 
                  size_t                            stateSize, 
                  Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData,
                  Api*                              setterPoint );

public:
    /// See Fxt::Point::Api
    uint32_t getId() const noexcept;
    
    /// See Fxt::Point::Api
    void getMetadata( bool& isValid, bool& isLocked ) const noexcept;

    /// See Fxt::Point::Api.  
    size_t getStatefulMemorySize() const noexcept;

    /// See Fxt::Point::Api
    void setInvalid( LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// See Fxt::Point::Api
    bool isNotValid( void ) const noexcept;

    /// See Fxt::Point::Api
    bool isLocked() const noexcept;

    /// See Fxt::Point::Api
    void setLockState( LockRequest_T lockRequest ) noexcept;

public:
    /// See Fxt::Point::Api
    bool hasSetter() const noexcept;

public:
    /// See Fxt::Point::Api
    void* getStartOfStatefulMemory_() const noexcept;

    /// See Fxt::Point::Api
    void updateFrom_( const void*                    srcData,
                      size_t                         srcSize,
                      bool                           srcNotValid,
                      Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept;

protected:
    /// See Fxt::Point::Api
    bool readData( void* dstData, size_t dstSize ) const noexcept;

    /// See Fxt::Point::Api
    void writeData( const void* srcData, size_t srcSize, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

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
     */
    virtual bool testAndUpdateLock( LockRequest_T lockRequest ) noexcept;

public:
    /// Structure for meta-data
    struct Metadata_T
    {
        bool  valid;        //!< The point's valid/not-valid state
        bool  locked;       //!< The point's locked state
    };

protected:
    /// The Point's unique numeric ID
    uint32_t    m_id;

    /// The point's 'data' and meta-data that is dynamically allocated. The Meta-data is REQUIRED to be first elements in the allocate chunk of memory
    void*       m_state;

    /// Number of bytes in of the data pointed to by m_state
    size_t      m_stateSize;

    /// Optional reference to the Point's internal setter (aka another Point instance)
    Api*        m_setter;

    
};

};      // end namespaces
};
#endif  // end header latch
