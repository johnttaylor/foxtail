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
#include "Fxt/Point/DatabaseApi.h"
#include "Fxt/Point/SetterApi.h"
#include "Cpl/Memory/ContiguousAllocator.h"

///
namespace Fxt {
///
namespace Point {


/** This concrete class necessary information needed to construct a Point. This
    includes:
        - Symbolic Point name
        - The memory Allocator used when creating the Point
        - A Factory function to create the point (which defines/specifies the Point's type)
        - A optional SetterApi instance (used to set a Point's value from another Point's value)
 */
class Descriptor 
{
public:
    /// Define the function signature for creating a concrete point
    typedef Fxt::Point::Api* (*CreateFunc_T)(DatabaseApi& db, Cpl::Memory::Allocator& allocatorForPoints, uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData );

public:
    /// Constructor
    Descriptor( uint32_t pointId, const char* symbolicName, CreateFunc_T createFunction, SetterApi* setter=nullptr )
        : m_createMethod( createFunction )
        , m_initialValue( setter )
        , m_pointName( symbolicName )
        , m_pointId( pointId )
    {
    }

    /// Default constructor.  If used, then the configure() method MUST be called prior any other methods being called
    Descriptor()
        : m_createMethod( nullptr )
        , m_initialValue( nullptr )
        , m_pointName( nullptr )
        , m_pointId( 0 )
    {
    }

    /// Configure the Descriptor after it has been constructed using the default constructor
    void configure( uint32_t pointId, const char* symbolicName, CreateFunc_T createFunction, SetterApi* setter=nullptr ) noexcept
    {
        m_pointId      = pointId;
        m_pointName    = symbolicName;
        m_createMethod = createFunction;
        m_initialValue = setter;
    }

public:
    /** This method is used to create Point defined by the descriptor.  The
        method returns the point instance when successful; else nullptr (e.g. 
        insufficient memory) is returned.
     */
    Fxt::Point::Api* createPoint( DatabaseApi& db, Cpl::Memory::Allocator& allocatorForPoints, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData )
    {
        return (m_createMethod) (db, allocatorForPoints, m_pointId, m_pointName, allocatorForPointStatefulData);
    }

public:
    /// Data accessor. 
    inline uint32_t         getPointId() { return m_pointId; }

    /// Data accessor.  
    inline const char*      getSymbolicName() { return m_pointName; }

    /// Data accessor.  If null, then the Point has no associated Setter
    inline SetterApi*       getSetter() { return m_initialValue; }



protected:
    /// 'Factory' method to create the point of the correct type
    CreateFunc_T                m_createMethod;

    /// Setter associated with the Point.  If no setter is needed for the point the value will be zero/nullptr
    SetterApi*                  m_initialValue;  

    /// The Point name/label
    const char*                 m_pointName;

    /// The Point identifier.
    uint32_t                    m_pointId;
};


};      // end namespaces
};
#endif  // end header latch
