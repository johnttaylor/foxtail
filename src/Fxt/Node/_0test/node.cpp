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

#include "Catch/catch.hpp"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Fxt/Node/Mock/Kestrel/Factory.h"
#include "Fxt/Card/Mock/AnalogIn8.h"
#include "Fxt/Card/Mock/Digital8.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/Math/real.h"
#include "Cpl/Text/FString.h"
#include <string.h>
#include "Fxt/Point/Float.h"
#include "Fxt/Point/Uint8.h"
#include "Fxt/Point/Bool.h"

#define SECT_   "_0test"

/// 
using namespace Fxt::Node;

static const char* NODE_DEFINITION = R"literalString(
{
  "name": "My Kestrel Node",
  "id": 1,
  "type": "d65ee614-dce4-43f0-af2c-830e3664ecaf",
  "chassis": [
    {
      "name": "My Chassis",
      "id": 1,
      "fer": 1000,
      "sharedPts": [
        {
          "id": 0,
          "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0",
          "typeName": "Fxt::Point::Bool",
          "initial": {
            "val": true,
            "id": 1
          },
          "name": "shared: 1"
        },
        {
          "id": 2,
          "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0",
          "typeName": "Fxt::Point::Bool",
          "initial": {
            "val": false,
            "id": 3
          },
          "name": "shared: 2"
        }
      ],
      "scanners": [
        {
          "name": "My Scanner",
          "id": 1,
          "scanRateMultiplier": 1,
          "cards": [
            {
              "name": "bob",
              "type": "1968f533-e323-4ae4-8493-9a572f3bd195",
              "typename": "Fxt::Card::HW::Mock::AnalogIn8",
              "slot": 22,
              "points": {
                "inputs": [
                  {
                    "channel": 2,
                    "id": 4,
                    "type": "708745fa-cef6-4364-abad-063a40f35cbc",
                    "typeName": "Fxt::Point::Float",
                    "initial": {
                      "valid": true,
                      "val": 1.2,
                      "id": 6
                    },
                    "name": "Motor Temperature",
                    "ioRegId": 5
                  },
                  {
                    "channel": 5,
                    "id": 7,
                    "type": "708745fa-cef6-4364-abad-063a40f35cbc",
                    "typeName": "Fxt::Point::Float",
                    "initial": {
                      "valid": true,
                      "val": 3.5,
                      "id": 9
                    },
                    "name": "Motor Voltage",
                    "ioRegId": 8
                  },
                  {
                    "channel": 6,
                    "id": 10,
                    "type": "708745fa-cef6-4364-abad-063a40f35cbc",
                    "typeName": "Fxt::Point::Float",
                    "initial": {
                      "valid": true,
                      "val": 0,
                      "id": 12
                    },
                    "name": "Motor Current",
                    "ioRegId": 11
                  }
                ]
              }
            },
            {
              "name": "My Digital8 Card",
              "type": "59d33888-62c7-45b2-a4d4-9dbc55914ed3",
              "typename": "Fxt::Card::HW::Mock::Digital8",
              "slot": 0,
              "points": {
                "inputs": [
                  {
                    "channel": 1,
                    "id": 13,
                    "type": "918cff9e-8007-4666-99ac-384b9624329c",
                    "typeName": "Fxt::Point::Uint8",
                    "initial": {
                      "valid": true,
                      "val": 128,
                      "id": 15
                    },
                    "name": "InputByte",
                    "ioRegId": 14
                  }
                ],
                "outputs": [
                  {
                    "channel": 1,
                    "id": 16,
                    "type": "918cff9e-8007-4666-99ac-384b9624329c",
                    "typeName": "Fxt::Point::Uint8",
                    "initial": {
                      "valid": true,
                      "val": 1,
                      "id": 18
                    },
                    "name": "OutputPt",
                    "ioRegId": 17
                  }
                ]
              }
            }
          ]
        }
      ],
      "executionSets": [
        {
          "name": "My Execution Set",
          "id": 1,
          "exeRateMultiplier": 1,
          "logicChains": [
            {
              "name": "my logic chain",
              "id": 1,
              "components": [
                {
                  "id": 100,
                  "name": "ByteSplitter #1",
                  "type": "8c55aa52-3bc8-4b8a-ad73-c434a0bbd4b4",
                  "typeName": "Fxt::Component::Digital::ByteSplitter",
                  "inputs": [
                    {
                      "type": "918cff9e-8007-4666-99ac-384b9624329c",
                      "typeName": "Fxt::Point::Uint8",
                      "idRef": 13,
                      "idRefName": "InputByte"
                    }
                  ],
                  "outputs": [
                    {
                      "bit": 4,
                      "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0",
                      "typeName": "Fxt::Point::Bool",
                      "idRef": 19,
                      "idRefName": "connect: bit4"
                    }
                  ]
                },
                {
                  "id": 101,
                  "name": "AND Gate#1",
                  "type": "e62e395c-d27a-4821-bba9-aa1e6de42a05",
                  "typeName": "Fxt::Component::Digital::And16Gate",
                  "inputs": [
                    {
                      "name": "Signal#1",
                      "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0",
                      "typeName": "Fxt::Point::Bool",
                      "idRef": 19,
                      "idRefName": "connect: bit4"
                    },
                    {
                      "name": "Signal#2",
                      "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0",
                      "typeName": "Fxt::Point::Bool",
                      "idRef": 21,
                      "idRefName": "auto: true"
                    }
                  ],
                  "outputs": [
                    {
                      "name": "out",
                      "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0",
                      "typeName": "Fxt::Point::Bool",
                      "idRef": 20,
                      "idRefName": "Output AndGate"
                    }
                  ]
                }
              ],
              "connectionPts": [
                {
                  "id": 19,
                  "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0",
                  "typeName": "Fxt::Point::Bool",
                  "name": "connect: bit4"
                },
                {
                  "id": 20,
                  "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0",
                  "typeName": "Fxt::Point::Bool",
                  "name": "Output AndGate"
                }
              ],
              "autoPts": [
                {
                  "id": 21,
                  "type": "f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0",
                  "typeName": "Fxt::Point::Bool",
                  "initial": {
                    "val": true,
                    "id": 22
                  },
                  "name": "auto: true"
                }
              ]
            }
          ]
        }
      ]
    }
  ]
}
)literalString";


#define FXT_PT_SHARED_1             0
#define FXT_PT_SHARED_2             2
#define FXT_PT_MOTOR_TEMPERATURE    4
#define FXT_PT_MOTOR_VOLTAGE        7
#define FXT_PT_MOTOR_CURRENT        10
#define FXT_PT_INPUTBYTE            13
#define FXT_PT_OUTPUTPT             16
#define FXT_PT_CONNECT_BIT4         19
#define FXT_PT_OUTPUT_ANDGATE       20
#define FXT_PT_AUTO_TRUE            21

#define FXT_PT_TOTAL_NUM_PTS        23
#define FXT_PT_TOTAL_NANMED_PTS     10

#define MAX_POINTS                  FXT_PT_TOTAL_NUM_PTS



////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Node" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Fxt::Point::Database<MAX_POINTS>                    pointDb;
    Fxt::Type::Error                                    nodeError;
    Fxt::Node::Mock::Kestrel::Factory                   uutFactory( 10000,
                                                                    Fxt::Card::Mock::AnalogIn8::CARD_STATEFUL_HEAP_SIZE + Fxt::Card::Mock::Digital8::CARD_STATEFUL_HEAP_SIZE,
                                                                    10000 );
    Cpl::Text::FString<Fxt::Type::Error::MAX_TEXT_LEN>  buf;


    SECTION( "json create" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, NODE_DEFINITION );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("json error=%s", err.c_str()) );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant nodeJsonObj = doc.as<JsonVariant>();
        REQUIRE( Api::getNode() == nullptr );
        Api* uut = uutFactory.createFromJSON( nodeJsonObj,
                                              pointDb,
                                              nodeError );

        CPL_SYSTEM_TRACE_MSG( SECT_, ("node error=%s", nodeError.toText( buf )) );
        REQUIRE( uut );
        REQUIRE( uut->getErrorCode() == Fxt::Type::Error::SUCCESS() );

        REQUIRE( uut->getNumChassis() == 1 );
        REQUIRE( uut->getChassis( 0 ) );
        REQUIRE( uut->getChassis( 1 ) == nullptr );
        REQUIRE( Api::getNode() == uut );

        uutFactory.destroy( *uut );
    }

    SECTION( "execute" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, NODE_DEFINITION );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("json error=%s", err.c_str()) );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant nodeJsonObj = doc.as<JsonVariant>();
        Api* uut = uutFactory.createFromJSON( nodeJsonObj,
                                              pointDb,
                                              nodeError );

        CPL_SYSTEM_TRACE_MSG( SECT_, ("node error=%s", nodeError.toText( buf )) );
        REQUIRE( uut );
        REQUIRE( uut->getErrorCode() == Fxt::Type::Error::SUCCESS() );

        // Run at least one interval
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Starting... node error=%s", nodeError.toText( buf )) );
        uut->start( Fxt::System::ElapsedTime::now() );
        Cpl::System::Api::sleep( 2500 );
        REQUIRE( uut->isStarted() );
        uut->stop();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Stop node error=%s", uut->getErrorCode().toText( buf )) );
        REQUIRE( uut->getErrorCode() == Fxt::Type::Error::SUCCESS() );

        // AnalogCard: Verify Point values
        float floatPointVal = 0;
        Fxt::Point::Float* floatPointPtr = (Fxt::Point::Float*) pointDb.lookupById( FXT_PT_MOTOR_TEMPERATURE );
        REQUIRE( floatPointPtr );
        REQUIRE( floatPointPtr->read( floatPointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( floatPointVal, 1.2F ) );
        floatPointPtr = (Fxt::Point::Float*) pointDb.lookupById( FXT_PT_MOTOR_VOLTAGE );
        REQUIRE( floatPointPtr );
        REQUIRE( floatPointPtr->read( floatPointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( floatPointVal, 3.5F ) );
        floatPointPtr = (Fxt::Point::Float*) pointDb.lookupById( FXT_PT_MOTOR_CURRENT );
        REQUIRE( floatPointPtr );
        REQUIRE( floatPointPtr->read( floatPointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( floatPointVal, 0.0F ) );

        // Digital: Verify Point values
        uint8_t uint8PointVal = 0;
        Fxt::Point::Uint8* uint8PointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( FXT_PT_INPUTBYTE );
        REQUIRE( uint8PointPtr );
        REQUIRE( uint8PointPtr->read( uint8PointVal ) );
        REQUIRE( uint8PointVal == 128 );
        uint8PointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( FXT_PT_OUTPUTPT );
        REQUIRE( uint8PointPtr->isNotValid() );  // Nothing drives this output

        // Shared Points: Verify Point values
        bool boolPointVal = false;
        Fxt::Point::Bool* boolPointPtr = (Fxt::Point::Bool*) pointDb.lookupById( FXT_PT_SHARED_1 );
        REQUIRE( boolPointPtr );
        REQUIRE( boolPointPtr->read( boolPointVal ) );
        REQUIRE( boolPointVal == true );
        boolPointPtr = (Fxt::Point::Bool*) pointDb.lookupById( FXT_PT_SHARED_2 );
        REQUIRE( boolPointPtr );
        REQUIRE( boolPointPtr->read( boolPointVal ) );
        REQUIRE( boolPointVal == false );

        // Logic Chain Output: Verify Point values
        boolPointPtr = (Fxt::Point::Bool*) pointDb.lookupById( FXT_PT_OUTPUT_ANDGATE );
        REQUIRE( boolPointPtr );
        REQUIRE( boolPointPtr->read( boolPointVal ) );
        REQUIRE( boolPointVal == false );

        // Logic Chain Auto Pt: Verify Point values
        boolPointPtr = (Fxt::Point::Bool*) pointDb.lookupById( FXT_PT_AUTO_TRUE );
        REQUIRE( boolPointPtr );
        REQUIRE( boolPointPtr->read( boolPointVal ) );
        REQUIRE( boolPointVal == true );

        //
        // Restart the NODE
        //
        floatPointPtr = (Fxt::Point::Float*) pointDb.lookupById( FXT_PT_MOTOR_TEMPERATURE );
        floatPointPtr->setInvalid();
        floatPointPtr = (Fxt::Point::Float*) pointDb.lookupById( FXT_PT_MOTOR_VOLTAGE );
        floatPointPtr->setInvalid();
        floatPointPtr = (Fxt::Point::Float*) pointDb.lookupById( FXT_PT_MOTOR_CURRENT );
        floatPointPtr->setInvalid();
        uint8PointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( FXT_PT_INPUTBYTE );
        uint8PointPtr->setInvalid();
        uint8PointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( FXT_PT_OUTPUTPT );
        uint8PointPtr->setInvalid();
        boolPointPtr = (Fxt::Point::Bool*) pointDb.lookupById( FXT_PT_SHARED_1 );
        boolPointPtr->setInvalid();
        boolPointPtr = (Fxt::Point::Bool*) pointDb.lookupById( FXT_PT_SHARED_2 );
        boolPointPtr->setInvalid();
        boolPointPtr = (Fxt::Point::Bool*) pointDb.lookupById( FXT_PT_OUTPUT_ANDGATE );
        boolPointPtr->setInvalid();

        // Logic Chain Auto Pt: Verify Point values
        boolPointPtr = (Fxt::Point::Bool*) pointDb.lookupById( FXT_PT_AUTO_TRUE );

        // Run at least one interval
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Starting (again)... node error=%s", nodeError.toText( buf )) );
        uut->start( Fxt::System::ElapsedTime::now() );
        Cpl::System::Api::sleep( 2500 );
        REQUIRE( uut->isStarted() );
        uut->stop();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Stop node error=%s", uut->getErrorCode().toText( buf )) );
        REQUIRE( uut->getErrorCode() == Fxt::Type::Error::SUCCESS() );

        // AnalogCard: Verify Point values
        floatPointVal = 0;
        floatPointPtr = (Fxt::Point::Float*) pointDb.lookupById( FXT_PT_MOTOR_TEMPERATURE );
        REQUIRE( floatPointPtr );
        REQUIRE( floatPointPtr->read( floatPointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( floatPointVal, 1.2F ) );
        floatPointPtr = (Fxt::Point::Float*) pointDb.lookupById( FXT_PT_MOTOR_VOLTAGE );
        REQUIRE( floatPointPtr );
        REQUIRE( floatPointPtr->read( floatPointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( floatPointVal, 3.5F ) );
        floatPointPtr = (Fxt::Point::Float*) pointDb.lookupById( FXT_PT_MOTOR_CURRENT );
        REQUIRE( floatPointPtr );
        REQUIRE( floatPointPtr->read( floatPointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( floatPointVal, 0.0F ) );

        // Digital: Verify Point values
        uint8PointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( FXT_PT_INPUTBYTE );
        REQUIRE( uint8PointPtr );
        REQUIRE( uint8PointPtr->read( uint8PointVal ) );
        REQUIRE( uint8PointVal == 128 );
        uint8PointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( FXT_PT_OUTPUTPT );
        REQUIRE( uint8PointPtr->isNotValid() );  // Nothing drives this output

        // Shared Points: Verify Point values
        boolPointPtr = (Fxt::Point::Bool*) pointDb.lookupById( FXT_PT_SHARED_1 );
        REQUIRE( boolPointPtr );
        REQUIRE( boolPointPtr->read( boolPointVal ) );
        REQUIRE( boolPointVal == true );
        boolPointPtr = (Fxt::Point::Bool*) pointDb.lookupById( FXT_PT_SHARED_2 );
        REQUIRE( boolPointPtr );
        REQUIRE( boolPointPtr->read( boolPointVal ) );
        REQUIRE( boolPointVal == false );

        // Logic Chain Output: Verify Point values
        boolPointPtr = (Fxt::Point::Bool*) pointDb.lookupById( FXT_PT_OUTPUT_ANDGATE );
        REQUIRE( boolPointPtr );
        REQUIRE( boolPointPtr->read( boolPointVal ) );
        REQUIRE( boolPointVal == false );

        // Logic Chain Auto Pt: Verify Point values
        boolPointPtr = (Fxt::Point::Bool*) pointDb.lookupById( FXT_PT_AUTO_TRUE );
        REQUIRE( boolPointPtr );
        REQUIRE( boolPointPtr->read( boolPointVal ) );
        REQUIRE( boolPointVal == true );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}