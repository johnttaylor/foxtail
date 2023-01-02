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
#include "Fxt/System/Tick1MsecBlocking.h"
#include "Fxt/Chassis/Chassis.h"
#include "Fxt/Chassis/Server.h"
#include "Fxt/Point/Database.h"
#include "Fxt/Component/FactoryDatabase.h"
#include "Fxt/Point/FactoryDatabase.h"
#include "Fxt/Point/Uint8.h"
#include "Fxt/Point/Float.h"
#include "Fxt/Point/Factory.h"
#include "Fxt/Card/FactoryDatabase.h"
#include "Fxt/Card/Mock/AnalogIn8Factory.h"
#include "Fxt/Card/Mock/Digital8Factory.h"
#include "Fxt/Component/Digital/And16GateFactory.h"
#include "Fxt/Component/Digital/ByteSplitterFactory.h"
#include "Cpl/Memory/LeanHeap.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Chassis;

#define CHASSIS_DEFINITION              "{" \
                                        "  \"name\": \"My Chassis\", " \
                                        "  \"id\": 1, " \
                                        "  \"fer\": 1000, " \
                                        "  \"sharedPts\": [ " \
                                        "    { " \
                                        "      \"id\": 15, " \
                                        "      \"name\": \"Shared1\", " \
                                        "      \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\", " \
                                        "      \"typeName\": \"Fxt::Point::Bool\", " \
                                        "      \"initial\": { " \
                                        "        \"val\": true, " \
                                        "        \"id\": 16 " \
                                        "      } " \
                                        "    }, " \
                                        "    { " \
                                        "      \"id\": 17, " \
                                        "      \"name\": \"Shared2\", " \
                                        "      \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\", " \
                                        "      \"typeName\": \"Fxt::Point::Bool\", " \
                                        "      \"initial\": { " \
                                        "        \"val\": false, " \
                                        "        \"id\": 18 " \
                                        "      } " \
                                        "    } " \
                                        "  ], " \
                                        "  \"scanners\": [ " \
                                        "    { " \
                                        "      \"name\": \"My Scanner\", " \
                                        "      \"id\": 1, " \
                                        "      \"scanRateMultiplier\": 2, " \
                                        "      \"cards\": [ " \
                                        "        { " \
                                        "          \"name\": \"bob\", " \
                                        "          \"type\": \"1968f533-e323-4ae4-8493-9a572f3bd195\", " \
                                        "          \"typename\": \"Fxt::Card::HW::Mock::AnalogIn8\", " \
                                        "          \"slot\": 22, " \
                                        "          \"points\": { " \
                                        "            \"inputs\": [ " \
                                        "              { " \
                                        "                \"channel\": 2, " \
                                        "                \"id\": 1, " \
                                        "                \"ioRegId\": 2, " \
                                        "                \"type\": \"708745fa-cef6-4364-abad-063a40f35cbc\", " \
                                        "                \"typeName\": \"Fxt::Point::Float\", " \
                                        "                \"name\": \"Motor Temperature\", " \
                                        "                \"initial\": { " \
                                        "                  \"valid\": true, " \
                                        "                  \"val\": 1.2, " \
                                        "                  \"id\": 0 " \
                                        "                } " \
                                        "              }, " \
                                        "              { " \
                                        "                \"channel\": 5, " \
                                        "                \"id\": 3, " \
                                        "                \"ioRegId\": 4, " \
                                        "                \"type\": \"708745fa-cef6-4364-abad-063a40f35cbc\", " \
                                        "                \"typeName\": \"Fxt::Point::Float\", " \
                                        "                \"name\": \"Motor Voltage\", " \
                                        "                \"initial\": { " \
                                        "                  \"valid\": true, " \
                                        "                  \"val\": 3.5, " \
                                        "                  \"id\": 7 " \
                                        "                } " \
                                        "              }, " \
                                        "              { " \
                                        "                \"channel\": 6, " \
                                        "                \"id\": 5, " \
                                        "                \"ioRegId\": 6, " \
                                        "                \"type\": \"708745fa-cef6-4364-abad-063a40f35cbc\", " \
                                        "                \"typeName\": \"Fxt::Point::Float\", " \
                                        "                \"name\": \"Motor Current\", " \
                                        "                \"initial\": { " \
                                        "                  \"valid\": true, " \
                                        "                  \"val\": 0, " \
                                        "                  \"id\": 8 " \
                                        "                } " \
                                        "              } " \
                                        "            ] " \
                                        "          } " \
                                        "        }, " \
                                        "        { " \
                                        "          \"name\": \"My Digital8 Card\", " \
                                        "          \"type\": \"59d33888-62c7-45b2-a4d4-9dbc55914ed3\", " \
                                        "          \"typename\": \"Fxt::Card::HW::Mock::Digital8\", " \
                                        "          \"slot\": 0, " \
                                        "          \"points\": { " \
                                        "            \"inputs\": [ " \
                                        "              { " \
                                        "                \"channel\": 1, " \
                                        "                \"id\": 9, " \
                                        "                \"ioRegId\": 10, " \
                                        "                \"type\": \"918cff9e-8007-4666-99ac-384b9624329c\", " \
                                        "                \"typeName\": \"Fxt::Point::Uint8\", " \
                                        "                \"name\": \"InputPt\", " \
                                        "                \"initial\": { " \
                                        "                  \"valid\": true, " \
                                        "                  \"val\": 128, " \
                                        "                  \"id\": 11 " \
                                        "                } " \
                                        "              } " \
                                        "            ], " \
                                        "            \"outputs\": [ " \
                                        "              { " \
                                        "                \"channel\": 1, " \
                                        "                \"id\": 12, " \
                                        "                \"ioRegId\": 13, " \
                                        "                \"type\": \"918cff9e-8007-4666-99ac-384b9624329c\", " \
                                        "                \"typeName\": \"Fxt::Point::Uint8\", " \
                                        "                \"name\": \"OutputPt\", " \
                                        "                \"initial\": { " \
                                        "                  \"valid\": true, " \
                                        "                  \"val\": 1, " \
                                        "                  \"id\": 14 " \
                                        "                } " \
                                        "              } " \
                                        "            ] " \
                                        "          } " \
                                        "        } " \
                                        "      ] " \
                                        "    } " \
                                        "  ], " \
                                        "  \"executionSets\": [ " \
                                        "    { " \
                                        "      \"name\": \"My Execution Set\", " \
                                        "      \"id\": 1, " \
                                        "      \"exeRateMultiplier\": 2, " \
                                        "      \"logicChains\": [ " \
                                        "        { " \
                                        "          \"name\": \"my logic chain\", " \
                                        "          \"id\": 1, " \
                                        "          \"components\": [ " \
                                        "            { " \
                                        "              \"id\": 100, " \
                                        "              \"name\": \"ByteSplitter #1\", " \
                                        "              \"type\": \"8c55aa52-3bc8-4b8a-ad73-c434a0bbd4b4\", " \
                                        "              \"typeName\": \"Fxt::Component::Digital::ByteSplitter\", " \
                                        "              \"inputs\": [ " \
                                        "                { " \
                                        "                  \"name\": \"input byte\", " \
                                        "                  \"type\": \"918cff9e-8007-4666-99ac-384b9624329c\", " \
                                        "                  \"typeName\": \"Fxt::Point::Uint8\", " \
                                        "                  \"idRef\": 19 " \
                                        "                } " \
                                        "              ], " \
                                        "              \"outputs\": [ " \
                                        "                { " \
                                        "                  \"bit\": 4, " \
                                        "                  \"name\": \"/bit4\", " \
                                        "                  \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\", " \
                                        "                  \"typeName\": \"Fxt::Point::Bool\", " \
                                        "                  \"idRef\": 20 " \
                                        "                } " \
                                        "              ] " \
                                        "            }, " \
                                        "            { " \
                                        "              \"id\": 101, " \
                                        "              \"name\": \"AND Gate#1\", " \
                                        "              \"type\": \"e62e395c-d27a-4821-bba9-aa1e6de42a05\", " \
                                        "              \"typeName\": \"Fxt::Component::Digital::And16Gate\", " \
                                        "              \"inputs\": [ " \
                                        "                { " \
                                        "                  \"name\": \"Signal#1\", " \
                                        "                  \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\", " \
                                        "                  \"typeName\": \"Fxt::Point::Bool\", " \
                                        "                  \"idRef\": 20 " \
                                        "                }, " \
                                        "                { " \
                                        "                  \"name\": \"Signal#2\", " \
                                        "                  \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\", " \
                                        "                  \"typeName\": \"Fxt::Point::Bool\", " \
                                        "                  \"idRef\": 21 " \
                                        "                } " \
                                        "              ], " \
                                        "              \"outputs\": [ " \
                                        "                { " \
                                        "                  \"name\": \"out\", " \
                                        "                  \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\", " \
                                        "                  \"typeName\": \"Fxt::Point::Bool\", " \
                                        "                  \"idRef\": 23 " \
                                        "                } " \
                                        "              ] " \
                                        "            } " \
                                        "          ], " \
                                        "          \"connectionPts\": [ " \
                                        "            { " \
                                        "              \"id\": 19, " \
                                        "              \"name\": \"Input to ByteSplitter\", " \
                                        "              \"type\": \"918cff9e-8007-4666-99ac-384b9624329c\", " \
                                        "              \"typeName\": \"Fxt::Point::Uint8\" " \
                                        "            }, " \
                                        "            { " \
                                        "              \"id\": 20, " \
                                        "              \"name\": \"Output ByteSplitter: Bit 4\", " \
                                        "              \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\", " \
                                        "              \"typeName\": \"Fxt::Point::Bool\" " \
                                        "            }, " \
                                        "            { " \
                                        "              \"id\": 23, " \
                                        "              \"name\": \"Output AndGate\", " \
                                        "              \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\", " \
                                        "              \"typeName\": \"Fxt::Point::Bool\" " \
                                        "            } " \
                                        "          ], " \
                                        "          \"autoPts\": [ " \
                                        "            { " \
                                        "              \"id\": 21, " \
                                        "              \"name\": \"Auto-true\", " \
                                        "              \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\", " \
                                        "              \"typeName\": \"Fxt::Point::Bool\", " \
                                        "              \"initial\": { " \
                                        "                \"val\": true, " \
                                        "                \"id\": 22" \
                                        "              } " \
                                        "            } " \
                                        "          ] " \
                                        "        } " \
                                        "      ] " \
                                        "    } " \
                                        "  ] " \
                                        "} " \

static size_t generalHeap_[10000];
static size_t cardStateFullHeap_[Fxt::Card::Mock::AnalogIn8::CARD_STATEFUL_HEAP_SIZE + Fxt::Card::Mock::Digital8::CARD_STATEFUL_HEAP_SIZE];
static size_t haStateFullHeap_[10000];

#define MAX_POINTS      100



////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Chassis" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap                               generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap                               cardStatefulAllocator( cardStateFullHeap_, sizeof( cardStateFullHeap_ ) );
    Cpl::Memory::LeanHeap                               haStatefulAllocator( haStateFullHeap_, sizeof( haStateFullHeap_ ) );
    Fxt::Component::FactoryDatabase                     componentFactoryDb;
    Fxt::Component::Digital::ByteSplitterFactory        byteSplitterFactory( componentFactoryDb );
    Fxt::Component::Digital::And16GateFactory           and16GateFactory( componentFactoryDb );
    Fxt::Point::Database<MAX_POINTS>                    pointDb;
    Fxt::Point::FactoryDatabase                         pointFactoryDb;
    Fxt::Type::Error                                    chassisError;
    Fxt::Point::Factory<Fxt::Point::Uint8>              factoryUint8( pointFactoryDb );
    Fxt::Point::Factory<Fxt::Point::Float>              factoryFloat( pointFactoryDb );
    Fxt::Point::Factory<Fxt::Point::Bool>               factoryBool( pointFactoryDb );
    Fxt::Card::FactoryDatabase                          cardFactoryDb;
    Fxt::Card::Mock::AnalogIn8Factory                   factoryCardAnalogIn8( cardFactoryDb );
    Fxt::Card::Mock::Digital8Factory                    factoryCardDigital8( cardFactoryDb );

    Cpl::Text::FString<Fxt::Type::Error::MAX_TEXT_LEN>  buf;
    Server<Fxt::System::Tick1MsecBlocking>              chassisServer( 100 * 1000LL );

    SECTION( "raw create" )
    {

        Chassis* uut = new Chassis( chassisServer, generalAllocator, 666, 2, 13 );
        REQUIRE( uut );
        REQUIRE( uut->getFER() == 666 );
        REQUIRE( uut->getErrorCode() == Fxt::Type::Error::SUCCESS() );
        delete uut;
    }

    SECTION( "json create" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, CHASSIS_DEFINITION );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("json error=%s", err.c_str()) );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant chassisJsonObj = doc.as<JsonVariant>();
        Api* uut = Chassis::createChassisfromJSON( chassisJsonObj,
                                                   chassisServer,
                                                   componentFactoryDb,
                                                   cardFactoryDb,
                                                   generalAllocator,
                                                   cardStatefulAllocator,
                                                   haStatefulAllocator,
                                                   pointFactoryDb,
                                                   pointDb,
                                                   chassisError );

        CPL_SYSTEM_TRACE_MSG( SECT_, ("chassis error=%s", chassisError.toText( buf )) );
        REQUIRE( uut );
        REQUIRE( uut->getErrorCode() == Fxt::Type::Error::SUCCESS() );
    }

#if 0
    SECTION( "execute" )
    {
        Cpl::System::Thread* t1  = Cpl::System::Thread::create( chassisServer, "Chassis" );

        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, CHASSIS_DEFINITION );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("json error=%s", err.c_str()) );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant scannerJsonObj = doc.as<JsonVariant>();
        ScannerApi* uut = ScannerApi::createScannerfromJSON( scannerJsonObj,
                                                             generalAllocator,
                                                             cardStatefulAllocator,
                                                             haStatefulAllocator,
                                                             cardFactoryDb,
                                                             pointFactoryDb,
                                                             pointDb,
                                                             chassisError );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("scanner error=%s", chassisError.toText( buf )) );
        REQUIRE( uut );
        REQUIRE( uut->getErrorCode() == Fxt::Type::Error::SUCCESS() );
        REQUIRE( uut->getScanRateMultiplier() == 2 );


        bool result  = uut->start( 00L );
        chassisError = uut->getErrorCode();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("result=%d, scanner error=%s", result, chassisError.toText( buf )) );
        REQUIRE( result );
        REQUIRE( chassisError == Fxt::Type::Error::SUCCESS() );
        REQUIRE( uut->isStarted() );

        // Analog card
        Fxt::Point::Float* pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 2 );
        float pointVal = 0;
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 1.2F ) );

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 4 );
        pointVal = 0;
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 3.5F ) );

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 6 );
        pointVal = 0;
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 0.0F ) );

        // Digital card
        Fxt::Point::Uint8* pointPtr2 = (Fxt::Point::Uint8*) pointDb.lookupById( 10 );
        uint8_t pointVal2 = 0;
        REQUIRE( pointPtr2->read( pointVal2 ) );
        REQUIRE( pointVal2 == 128 );

        pointPtr2 = (Fxt::Point::Uint8*) pointDb.lookupById( 13 );
        pointVal2 = 0;
        REQUIRE( pointPtr2->read( pointVal2 ) );
        REQUIRE( pointVal2 == 1 );


        result = uut->getInputPeriod().execute( 0LL, 0LL );
        chassisError = uut->getErrorCode();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("result=%d, scanner error=%s", result, chassisError.toText( buf )) );
        REQUIRE( result );
        REQUIRE( chassisError == Fxt::Type::Error::SUCCESS() );


        // Analog card
        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 1 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 1.2F ) );

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 3 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 3.5F ) );

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 5 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 0.0F ) );

        // Digital card
        pointPtr2 = (Fxt::Point::Uint8*) pointDb.lookupById( 9 );
        REQUIRE( pointPtr2->read( pointVal2 ) );
        REQUIRE( pointVal2 == 128 );
        pointPtr2 = (Fxt::Point::Uint8*) pointDb.lookupById( 12 );
        pointPtr2->write( 32 );


        result = uut->getOutputPeriod().execute( 0LL, 0LL );


        // Digital Card
        pointPtr2 = (Fxt::Point::Uint8*) pointDb.lookupById( 13 );
        REQUIRE( pointPtr2->read( pointVal2 ) );
        REQUIRE( pointVal2 == 32 );


        // Shutdown threads
        uut.pleaseStop();
        Cpl::System::Api::sleep( 300 ); // allow time for threads to stop
        REQUIRE( t1->isRunning() == false );
        Cpl::System::Thread::destroy( *t1 );
        Cpl::System::Api::sleep( 300 ); // allow time for threads to stop
    }
#endif

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}