#ifndef Cpl_Point_Int16_h_
#define Cpl_Point_Int16_h_
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
    int16_t.

    The toJSON()/fromJSON format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:"hex-value" }

        \endcode

 */
class Int16 : public BasicInteger_<int16_t>
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
    inline Int16::Id_T getId() const noexcept { return m_id; }

public:
    /** Constructor. Invalid Point.
     */
    Int16( const Id_T myIdentifier ) : BasicInteger_<int16_t>(), m_id( myIdentifier ) {}

    /// Constructor. Valid Point.  Requires an initial value
    Int16( const Id_T myIdentifier, int16_t initialValue ) : BasicInteger_<int16_t>( initialValue ), m_id( myIdentifier ) {}

public:
    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept { return "Cpl::Point::Int16"; }

public:
    /// Creates a concrete instance in the invalid state
    static Api* create( Cpl::Memory::Allocator& allocatorForPoints, uint32_t pointId ) { return new(allocatorForPoints.allocate( sizeof( Int16 ) )) Int16( pointId ); }

protected:
    /// The points numeric identifier
    Id_T m_id;
};



};      // end namespaces
};
#endif  // end header latch
