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
#include "Fxt/Card/Database.h"
#include "Fxt/Point/Float.h"
#include "Fxt/Point/Database.h"
#include "Cpl/Memory/LeanHeap.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Card::Mock;


#define CARD_DEFINTION     "{\"cards\":[" \
                           "{" \
                           "  \"name\": \"bob\"," \
                           "  \"id\": 1," \
                           "  \"type\": \"1968f533-e323-4ae4-8493-9a572f3bd195\"," \
                           "  \"typename\": \"Fxt::Card::HW::Mock::AnalogIn8\"," \
                           "  \"slot\": 22," \
                           "    \"points\": {" \
                           "       \"inputs\": [" \
                           "           {" \
                           "               \"channel\": 2," \
                           "                   \"id\": 1," \
                           "                   \"ioRegId\": 2," \
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
                           "                   \"name\": \"Motor Voltage\"," \
                           "                   \"initial\": {" \
                           "                     \"valid\": true," \
                           "                     \"val\": 3.5," \
                           "                     \"id\": 0" \
                           "                   }" \
                           "           }," \
                           "           {" \
                           "               \"channel\": 6," \
                           "                   \"id\": 5," \
                           "                   \"ioRegId\": 6," \
                           "                   \"name\": \"Motor Current\"," \
                           "                   \"initial\": {" \
                           "                     \"valid\": true," \
                           "                     \"val\": 0.0," \
                           "                     \"id\": 0" \
                           "                   }" \
                           "           }" \
                           "       ]" \
                           "    }" \
                           "}" \
                           "]}"

static size_t generalHeap_[10000];
static size_t statefulHeap_[10000];

#define MAX_POINTS      100
#define MAX_CARDS       3

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "AnalogIn8" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap statefulAllocator( statefulHeap_, sizeof( statefulHeap_ ) );
    Fxt::Point::Database<MAX_POINTS> pointDb;
    Fxt::Card::Database<MAX_CARDS>   cardDb;


    SECTION( "create card" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, CARD_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant cardObj = doc["cards"][0];
        AnalogIn8 uut( cardDb,
                       generalAllocator,
                       statefulAllocator,
                       pointDb,
                       1,
                       cardObj );

        REQUIRE( uut.getErrorCode() == FXT_CARD_ERR_NO_ERROR );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("error Code=%s", Fxt::Card::Api::getErrorText( uut.getErrorCode() )) );

        REQUIRE( strcmp( uut.getTypeName(), AnalogIn8::TYPE_NAME ) == 0 );
        REQUIRE( strcmp( uut.getTypeGuid(), AnalogIn8::GUID_STRING ) == 0 );

        REQUIRE( uut.getId() == 1 );

        Fxt::Point::Float* pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 1 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "Motor Temperature" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 2 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "Motor Temperature" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 3 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "Motor Voltage" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 4 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "Motor Voltage" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 5 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "Motor Current" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 6 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "Motor Current" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        REQUIRE( uut.isStarted() == false );
        REQUIRE( uut.start() );
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

        uut.scanInputs();
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
        uut.scanInputs();
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
        uut.scanInputs();
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
        uut.scanInputs();
        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 1 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 3.14F ) );
        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 3 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pointVal, 99.99F ) );
        pointPtr = (Fxt::Point::Float*) pointDb.lookupById( 5 );
        REQUIRE( pointPtr->isNotValid() );


        REQUIRE( uut.stop() );
        REQUIRE( uut.isStarted() == false );
    }

    SECTION( "app interface" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, CARD_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant cardObj = doc["cards"][0];
        AnalogIn8 uut( cardDb,
                       generalAllocator,
                       statefulAllocator,
                       pointDb,
                       1,
                       cardObj );

        REQUIRE( uut.getErrorCode() == FXT_CARD_ERR_NO_ERROR );

        REQUIRE( uut.start() );

        uut.setInputs( 1, 22.2F );
        uut.setInputs( 2, 2 );
        uut.setInputs( 5, 5 );
        uut.setInputs( 6, 6 );
        uut.scanInputs();
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