#ifndef Cpl_Point_Bool_h_
#define Cpl_Point_Bool_h_
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
    bool.

 	The toJSON()/fromJSON format is:
	\code
	
	{ name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:true|false }
	
	\endcode
	
	NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class Bool : public Basic_<bool>
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
    inline Bool::Id_T getId() const noexcept { return m_id; }

public:
    /** Constructor. Invalid Point.
     */
    Bool( const Id_T myIdentifier );

    /// Constructor. Valid Point.  Requires an initial value
    Bool( const Id_T myIdentifier, bool initialValue );

public:
    /// Type safe write. See Cpl::Point::Api
    virtual bool write( bool newValue, Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST ) noexcept;

    /// Short-cut for writing true
    inline bool set( Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST ) noexcept { return write( true, lockRequest ); }

    /// Short-cut for writing false
    inline bool clear( Cpl::Point::Api::LockRequest_T lockRequest = Cpl::Point::Api::eNO_REQUEST ) noexcept { return write( false, lockRequest ); }


public:
    /// See Cpl::Point::Api.  
    bool toJSON_( JsonDocument& doc, bool verbose = true ) noexcept;

    /// See Cpl::Point::Api.  
    bool fromJSON_( JsonVariant& src, Cpl::Point::Api::LockRequest_T lockRequest, Cpl::Text::String* errorMsg=0 ) noexcept;


public:
    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept;

protected:
    /// The points numeric identifier
    Id_T m_id;

};



};      // end namespaces
};
#endif  // end header latch
