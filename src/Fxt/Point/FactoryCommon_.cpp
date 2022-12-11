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


#include "FactoryCommon_.h"
#include "Cpl/Text/FString.h"
#include "Cpl/System/Trace.h"

#define SECT_   "Fxt::Point"

///
using namespace Fxt::Point;


///////////////////////////////////////////////////////////////////////////////
FactoryCommon_::FactoryCommon_( FactoryDatabaseApi& factoryDatabase )
{
    // Auto register with factory database
    factoryDatabase.insert_( *this );
}

Api* FactoryCommon_::create( JsonObject&                        pointObject,
                             Fxt::Type::Error&                  pointErrorCode,
                             Cpl::Memory::ContiguousAllocator&  generalAllocator,
                             Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                             Fxt::Point::DatabaseApi&           dbForPoints,
                             const char*                        pointIdKeyName ) noexcept
{
    // Get the reference to the typeCfg key/object
    JsonObject typeCfgJson = pointObject["typeCfg"].as<JsonObject>();

    // Is there a Setter/Initial-value
    Api* setter = nullptr;
    if ( pointObject["initial"].is<JsonObject>() == true )
    {
        // Create the Setter
        JsonObject setterJson  = pointObject["initial"].as<JsonObject>();
        setter                 = createRawPoint( setterJson,
                                                 pointErrorCode,
                                                 generalAllocator,
                                                 statefulDataAllocator,
                                                 dbForPoints,
                                                 typeCfgJson,
                                                 "id",
                                                 nullptr );
        if ( setter == nullptr )
        {
            return nullptr;
        }

        // Parse the Valid/Invalid state
        JsonVariant valid = setterJson["valid"];
        if ( valid.isNull() == false && valid.as<bool>() == false )
        {
            // Setter/initial value is the 'invalid-state'
            setter->setInvalid( Api::eLOCK );
        }

        // Parse the Setter point's value
        else
        {
            Cpl::Text::FString<128> errMsg;
            JsonVariant val = setterJson["val"];
            if ( !setter->fromJSON_( val, Api::eLOCK, &errMsg ) )
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Failed to parse 'val' object for the Setter point.id=%lu err=%s", setter->getId(), errMsg.getString()) );
                pointErrorCode = fullErr( Err_T::BAD_SETTER_VALUE );
                return nullptr;
            }
        }
    }

    // Create the Point
    Api* point =  createRawPoint( pointObject,
                                  pointErrorCode,
                                  generalAllocator,
                                  statefulDataAllocator,
                                  dbForPoints,
                                  typeCfgJson,
                                  pointIdKeyName,
                                  setter );
    if ( point == nullptr )
    {
        return nullptr;
    }

    
    // If I get there -->everything worked!
    pointErrorCode = Fxt::Type::Error::SUCCESS();
    return point;
}