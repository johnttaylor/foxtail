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
#include "Fxt/Component/Digital/Mux.h"
#include "Fxt/Component/Digital/Error.h"
#include "Fxt/Point/Database.h"
#include "Fxt/Point/FactoryDatabase.h"
#include "Cpl/Memory/LeanHeap.h"
#include "Fxt/Point/Int16.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Component::Digital;


#define COMP_DEFINTION     "{\"components\":[" \
                           "{" \
                           "  \"name\": \"Mux #1\"," \
                           "  \"type\": \"d60f2daf-9709-42d6-ba92-b76f641eb930\"," \
                           "  \"typeName\": \"Fxt::Component::Digital::Mux\"," \
                           "  \"inputs\": [" \
                           "      {" \
                           "          \"bit\": 4," \
                           "          \"name\": \"bit4\"," \
                           "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                           "          \"typeName\": \"Fxt::Point::Bool\"," \
                           "          \"idRef\": 0," \
                           "          \"negate\": false" \
                           "      }," \
                           "      {" \
                           "          \"bit\": 0," \
                           "          \"name\": \"/bit1\"," \
                           "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                           "          \"typeName\": \"Fxt::Point::Bool\"," \
                           "          \"idRef\": 1," \
                           "          \"negate\": true" \
                           "      }," \
                           "      {" \
                           "          \"bit\": 1," \
                           "          \"name\": \"bit1\"," \
                           "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                           "          \"typeName\": \"Fxt::Point::Bool\"," \
                           "          \"idRef\": 2" \
                           "      }" \
                           "    ]," \
                           "  \"outputs\": [" \
                           "      {" \
                           "          \"name\": \"output word\"," \
                           "          \"type\": \"9b747497-eabf-4d11-8ce3-11ad9b048c43\"," \
                           "          \"typeName\": \"Fxt::Point::Int16\"," \
                           "          \"idRef\": 3" \
                           "      }" \
                          "    ]" \
                           "  }" \
                           "]}"

static size_t generalHeap_[10000];
static size_t statefulHeap_[10000];

#define MAX_POINTS              4

#define POINT_ID__OUT_SIGNAL    3

#define POINT_ID__BIT4_IN       0
#define POINT_ID__BIT0_IN       1
#define POINT_ID__BIT1_IN       2



////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Mux" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap statefulAllocator( statefulHeap_, sizeof( statefulHeap_ ) );
    Fxt::Point::FactoryDatabase      pointFactoryDb;
    Fxt::Point::Database<MAX_POINTS> pointDb;

    SECTION( "create component" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, COMP_DEFINTION );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("json error=%s", err.c_str()) );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant componentObj = doc["components"][0];
        Mux uut( componentObj,
                 generalAllocator,
                 statefulAllocator,
                 pointFactoryDb,
                 pointDb );

        REQUIRE( uut.getErrorCode() == Fxt::Type::Error::SUCCESS() );

        REQUIRE( strcmp( uut.getTypeGuid(), Mux::GUID_STRING ) == 0 );
        REQUIRE( strcmp( uut.getTypeName(), Mux::TYPE_NAME ) == 0 );

        REQUIRE( uut.isStarted() == false );
    }

    SECTION( "start component" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, COMP_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant componentObj = doc["components"][0];
        Mux uut( componentObj,
                 generalAllocator,
                 statefulAllocator,
                 pointFactoryDb,
                 pointDb );

        REQUIRE( uut.getErrorCode() == Fxt::Type::Error::SUCCESS() );

        uint64_t nowUsec = Cpl::System::ElapsedTime::milliseconds() * 1000;

        REQUIRE( uut.start( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( uut.isStarted() == true );
    }

    SECTION( "execute component - happy path" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, COMP_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant componentObj = doc["components"][0];
        Mux uut( componentObj,
                 generalAllocator,
                 statefulAllocator,
                 pointFactoryDb,
                 pointDb );

        REQUIRE( uut.getErrorCode() == Fxt::Type::Error::SUCCESS() );

        Fxt::Point::Int16* ptOut    = new(std::nothrow) Fxt::Point::Int16( pointDb, POINT_ID__OUT_SIGNAL, statefulAllocator );
        Fxt::Point::Bool*  ptInBit0 = new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT0_IN, statefulAllocator );
        Fxt::Point::Bool*  ptInBit1 = new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT1_IN, statefulAllocator );
        Fxt::Point::Bool*  ptInBit4 = new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT4_IN, statefulAllocator );

        Fxt::Type::Error errCode = uut.resolveReferences( pointDb );
        REQUIRE( errCode == Fxt::Type::Error::SUCCESS() );

        uint64_t nowUsec = Cpl::System::ElapsedTime::milliseconds() * 1000;
        REQUIRE( uut.start( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( uut.isStarted() == true );

        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut->isNotValid() );

        ptInBit0->write( true );
        ptInBit1->write( true );
        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut->isNotValid() );

        ptInBit4->write( true );
        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        int16_t val;
        REQUIRE( ptOut->read( val ) );
        REQUIRE( val == 0b10010 );

        ptInBit0->write( false );
        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut->read( val ) );
        REQUIRE( val == 0b10011 );

        ptInBit1->setInvalid();
        REQUIRE( uut.execute( nowUsec ) == Fxt::Type::Error::SUCCESS() );
        REQUIRE( ptOut->isNotValid() );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}