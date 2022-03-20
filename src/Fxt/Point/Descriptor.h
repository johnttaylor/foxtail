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

#include "Fxt/Point/Api.h"
#include "Fxt/Point/SetterApi.h"
#include "Cpl/Memory/ContiguousAllocator.h"

///
namespace Fxt {
///
namespace Point {


/** This concrete class provides a mapping of 'User facing point ID' of a Point
    to the its actual runtime instance pointer.  An instance also contains
    additional information such as:
        - A Factory function to create the point
        - A SetterApi instance if the Point is an 'Auto Data' Point

    This class is NOT thread-safe.
 */
class Descriptor 
{
public:
    /// Define the function signature for creating a concrete point
    typedef Fxt::Point::Api* (*CreateFunc_T)( Cpl::Memory::Allocator& allocatorForPoints, uint32_t pointId, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData );

public:
    /// Constructor
    Descriptor( const char* symbolicName, uint32_t pointId, CreateFunc_T createFunction, SetterApi* setter=nullptr )
        : m_createMethod( createFunction )
        , m_initialValue( setter )
        , m_pointId( 0 )
        , m_pointInstance( nullptr )
    {
    }

    /// Default constructor.  If use, then the configure() method MUST be called prior any other methods being called
    Descriptor()
        : m_createMethod( nullptr )
        , m_initialValue( nullptr )
        , m_pointId( 0 )
        , m_pointInstance( nullptr )
    {
    }

    /// Configure the Descriptor after it has been constructed using the default constructor
    void configure( CreateFunc_T createFunction, SetterApi* setter=nullptr ) noexcept
    {
        m_createMethod = createFunction;
        m_initialValue = setter;
    }

public:
    /** This method is used to create Point defined by the descriptor.  The
        method returns the point instance when successful; else nullptr (e.g. 
        insufficient memory) is returned.
     */
    Fxt::Point::Api* createPoint( Cpl::Memory::Allocator& allocatorForPoints, uint32_t pointId, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData )
    {
        m_pointInstance = (m_createMethod) (allocatorForPoints, pointId, allocatorForPointStatefulData );
        return m_pointInstance;
    }

public:
    /// Data accessor
    inline Fxt::Point::Api* getPointInstance() { return m_pointInstance; }

    /// Data accessor. If getPoint() returns zero/nullptr then this value has no meaning
    inline uint32_t         getPointId() { return m_pointId; }

    /// Data accessor.  
    inline const char*      getSymbolicName() { return m_pointInstance? m_pointInstance->getName(): ""; }

    /// Data accessor.  If null, then the Point has no associated Setter
    inline SetterApi*       getSetter() { return m_initialValue; }



protected:
    /// 'Factory' method to create the point of the correct type
    CreateFunc_T                m_createMethod;

    /// Setter associated with the Point.  If no setter is needed for the point the value will be zero/nullptr
    SetterApi*                  m_initialValue;  

    /// The runtime Point identifier.  If m_pointInstance is zero/nullptr - then this field has no meaning
    uint32_t                    m_pointId;
    
    /// Contains the pointer to the Point (is zero until created) 
    Fxt::Point::Api*            m_pointInstance;
};


};      // end namespaces
};
#endif  // end header latch
