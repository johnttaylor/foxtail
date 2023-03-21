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


#include "Factory.h"
#include "Drivers.h"

#define FXT_MY_APP_POINT_FACTORY_DB             Fxt::Node::FactoryCommon_::g_pointFactoryDb
#define FXT_MY_APP_CARD_FACTORY_DB              Fxt::Node::FactoryCommon_::g_cardFactoryDb
#define FXT_MY_APP_COMPONENT_FACTORY_DB         Fxt::Node::FactoryCommon_::g_componentFactoryDb

// Supported Point types and Components
#include "Fxt/Point/PopulateFactoryDb.h"
#include "Fxt/Component/Digital/PopulateFactoryDb.h"
#include "Fxt/Component/Analog/PopulateFactoryDb.h"

// Supported Cards
#include "Fxt/Card/Gpio/RP2040/Dio30Factory.h"
static Fxt::Card::Gpio::RP2040::Dio30Factory            dio30Factory_( FXT_MY_APP_CARD_FACTORY_DB );
#include "Fxt/Card/Sensor/I2C/RHTemperatureFactory.h"
static Fxt::Card::Sensor::I2C::RHTemperatureFactory     rhtempFactory_( FXT_MY_APP_CARD_FACTORY_DB, g_driverThreadMailboxPtr );
#include "Fxt/Card/Mock/PopulateFactoryDb.h"


//////////////////////////////////////////
// Note: It is IMPORTANT to have at least ONE function that gets 'referenced' - 
//       so that this object file gets linked.  The above population of the 
//       Factory databases is 'insufficient', i.e. no explicit reference(s) to 
//       the factory instance.

using namespace Fxt::Node::SBC::PiPicoDemo;

/// Constructor
Factory::Factory( size_t                              sizeGeneralHeap,
                  size_t                              sizeCardStatefulHeap,
                  size_t                              sizeHaStatefulHeap,
                  Cpl::System::SharedEventHandlerApi* eventHandler )
    : FactoryCommon_()
    , m_eventHandler( eventHandler )
    , m_sizeGeneralHeap( sizeGeneralHeap )
    , m_sizeCardStatefulHeap( sizeCardStatefulHeap )
    , m_sizeHaStatefulHeap( sizeHaStatefulHeap )
{
}

Fxt::Node::Api* Factory::createNode( uint8_t                    numChassis,
                                     Fxt::Point::DatabaseApi&   pointDb,
                                     JsonVariant                nodeJsonObject,
                                     Fxt::Type::Error&          nodeErrorCode ) noexcept
{
    Api* node = new (std::nothrow) Node( pointDb,
                                         m_sizeGeneralHeap,
                                         m_sizeCardStatefulHeap,
                                         m_sizeHaStatefulHeap,
                                         m_eventHandler );
    if ( node == nullptr )
    {
        nodeErrorCode = Fxt::Node::fullErr( Fxt::Node::Err_T::NO_MEMORY_NODE );
        nodeErrorCode.logIt();
    }

    return node;
}