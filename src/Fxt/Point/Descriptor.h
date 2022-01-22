#ifndef Fxt_Point_Descriptor_h_
#define Fxt_Point_Descriptor_h_
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

#include "Cpl/Container/DictItem.h"
#include "Fxt/Point/SetterApi.h"
#include "Cpl/Memory/Allocator.h"
#include "Cpl/Point/Identifier.h"

///
namespace Fxt {
///
namespace Point {


/** This concrete class provides a mapping of 'User facing local ID' of a Point
    to the its actual runtime Point Identifier.  An instance also contains
    additional information such as:
        - A Factory function to create the point
        - A SetterApi instance if the Point is an 'Auto Data' Point

    Descriptors can be stored in Dictionary and looked-up by their User facing
    localId.

    This class is NOT thread-safe.
 */
class Descriptor : public Cpl::Container::DictItem, public Cpl::Container::KeyUinteger32_T
{
public:
    /// Define the function signature for creating a concrete point
    typedef Cpl::Point::Api* (*CreateFunc_T)(Cpl::Memory::Allocator& allocatorForPoints, uint32_t pointId);

public:
    /// Constructor
    Descriptor( uint32_t localId, CreateFunc_T createFunction, SetterApi* setter=nullptr )
        : Cpl::Container::KeyUinteger32_T( localId )
        , m_createMethod( createFunction )
        , m_initialValue( setter )
        , m_pointId( 0 )
        , m_point( nullptr )
    {
    }


public:
    /** This method is used to create Point defined by the descriptor.  The
        method returns true when successful; else false (e.g. insufficient memory)
        is returned.
     */
    bool createPoint( Cpl::Memory::Allocator& allocatorForPoints, uint32_t pointId )
    {
        m_point   = (m_createMethod) (allocatorForPoints, pointId);
        m_pointId = pointId;
        return m_point != nullptr;
    }

public:
    /// This function is used to identify a 'Null Descriptor'
    virtual bool isNullDescriptor() const noexcept { return false; }

public:
    /// Data accessor
    inline uint32_t getLocalId() { return m_keyData; }
    
    /// Data accessor.  If null, then no Point has been allocated
    inline Cpl::Point::Api* getPoint() { return m_point; }

    /// Data accessor. If getPoint() returns zero/nullptr then this value has no meaning
    inline Cpl::Point::Identifier_T getPointId() { return m_pointId; }

    /// Data accessor.  If null, then the Point has no associated Setter
    inline SetterApi* getSetter() { return m_initialValue; }


protected:
    /// 'Factory' method to create the point of the correct type
    CreateFunc_T                m_createMethod;

    /// Setter associated with the Point.  If no setter is needed for the point the value will be zero/nullptr
    SetterApi*                  m_initialValue;  

    /// The runtime Point identifier.  If m_point is zero/nullptr - then the is method has no meaning
    uint32_t                    m_pointId;
    
    /// Handle to the create Point.  If the point has not been created, the value is zero/nullptr
    Cpl::Point::Api*            m_point;
};


/** This concrete Descriptor Instance is a/the NULL descriptor (used when 
    creating variable length list of descriptors.  Only ONE instance of
    this class should every been created/needed.
 */
class NullDescriptor : public Descriptor
{
public:
    /// Constructor
    NullDescriptor() :Descriptor( 0, 0 ){}

public:
    /// See Descriptor
    bool isNullDescriptor() const noexcept { return true; }
};


};      // end namespaces
};
#endif  // end header latch
