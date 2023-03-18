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
#include "Fxt/Chassis/Api.h"
#include "Cpl/System/Thread.h"
#include "Error.h"

///
using namespace Fxt::Node;

Fxt::Point::FactoryDatabase         Fxt::Node::FactoryCommon_::g_pointFactoryDb("ignoreMe-invoke-staticConstructor");
Fxt::Card::FactoryDatabase          Fxt::Node::FactoryCommon_::g_cardFactoryDb( "ignoreMe-invoke-staticConstructor" );
Fxt::Component::FactoryDatabase     Fxt::Node::FactoryCommon_::g_componentFactoryDb( "ignoreMe-invoke-staticConstructor" );

/////////////////////////////////
FactoryCommon_::FactoryCommon_()
{
    // Nothing needed yet...
}

FactoryCommon_::~FactoryCommon_()
{
    // Nothing needed yet...
}

//////////////////////////////////////////////////
static Api* theOne_;

Api* Api::getNode() noexcept
{
    return theOne_;
}

/////////////////////////////////
void FactoryCommon_::destroy( Api& nodeToDestroy ) noexcept
{
    delete &nodeToDestroy;
    theOne_ = nullptr;
}

Api* FactoryCommon_::createFromJSON( JsonVariant&             nodeJsonObject,
                                     Fxt::Point::DatabaseApi& dbForPoints,
                                     Fxt::Type::Error&        nodeErrorCode ) noexcept
{
    // Get the chassis Array
    JsonArray chassisArray = nodeJsonObject["chassis"];
    uint8_t   numChassis   = (uint8_t) chassisArray.size();
    if ( numChassis == 0 )
    {
        nodeErrorCode = fullErr( Err_T::PARSE_CHASSIS_ARRAY );
        nodeErrorCode.logIt();
        return nullptr;
    }
    if ( numChassis > getMaxAllowedChassis() )
    {
        nodeErrorCode = fullErr( Err_T::MAX_CHASSIS_EXCEEDED );
        nodeErrorCode.logIt();
        return nullptr;
    }

    // Validate the Node type
    const char* typeGuid = nodeJsonObject["type"];
    if ( typeGuid == nullptr || strcmp( typeGuid, getGuid() ) != 0 )
    {
        nodeErrorCode = fullErr( Err_T::NOT_ME );
        nodeErrorCode.logIt();
        return nullptr;
    }

    // Create Node instance
    Api* node = createNode( numChassis, dbForPoints, nodeJsonObject, nodeErrorCode );
    if ( node == nullptr )
    {
        nodeErrorCode = fullErr( Err_T::NO_MEMORY_NODE );
        nodeErrorCode.logIt();
        return nullptr;
    }
    if ( node->getErrorCode() != Fxt::Type::Error::SUCCESS() )
    {
        nodeErrorCode = fullErr( Err_T::NODE_CREATE_ERROR );
        nodeErrorCode.logIt();
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
            nodeErrorCode.logIt();
            destroy( *node );
            return nullptr;
        }

        // Create the Chassis
        JsonObject         chassisObj = chassisArray[i];
        Fxt::Chassis::Api* chassisPtr = Fxt::Chassis::Api::createChassisfromJSON( chassisObj,
                                                                                  *serverApi,
                                                                                  g_componentFactoryDb,
                                                                                  g_cardFactoryDb,
                                                                                  node->getGeneralAlloactor(),
                                                                                  node->getCardStatefulAlloactor(),
                                                                                  node->getHaStatefulAlloactor(),
                                                                                  g_pointFactoryDb,
                                                                                  dbForPoints,
                                                                                  errorCode );
        if ( chassisPtr == nullptr )
        {
            nodeErrorCode = fullErr( Err_T::FAILED_CREATE_CHASSIS );
            nodeErrorCode.logIt();
            destroy( *node );
            return nullptr;
        }
        if ( errorCode != Fxt::Type::Error::SUCCESS() )
        {
            nodeErrorCode = fullErr( Err_T::CHASSIS_CREATE_ERROR );
            nodeErrorCode.logIt();
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
    theOne_ = node;
    return node;
}

