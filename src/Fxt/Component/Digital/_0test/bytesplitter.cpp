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
#include "Fxt/Component/Digital/ByteSplitter.h"
#include "Fxt/Component/Digital/Error.h"
#include "Fxt/Point/Database.h"
#include "Cpl/Memory/LeanHeap.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Component::Digital;


#define COMP_DEFINTION     "{\"components\":[" \
                           "{" \
                           "  \"name\": \"ByteSplitter #1\"," \
                           "  \"type\": \"8c55aa52-3bc8-4b8a-ad73-c434a0bbd4b4\"," \
                           "  \"typeName\": \"Fxt::Component::Digital::ByteSplitter\"," \
                           "  \"inputs\": [" \
                           "      {" \
                           "          \"name\": \"input byte\"," \
                           "          \"type\": \"918cff9e-8007-4666-99ac-384b9624329c\"," \
                           "          \"typeName\": \"Fxt::Point::Uint8\"," \
                           "          \"idRef\": 2" \
                           "      }" \
                           "    ]," \
                           "  \"outputs\": [" \
                           "      {" \
                           "          \"bit\": 1," \
                           "          \"name\": \"bit1\"," \
                           "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                           "          \"typeName\": \"Fxt::Point::Bool\"," \
                           "          \"idRef\": 3" \
                           "      }," \
                           "      {" \
                           "          \"bit\": 1," \
                           "          \"name\": \"/bit1\"," \
                           "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                           "          \"typeName\": \"Fxt::Point::Bool\"," \
                           "          \"idRef\": 1," \
                           "          \"negate\": true" \
                           "      }," \
                           "      {" \
                           "          \"bit\": 4," \
                           "          \"name\": \"bit4\"," \
                           "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                           "          \"typeName\": \"Fxt::Point::Bool\"," \
                           "          \"idRef\": 0" \
                           "      }," \
                           "      {" \
                           "          \"bit\": 4," \
                           "          \"name\": \"/bit4\"," \
                           "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                           "          \"typeName\": \"Fxt::Point::Bool\"," \
                           "          \"idRef\": 4," \
                           "          \"negate\": true" \
                           "      }," \
                           "      {" \
                           "          \"bit\": 5," \
                           "          \"name\": \"/bit5\"," \
                           "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                           "          \"typeName\": \"Fxt::Point::Bool\"," \
                           "          \"idRef\": 5," \
                           "          \"negate\": true" \
                           "      }" \
                          "    ]" \
                           "  }" \
                           "]}"

static size_t generalHeap_[10000];
static size_t statefulHeap_[10000];

#define MAX_POINTS              6

#define POINT_ID__IN_SIGNAL_1   2

#define POINT_ID__BIT1_OUT      3
#define POINT_ID__BIT1_NEGATED  1
#define POINT_ID__BIT4_OUT      0
#define POINT_ID__BIT4_NEGATED  4
#define POINT_ID__BIT5_NEGATED  5



////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "ByteSplitter" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap statefulAllocator( statefulHeap_, sizeof( statefulHeap_ ) );
    Fxt::Point::Database<MAX_POINTS> pointDb;

    SECTION( "create component" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, COMP_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant componentObj = doc["components"][0];
        ByteSplitter uut( componentObj,
                          generalAllocator,
                          statefulAllocator,
                          pointDb );

        REQUIRE( uut.getErrorCode() == fullErr( Err_T::SUCCESS ) );

        REQUIRE( strcmp( uut.getTypeGuid(), ByteSplitter::GUID_STRING ) == 0 );
        REQUIRE( strcmp( uut.getTypeName(), ByteSplitter::TYPE_NAME ) == 0 );

        REQUIRE( uut.isStarted() == false );
    }

    SECTION( "start component" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, COMP_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant componentObj = doc["components"][0];
        ByteSplitter uut( componentObj,
                          generalAllocator,
                          statefulAllocator,
                          pointDb );

        REQUIRE( uut.getErrorCode() == fullErr( Err_T::SUCCESS ) );

        uint64_t nowUsec = Cpl::System::ElapsedTime::milliseconds() * 1000;

        REQUIRE( uut.start( nowUsec ) == fullErr( Err_T::SUCCESS ) );
        REQUIRE( uut.isStarted() == true );
    }

    SECTION( "execute component - happy path" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, COMP_DEFINTION );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant componentObj = doc["components"][0];
        ByteSplitter uut( componentObj,
                          generalAllocator,
                          statefulAllocator,
                          pointDb );

        REQUIRE( uut.getErrorCode() == fullErr( Err_T::SUCCESS ) );

        Fxt::Point::Uint8* ptIn1  = new(std::nothrow) Fxt::Point::Uint8( pointDb, POINT_ID__IN_SIGNAL_1, "inSig1", statefulAllocator );
        Fxt::Point::Bool* ptOutBit1 = new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT1_OUT, "outBit1", statefulAllocator );
        Fxt::Point::Bool* ptOutBit1_N = new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT1_NEGATED, "/outBit1", statefulAllocator );
        Fxt::Point::Bool* ptOutBit4 = new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT4_OUT, "outBit4", statefulAllocator );
        Fxt::Point::Bool* ptOutBit4_N = new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT4_NEGATED, "/outBit4", statefulAllocator );
        Fxt::Point::Bool* ptOutBit5_N = new(std::nothrow) Fxt::Point::Bool( pointDb, POINT_ID__BIT5_NEGATED, "/outBit5", statefulAllocator );

        REQUIRE( uut.resolveReferences( pointDb ) == fullErr( Err_T::SUCCESS ) );

        uint64_t nowUsec = Cpl::System::ElapsedTime::milliseconds() * 1000;
        REQUIRE( uut.start( nowUsec ) == fullErr( Err_T::SUCCESS ) );
        REQUIRE( uut.isStarted() == true );

        REQUIRE( uut.execute( nowUsec ) == fullErr( Err_T::SUCCESS ) );
        REQUIRE( ptOutBit1->isNotValid() );
        REQUIRE( ptOutBit1_N->isNotValid() );
        REQUIRE( ptOutBit4->isNotValid() );
        REQUIRE( ptOutBit4_N->isNotValid() );
        REQUIRE( ptOutBit5_N->isNotValid() );

        ptIn1->write( 0 );
        REQUIRE( uut.execute( nowUsec ) == fullErr( Err_T::SUCCESS ) );
        bool val;
        REQUIRE( ptOutBit1->read( val ) );
        REQUIRE( val == false );
        REQUIRE( ptOutBit1_N->read( val ) );
        REQUIRE( val == true );
        REQUIRE( ptOutBit4->read( val ) );
        REQUIRE( val == false );
        REQUIRE( ptOutBit4_N->read( val ) );
        REQUIRE( val == true );
        REQUIRE( ptOutBit5_N->read( val ) );
        REQUIRE( val == true );

        ptIn1->write( 0x2 );
        REQUIRE( uut.execute( nowUsec ) == fullErr( Err_T::SUCCESS ) );
        REQUIRE( ptOutBit1->read( val ) );
        REQUIRE( val == true );
        REQUIRE( ptOutBit1_N->read( val ) );
        REQUIRE( val == false );
        REQUIRE( ptOutBit4->read( val ) );
        REQUIRE( val == false );
        REQUIRE( ptOutBit4_N->read( val ) );
        REQUIRE( val == true );
        REQUIRE( ptOutBit5_N->read( val ) );
        REQUIRE( val == true );

        ptIn1->write( 0x12 );
        REQUIRE( uut.execute( nowUsec ) == fullErr( Err_T::SUCCESS ) );
        REQUIRE( ptOutBit1->read( val ) );
        REQUIRE( val == true );
        REQUIRE( ptOutBit1_N->read( val ) );
        REQUIRE( val == false );
        REQUIRE( ptOutBit4->read( val ) );
        REQUIRE( val == true );
        REQUIRE( ptOutBit4_N->read( val ) );
        REQUIRE( val == false );
        REQUIRE( ptOutBit5_N->read( val ) );
        REQUIRE( val == true );

        ptIn1->write( 0x30 );
        REQUIRE( uut.execute( nowUsec ) == fullErr( Err_T::SUCCESS ) );
        REQUIRE( ptOutBit1->read( val ) );
        REQUIRE( val == false );
        REQUIRE( ptOutBit1_N->read( val ) );
        REQUIRE( val == true );
        REQUIRE( ptOutBit4->read( val ) );
        REQUIRE( val == true );
        REQUIRE( ptOutBit4_N->read( val ) );
        REQUIRE( val == false );
        REQUIRE( ptOutBit5_N->read( val ) );
        REQUIRE( val == false );

        ptIn1->write( 0x32 );
        REQUIRE( uut.execute( nowUsec ) == fullErr( Err_T::SUCCESS ) );
        REQUIRE( ptOutBit1->read( val ) );
        REQUIRE( val == true );
        REQUIRE( ptOutBit1_N->read( val ) );
        REQUIRE( val == false );
        REQUIRE( ptOutBit4->read( val ) );
        REQUIRE( val == true );
        REQUIRE( ptOutBit4_N->read( val ) );
        REQUIRE( val == false );
        REQUIRE( ptOutBit5_N->read( val ) );
        REQUIRE( val == false );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}