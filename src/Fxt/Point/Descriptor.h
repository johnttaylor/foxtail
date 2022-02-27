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
#include "Cpl/Point/Info.h"

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
    Descriptor( const char* symbolicName, uint32_t localId, CreateFunc_T createFunction, SetterApi* setter=nullptr )
        : Cpl::Container::KeyUinteger32_T( localId )
        , m_createMethod( createFunction )
        , m_initialValue( setter )
        , m_pointId( 0 )
        , m_pointInfo( { nullptr, symbolicName } )
    {
    }

    /// Default constructor.  If use, then the configure() method MUST be called prior any other methods being called
    Descriptor()
        : Cpl::Container::KeyUinteger32_T( 0 )
        , m_createMethod( nullptr )
        , m_initialValue( nullptr )
        , m_pointId( 0 )
        , m_pointInfo( { nullptr, nullptr } )
    {
    }

    /// Configure the Descriptor after it has been constructed using the default constructor
    void configure( const char* symbolicName, uint32_t localId, CreateFunc_T createFunction, SetterApi* setter=nullptr ) noexcept
    {
        m_keyData                = localId;
        m_createMethod           = createFunction;
        m_initialValue           = setter;
        m_pointInfo.symbolicName = symbolicName;
    }

public:
    /** This method is used to create Point defined by the descriptor.  The
        method returns true when successful; else false (e.g. insufficient memory)
        is returned.
     */
    bool createPoint( Cpl::Memory::Allocator& allocatorForPoints, uint32_t pointId )
    {
        m_pointInfo.pointInstance = (m_createMethod) (allocatorForPoints, pointId);
        m_pointId                 = pointId;
        return m_pointInfo.pointInstance != nullptr;
    }

public:
    /// Data accessor
    inline uint32_t getLocalId() { return m_keyData; }
    
    /// Data accessor.  
    inline const char* getSymbolicName() { return m_pointInfo.symbolicName; }

    /// Data accessor.  If null, then no Point has been allocated
    inline Cpl::Point::Api* getPoint() { return m_pointInfo.pointInstance; }

    /// Data accessor. If getPoint() returns zero/nullptr then this value has no meaning
    inline Cpl::Point::Identifier_T getPointId() { return m_pointId; }

    /// Data accessor.  If null, then the Point has no associated Setter
    inline SetterApi* getSetter() { return m_initialValue; }

    /// Data accessor
    inline Cpl::Point::Info_T& getPointInfo() { return m_pointInfo; }

public:
    ///  API from DictItem
    const Cpl::Container::Key& getKey() const noexcept { return *this; }

protected:
    /// 'Factory' method to create the point of the correct type
    CreateFunc_T                m_createMethod;

    /// Setter associated with the Point.  If no setter is needed for the point the value will be zero/nullptr
    SetterApi*                  m_initialValue;  

    /// The runtime Point identifier.  If m_pointInfo.pointInstance is zero/nullptr - then the is method has no meaning
    uint32_t                    m_pointId;
    
    /// Contains the pointer to the Point (is zero until created) and Points symbolic name
    Cpl::Point::Info_T          m_pointInfo;
};


};      // end namespaces
};
#endif  // end header latch
