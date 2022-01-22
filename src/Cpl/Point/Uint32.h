#ifndef Cpl_Point_Uint32_h_
#define Cpl_Point_Uint32_h_
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


#include "Cpl/Point/Basic_.h"
#include "Cpl/Point/Identifier.h"
#include "Cpl/Memory/Allocator.h"

///
namespace Cpl {
///
namespace Point {


/** This class provides a concrete implementation for a Point who's data is a
    uint32_t.

    The toJSON()/fromJSON format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:"hex-value" }

        \endcode

 */
class Uint32 : public BasicInteger_<uint32_t>
{
public:
    /// Type safe Point Identifier
    class Id_T : public Identifier_T
    {
    public:
        constexpr Id_T() : Identifier_T() {}
        constexpr Id_T( uint32_t x ) : Identifier_T( x ) {}
    };


public:
    /// Returns the Point's Identifier
    inline Uint32::Id_T getId() const noexcept { return m_id; }

public:
    /** Constructor. Invalid Point.
     */
    Uint32( const Id_T myIdentifier ) : BasicInteger_<uint32_t>(), m_id( myIdentifier ) {}

    /// Constructor. Valid Point.  Requires an initial value
    Uint32( const Id_T myIdentifier, uint32_t initialValue ) : BasicInteger_<uint32_t>( initialValue ), m_id( myIdentifier ) {}

public:
    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept { return "Cpl::Point::Uint32"; }

public:
    /// Creates a concrete instance in the invalid state
    static Api* create( Cpl::Memory::Allocator& allocatorForPoints, uint32_t pointId ) { return new(allocatorForPoints.allocate( sizeof( Uint32 ) )) Uint32( pointId ); }

protected:
    /// The points numeric identifier
    Id_T m_id;
};



};      // end namespaces
};
#endif  // end header latch
