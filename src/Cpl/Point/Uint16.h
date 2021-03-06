#ifndef Cpl_Point_Uint16_h_
#define Cpl_Point_Uint16_h_
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
    uint16_t.

    The toJSON()/fromJSON format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:"hex-value" }

        \endcode

 */
class Uint16 : public BasicInteger_<uint16_t>
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
    inline Uint16::Id_T getId() const noexcept { return m_id; }

public:
    /** Constructor. Invalid Point.
     */
    Uint16( const Id_T myIdentifier ) : BasicInteger_<uint16_t>(), m_id( myIdentifier ) {}

    /// Constructor. Valid Point.  Requires an initial value
    Uint16( const Id_T myIdentifier, uint16_t initialValue ) : BasicInteger_<uint16_t>( initialValue ), m_id( myIdentifier ) {}

public:
    /// Pull in overloaded methods from base class
    using BasicInteger_<uint16_t>::write;

    /// Updates the MP's data from 'src'
    virtual void write( Uint16& src, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST ) noexcept
    {
        if ( src.isNotValid() )
        {
            setInvalid();
        }
        else
        {
            BasicInteger_<uint16_t>::write( src.m_data, lockRequest );
        }
    }


public:
    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept { return "Cpl::Point::Uint16"; }

    ///  See Cpl::Point::Api
    size_t getTotalSize() const noexcept { return sizeof( Uint16 ); }


public:
    /// Creates a concrete instance in the invalid state
    static Api* create( Cpl::Memory::Allocator& allocatorForPoints, uint32_t pointId ) { return PointCommon_::create<Uint16>( allocatorForPoints, pointId ); }

protected:
    /// The points numeric identifier
    Id_T m_id;
};



};      // end namespaces
};
#endif  // end header latch
