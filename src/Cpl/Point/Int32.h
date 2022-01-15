#ifndef Cpl_Point_Int32_h_
#define Cpl_Point_Int32_h_
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

///
namespace Cpl {
///
namespace Point {


/** This class provides a concrete implementation for a Point who's data is a
    int32_t.

    The toJSON()/fromJSON format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:"hex-value" }

        \endcode

 */
class Int32 : public BasicInteger_<int32_t>
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
    inline Int32::Id_T getId() const noexcept { return m_id; }

public:
    /** Constructor. Invalid Point.
     */
    Int32( const Id_T myIdentifier ) : BasicInteger_<int32_t>(), m_id( myIdentifier ) {}

    /// Constructor. Valid Point.  Requires an initial value
    Int32( const Id_T myIdentifier, int32_t initialValue ) : BasicInteger_<int32_t>( initialValue ), m_id( myIdentifier ) {}

public:
    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept { return "Cpl::Point::Int32"; }

protected:
    /// The points numeric identifier
    Id_T m_id;
};



};      // end namespaces
};
#endif  // end header latch