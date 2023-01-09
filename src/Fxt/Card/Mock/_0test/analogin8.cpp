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
#include "Fxt/Card/Mock/AnalogIn8.h"
#include "Fxt/Point/Float.h"
#include "Fxt/Point/Database.h"
#include "Cpl/Memory/LeanHeap.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"
#include "Fxt/Point/FactoryDatabase.h"
#include "Fxt/Point/Factory.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Card::Mock;


#define CARD_DEFINTION     "{\"cards\":[" \
                           "{" \
                           "  \"name\": \"bob\"," \
                           "  \"type\": \"1968f533-e323-4ae4-8493-9a572f3bd195\"," \
                           "  \"typename\": \"Fxt::Card::HW::Mock::AnalogIn8\"," \
                           "  \"slot\": 22," \
                           "    \"points\": {" \
                           "       \"inputs\": [" \
                           "           {" \
                           "               \"channel\": 2," \
                           "                   \"id\": 1," \
                           "                   \"ioRegId\": 2," \
                           "                   \"type\": \"708745fa-cef6-4364-abad-063a40f35cbc\"," \
                           "                   \"typeName\": \"Fxt::Point::Float\"," \
                           "                   \"name\": \"Motor Temperature\"," \
                           "                   \"initial\": {" \
                           "                     \"valid\": true," \
                           "                     \"val\": 1.2," \
                           "                     \"id\": 0" \
                           "                   }" \
                           "           }," \
                           "           {" \
                           "               \"channel\": 5," \
                           "                   \"id\": 3," \
                           "                   \"ioRegId\": 4," \
                           "                   \"type\": \"708745fa-cef6-4364-abad-063a40f35cbc\"," \
                           "                   \"typeName\": \"Fxt::Point::Float\"," \
                           "                   \"name\": \"Motor Voltage\"," \
                           "                   \"initial\": {" \
                           "                     \"valid\": true," \
                           "                     \"val\": 3.5," \
                           "                     \"id\": 7" \
                           "                   }" \
                           "           }," \
                           "           {" \
                           "               \"channel\": 6," \
                           "                   \"id\": 5," \
                           "                   \"ioRegId\": 6," \
                           "                   \"type\": \"708745fa-cef6-4364-abad-063a40f35cbc\"," \
                           "                   \"typeName\": \"Fxt::Point::Float\"," \
                           "                   \"name\": \"Motor Current\"," \
                           "                   \"initial\": {" \
                           "                     \"valid\": true," \
                           "                     \"val\": 0.0," \
                           "                     \"id\": 8" \
                           "                   }" \
                           "           }" \
                           "       ]" \
                           "    }" \
                           "}" \
                           "]}"

static size_t generalHeap_[10000];
static size_t cardStateFullHeap_[AnalogIn8::CARD_STATEFUL_HEAP_SIZE];
static size_t haStateFullHeap_[10000];

#define MAX_POINTS      100
#define MAX_CARDS       3

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "AnalogIn8" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap                              generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap                              cardStatefulAllocator( cardStateFullHeap_, sizeof( cardStateFullHeap_ ) );
    Cpl::Memory::LeanHeap                              haStatefulAllocator( haStateFullHeap_, sizeof( haStateFullHeap_ ) );
    Fxt::Point::Database<MAX_POINTS>                   pointDb;
    Fxt::Point::FactoryDatabase                        pointFactoryDb;
    Fxt::Point::Factory<Fxt::Point::Float>             factoryFloat( pointFactoryDb );
    Cpl::Text::FString<Fxt::Type::Error::MAX_TEXT_LEN> errText;

    SECTION( "create card" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, CARD_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant cardObj = doc["cards"][0];
        AnalogIn8 uut( generalAllocator,
                       cardStatefulAllocator,
                       haStatefulAllocator,
                       pointFactoryDb,
                       pointDb,
                       cardObj );

        CPL_SYSTEM_TRACE_MSG( SECT_, ("error Code=%s", Fxt::Type::Error::toText( uut.getErrorCode(), errText )) );
        REQUIRE( uut.getErrorCode() == Fxt::Type::Error::SUCCESS() );

        REQUIRE( strcmp( uut.getTypeName(), AnalogIn8::TYPE_NAME ) == 0 );
        REQUIRE( strcmp( uut.getTypeGuid(), AnalogIn8::GUID_STRING ) == 0 );
        REQUIRE( uut.getSlotNumber() == 22 );

        Fxt::Point::Float* pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 1 );
        REQUIRE( pointPtr );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 2 );
        REQUIRE( pointPtr );
        REQUIRE( pointPtr->isNotValid() == false );
        pointPtr->setInvalid();

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 3 );
        REQUIRE( pointPtr );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 4 );
        REQUIRE( pointPtr );
        REQUIRE( pointPtr->isNotValid() == false );
        pointPtr->setInvalid();

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 5 );
        REQUIRE( pointPtr );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 6 );
        REQUIRE( pointPtr );
        REQUIRE( pointPtr->isNotValid() == false );
        pointPtr->setInvalid();

        REQUIRE( uut.isStarted() == false );
        REQUIRE( uut.start( 0LL ) );
        REQUIRE( uut.isStarted() );

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 2 );
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

        uut.scanInputs( 0LL );
        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 1 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 1.2F ) );

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 3 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 3.5F ) );

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 5 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 0.0F ) );


        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 2 );
        pointPtr->setInvalid();
        uut.scanInputs( 0LL );
        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 1 );
        REQUIRE( pointPtr->isNotValid() );
        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 3 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 3.5F ) );
        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 5 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 0.0F ) );

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 2 );
        pointPtr->write( 3.14F );
        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 4 );
        pointPtr->setInvalid();
        uut.scanInputs( 0LL );
        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 1 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 3.14F ) );
        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 3 );
        REQUIRE( pointPtr->isNotValid() );
        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 5 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 0.0F ) );

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 4 );
        pointPtr->write( 99.99F );
        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 6 );
        pointPtr->setInvalid();
        uut.scanInputs( 0LL );
        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 1 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 3.14F ) );
        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 3 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 99.99F ) );
        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 5 );
        REQUIRE( pointPtr->isNotValid() );


        uut.stop();
        REQUIRE( uut.isStarted() == false );
    }

    SECTION( "app interface" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, CARD_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant cardObj = doc["cards"][0];
        AnalogIn8 uut( generalAllocator,
                       cardStatefulAllocator,
                       haStatefulAllocator,
                       pointFactoryDb,
                       pointDb,
                       cardObj );

        REQUIRE( uut.getErrorCode() == Fxt::Type::Error::SUCCESS()  );

        REQUIRE( uut.start( 0LL ) );

        uut.setInput( 1, 22.2F );
        uut.setInput( 2, 2 );
        uut.setInput( 5, 5 );
        uut.setInput( 6, 6 );
        uut.scanInputs( 0LL );
        float              pointVal = 0;
        Fxt::Point::Float* pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 1 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 2 ) );
        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 3 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 5 ) );
        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 5 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 6 ) );

    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}