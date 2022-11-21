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
#include "Fxt/Card/Mock/Digital8.h"
#include "Fxt/Card/Database.h"
#include "Fxt/Point/Database.h"
#include "Fxt/Point/Uint8.h"
#include "Cpl/Memory/LeanHeap.h"
#include "Cpl/System/Trace.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Card::Mock;


#define CARD_DEFINTION     "{\"cards\":[" \
                           "{" \
                           "  \"name\": \"My Digital8 Card\"," \
                           "  \"id\": 0," \
                           "  \"type\": \"59d33888-62c7-45b2-a4d4-9dbc55914ed3\"," \
                           "  \"typename\": \"Fxt::Card::HW::Mock::Digital8\"," \
                           "  \"slot\": 0," \
                           "    \"points\": {" \
                           "       \"inputs\": [" \
                           "           {" \
                           "               \"channel\": 1," \
                           "                   \"id\": 1," \
                           "                   \"ioRegId\": 2," \
                           "                   \"name\": \"InputPt\"," \
                           "                   \"initial\": {" \
                           "                     \"valid\": true," \
                           "                     \"val\": 128," \
                           "                     \"id\": 0" \
                           "                   }" \
                           "           }" \
                           "       ]," \
                           "       \"outputs\": [" \
                           "           {" \
                           "              \"channel\": 1," \
                           "              \"id\": 3," \
                           "              \"ioRegId\": 4," \
                           "              \"name\": \"OutputPt\"," \
                           "              \"initial\": {" \
                           "                 \"valid\": true," \
                           "                 \"val\": 1," \
                           "                 \"id\": 0" \
                           "              }" \
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
TEST_CASE( "Digital8" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap statefulAllocator( statefulHeap_, sizeof( statefulHeap_ ) );
    Fxt::Point::Database<MAX_POINTS> pointDb;
    Fxt::Card::Database<MAX_CARDS>   cardDb;
    Cpl::Text::FString<Fxt::Type::Error::MAX_TEXT_LEN> errText;


    SECTION( "create card" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, CARD_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant cardObj = doc["cards"][0];
        Digital8 uut( cardDb,
                      generalAllocator,
                      statefulAllocator,
                      pointDb,
                      0,
                      cardObj );

        CPL_SYSTEM_TRACE_MSG( SECT_, ("error Code=%s", uut.getErrorCode().toText( errText )) );
        REQUIRE( uut.getErrorCode() == Fxt::Type::Error( Fxt::Type::Err_T::SUCCESS ) );

        REQUIRE( strcmp( uut.getTypeName(), Digital8::TYPE_NAME ) == 0 );
        REQUIRE( strcmp( uut.getTypeGuid(), Digital8::GUID_STRING ) == 0 );

        REQUIRE( uut.getId() == 0 );

        Fxt::Point::Uint8* pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 1 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "InputPt" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 2 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "InputPt" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 3 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "OutputPt" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 4 );
        REQUIRE( pointPtr );
        REQUIRE( strcmp( pointPtr->getName(), "OutputPt" ) == 0 );
        REQUIRE( pointPtr->isNotValid() );

        REQUIRE( uut.isStarted() == false );
        REQUIRE( uut.start() );
        REQUIRE( uut.isStarted() );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 2 );
        uint8_t pointVal = 0;
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( pointVal == 128 );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 4 );
        pointVal = 0;
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( pointVal == 1 );


        uut.scanInputs();
        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 1 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( pointVal == 128 );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 2 );
        pointPtr->setInvalid();
        uut.scanInputs();
        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 1 );
        REQUIRE( pointPtr->isNotValid() );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 3 );
        pointPtr->write( 32 );
        uut.flushOutputs();
        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 4 );
        REQUIRE( pointPtr->read( pointVal ) );
        REQUIRE( pointVal == 32 );

        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 3 );
        pointPtr->setInvalid();
        uut.flushOutputs();
        pointPtr = (Fxt::Point::Uint8*) pointDb.lookupById( 4 );
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
        Digital8 uut( cardDb,
                      generalAllocator,
                      statefulAllocator,
                      pointDb,
                      0,
                      cardObj );

        REQUIRE( uut.getErrorCode() == Fxt::Type::Error( Fxt::Type::Err_T::SUCCESS ) );
        REQUIRE( uut.start() );

        uut.setInputBit( 1 );
        Fxt::Point::Uint8* pointPtr   = (Fxt::Point::Uint8*) pointDb.lookupById( 2 );
        uint8_t            pointValue = 0;
        REQUIRE( pointPtr->read( pointValue ) );
        REQUIRE( pointValue == 0x82 );
        uut.toggleInputBit( 7 );
        REQUIRE( pointPtr->read( pointValue ) );
        REQUIRE( pointValue == 0x02 );
        uut.setInputBit( 0 );
        uut.clearInputBit( 1 );
        REQUIRE( pointPtr->read( pointValue ) );
        REQUIRE( pointValue == 0x01 );

        pointPtr   = (Fxt::Point::Uint8*) pointDb.lookupById( 3 );
        pointPtr->write( 0xAA );
        uut.flushOutputs();
        REQUIRE( uut.getOutputs( pointValue ) );
        REQUIRE( pointValue == 0xAA );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}