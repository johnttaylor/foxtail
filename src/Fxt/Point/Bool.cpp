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


#include "Bool.h"

///
using namespace Fxt::Point;

///////////////////////////////////////////////////////////////////////////////
Bool::Bool( const Id_T myIdentifier )
    : Basic_<bool>()
    , m_id( myIdentifier )
{
}

/// Constructor. Valid Point.  Requires an initial value
Bool::Bool( const Id_T myIdentifier, bool initialValue )
    : Basic_<bool>( initialValue )
    , m_id( myIdentifier )
{
}


///////////////////////////////////////////////////////////////////////////////
bool Bool::write( bool newValue, Fxt::Point::Api::LockRequest_T lockRequest ) noexcept
{
    Fxt::Point::PointCommon_::write( &newValue, sizeof( bool ), lockRequest );
    return m_data;
}

const char* Bool::getTypeAsText() const noexcept
{
    return "Fxt::Point::Bool";
}

bool Bool::toJSON_( JsonDocument& doc, bool verbose ) noexcept
{
    // Construct the 'val' key/value pair (as a HEX string)
    doc["val"] = m_data;
    return true;
}

bool Bool::fromJSON_( JsonVariant& src, Fxt::Point::Api::LockRequest_T lockRequest, Cpl::Text::String* errorMsg ) noexcept
{
    // Attempt to parse the value key/value pair
    bool checkForError = src | false;
    bool newValue      = src | true;
    if ( newValue == true && checkForError == false )
    {
        if ( errorMsg )
        {
            *errorMsg = "Invalid syntax for the 'val' key/value pair";
        }
        return false;
    }

    write( newValue, lockRequest );
    return true;
}
