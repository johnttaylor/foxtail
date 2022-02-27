#ifndef Fxt_Point_Database_h_
#define Fxt_Point_Database_h_
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

#include "Cpl/Point/Database.h"


///
namespace Fxt {
///
namespace Point {


/** This concrete class extends the Cpl::Point::Database implementation to
    provide an auto-incremented Point ID that can be used to add points
    to the database.  Note: The original intent of the Point ID was to allow
    type safety for statically created points.  Since Foxtail always dynamically
    creates its points - it is easier to allow the database to 'assigned' the
    concrete Point ID values.
  */
class Database : public Cpl::Point::Database
{
public:
    /// Constructor
    Database( size_t maxNumPoints ) noexcept;

public:
    /// See Cpl::Point::DatabaseApi
    bool add( const Cpl::Point::Identifier_T numericId, Cpl::Point::Info_T& pointInfo ) noexcept;

    /** Returns the 'next' Point Identifier for when creating a Point and
        adding it to the database
     */
    virtual const Cpl::Point::Identifier_T getNextAvailablePointId() const noexcept;

protected:
    /// Next point ID
    uint32_t m_nextPointId;
};


};      // end namespaces
};
#endif  // end header latch
