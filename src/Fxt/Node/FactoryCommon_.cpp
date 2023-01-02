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
#include "Error.h"

///
using namespace Fxt::Node;

FactoryCommon_::FactoryCommon_()
{
    // Nothing needed yet...
    factoryDb.insert_( *this );
}

FactoryCommon_::~FactoryCommon_()
{
    // Nothing needed yet...
}

/////////////////////////////////
void FactoryCommon_::destroy( Api& nodeToDestroy ) noexcept
{
    delete &nodeToDestroy;
}

Api* FactoryCommon_::create( JsonVariant              nodeJsonObject,
                             Fxt::Point::DatabaseApi& dbForPoints,
                             Fxt::Type::Error&        nodeErrorCode ) noexcept
{
    // Get the chassis Array
    JsonArray chassisArray = nodeJsonObject["chassis"];
    size_t    numChassis   = chassisArray.size();
    if ( numChassis == 0 )
    {
        nodeErrorCode = fullErr( Err_T::PARSE_CHASSIS_ARRAY );
        return nullptr;
    }
    if ( numChassis > getMaxAllowedChassis() )
    {
        nodeErrorCode = fullErr( Err_T::MAX_CHASSIS_EXCEEDED );
        return nullptr;
    }

    // Validate the Node type
    const char* typeGuid = nodeJsonObject["type"];
    if ( typeGuid == nullptr || strcmp( typeGuid, getGuid() ) != 0 )
    {
        nodeErrorCode = fullErr( Err_T::NOT_ME );
        return nullptr;
    }

    // Create Node instance
    Api* node = createNode( numChassis, nodeJsonObject, dbForPoints, nodeErrorCode );
    if ( node == nullptr )
    {
        nodeErrorCode = fullErr( Err_T::NO_MEMORY_NODE );
        return nullptr;
    }
    if ( node->getErrorCode() != Fxt::Type::Error::SUCCESS() )
    {
        nodeErrorCode = fullErr( Err_T::NODE_CREATE_ERROR );
        destroy( *node );
        return nullptr;
    }

    // Create Chassis
    for ( uint8_t i=0; i < numChassis; i++ )
    {
        Fxt::Type::Error errorCode  = Fxt::Type::Error::SUCCESS();

        // Create the Chassis thread (and runnable obj) instance
        Fxt::Chassis::ServerApi* serverApi;
        Cpl::System::Thread*     chassisThread = node->createChassisThread( serverApi );
        if ( chassisThread == nullptr || serverApi == nullptr )
        {
            nodeErrorCode = fullErr( Err_T::FAILED_CREATE_CHASSIS_SERVER );
            destroy( *node );
            return nullptr;
        }

        // Create the Chassis
        Fxt::Type::Error   errorCode;
        JsonObject         chassisObj = chassisArray[i];
        Fxt::Chassis::Api* chassisPtr = Fxt::Chassis::Api::createChassisfromJSON( chassisObj,
                                                                                  *serverApi,
                                                                                  m_componentFactoryDb,
                                                                                  m_cardFactoryDb,
                                                                                  node->getGeneralAlloactor(),
                                                                                  node->getCardStatefulAlloactor(),
                                                                                  node->getHaStatefulAlloactor(),
                                                                                  m_pointFactoryDb,
                                                                                  dbForPoints,
                                                                                  errorCode );
        if ( chassisPtr == nullptr )
        {
            nodeErrorCode = fullErr( Err_T::FAILED_CREATE_CHASSIS );
            destroy( *node );
            return nullptr;
        }
        if ( errorCode != Fxt::Type::Error::SUCCESS() )
        {
            nodeErrorCode = fullErr( Err_T::CHASSIS_CREATE_ERROR );
            destroy( *node );
            return nullptr;
        }
        nodeErrorCode = node->add( *chassisPtr, *chassisThread );
        if ( nodeErrorCode != Fxt::Type::Error::SUCCESS() )
        {
            destroy( *node );
            return nullptr;
        }
    }


    // If I get here -->everything worked
    return node;
}

