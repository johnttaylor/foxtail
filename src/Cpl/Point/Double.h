#ifndef Cpl_Point_Double_h_
#define Cpl_Point_Double_h_
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


#include "Cpl/Point/Identifier.h"
#include "Cpl/Point/Basic_.h"
#include "Cpl/Memory/Allocator.h"

///
namespace Cpl {
///
namespace Point {


/** This class provides a concrete implementation for a Point who's data is a
    double.

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:<numvalue> }

    \endcode
 */
class Double : public BasicReal_<double>
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
    inline Double::Id_T getId() const noexcept { return m_id; }

public:
    /// Constructor. Invalid MP. 
    Double( const Id_T myIdentifier ) :BasicReal_<double>(), m_id( myIdentifier ){}

    /// Constructor. Valid MP.  Requires an initial value
    Double( const Id_T myIdentifier, double initialValue ) :BasicReal_<double>( initialValue ), m_id( myIdentifier ){}


public:
    ///  See Cpl::Point::Api.
    const char* getTypeAsText() const noexcept { return "Cpl::Point:Double"; }

public:
    /// Creates a concrete instance in the invalid state
    static Api* create( Cpl::Memory::Allocator& allocatorForPoints, uint32_t pointId ) { return new(allocatorForPoints.allocate( sizeof( Double ) )) Double( pointId ); }

protected:
    /// The points numeric identifier
    Id_T m_id;
};



};      // end namespaces
};
#endif  // end header latch
