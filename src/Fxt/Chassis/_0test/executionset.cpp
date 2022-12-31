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
#include "Fxt/Chassis/ExecutionSet.h"
#include "Fxt/Point/Database.h"
#include "Fxt/Point/FactoryDatabase.h"
#include "Fxt/Point/Uint8.h"
#include "Fxt/Point/Bool.h"
#include "Fxt/Point/Factory.h"
#include "Fxt/Component/Digital/And16GateFactory.h"
#include "Fxt/Component/Digital/ByteSplitterFactory.h"
#include "Fxt/Component/FactoryDatabase.h"
#include "Cpl/Memory/LeanHeap.h"
#include "Cpl/System/Trace.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Chassis;

#define ID_INPUT_BYTE_SPLITTER          0
#define ID_OUTPUT_AND16GATE             2                   

#define EXESET_DEFINITION   "{" \
                            "\"name\":\"My Execution Set\"," \
                            "\"id\":1," \
                            "\"exeRateMultiplier\" : 2," \
                            "\"logicChains\":[" \
                            "   {\"name\":\"my logic chain\"," \
                            "    \"id\":1," \
                            "    \"components\":[" \
                            "     {" \
                            "     \"id\": 100," \
                            "     \"name\": \"ByteSplitter #1\"," \
                            "     \"type\": \"8c55aa52-3bc8-4b8a-ad73-c434a0bbd4b4\"," \
                            "     \"typeName\": \"Fxt::Component::Digital::ByteSplitter\"," \
                            "     \"inputs\": [" \
                            "         {" \
                            "             \"name\": \"input byte\"," \
                            "             \"type\": \"918cff9e-8007-4666-99ac-384b9624329c\"," \
                            "             \"typeName\": \"Fxt::Point::Uint8\"," \
                            "             \"idRef\": 0" \
                            "         }" \
                            "       ]," \
                            "     \"outputs\": [" \
                            "         {" \
                            "             \"bit\": 4," \
                            "             \"name\": \"/bit4\"," \
                            "             \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                            "             \"typeName\": \"Fxt::Point::Bool\"," \
                            "             \"idRef\": 1" \
                            "         }" \
                            "       ]" \
                            "     }," \
                            "     {" \
                            "     \"id\": 101," \
                            "     \"name\": \"AND Gate#1\"," \
                            "     \"type\": \"e62e395c-d27a-4821-bba9-aa1e6de42a05\"," \
                            "     \"typeName\": \"Fxt::Component::Digital::And16Gate\"," \
                            "     \"inputs\": [" \
                            "         {" \
                            "             \"name\": \"Signal#1\"," \
                            "             \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                            "             \"typeName\": \"Fxt::Point::Bool\"," \
                            "             \"idRef\": 1" \
                            "         }," \
                            "         {" \
                            "             \"name\": \"Signal#2\"," \
                            "             \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                            "             \"typeName\": \"Fxt::Point::Bool\"," \
                            "             \"idRef\": 3" \
                            "         }" \
                            "       ]," \
                            "     \"outputs\": [" \
                            "         {" \
                            "             \"name\": \"out\"," \
                            "             \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                            "             \"typeName\": \"Fxt::Point::Bool\"," \
                            "             \"idRef\": 2" \
                            "         }" \
                            "       ]" \
                            "     }" \
                            "   ]," \
                            "    \"connectionPts\":[" \
                            "        {"\
                            "          \"id\": 0, "\
                            "          \"name\": \"Input to ByteSplitter\"," \
                            "          \"type\": \"918cff9e-8007-4666-99ac-384b9624329c\"," \
                            "          \"typeName\": \"Fxt::Point::Uint8\" " \
                            "         }, "\
                            "        {"\
                            "          \"id\": 1, "\
                            "          \"name\": \"Output ByteSplitter: Bit 4\"," \
                            "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                            "          \"typeName\": \"Fxt::Point::Bool\" " \
                            "         }, "\
                            "        {"\
                            "          \"id\": 2, "\
                            "          \"name\": \"Output AndGate\"," \
                            "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                            "          \"typeName\": \"Fxt::Point::Bool\" " \
                            "         } "\
                            "   ]," \
                            "    \"autoPts\":[" \
                            "        {"\
                            "          \"id\": 3, "\
                            "          \"name\": \"Auto-true\"," \
                            "          \"type\": \"f574ca64-b5f2-41ae-bdbf-d7cb7d52aeb0\"," \
                            "          \"typeName\": \"Fxt::Point::Bool\"," \
                            "              \"initial\": {" \
                            "                 \"val\" : true, " \
                            "                 \"id\" : 4 " \
                            "               } "\
                            "         } "\
                            "   ]" \
                            " }" \
                            "]}"


static size_t generalHeap_[10000];
static size_t haStatefulHeap_[10000];

#define MAX_POINTS      100



////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "LogicChain" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Memory::LeanHeap                               generalAllocator( generalHeap_, sizeof( generalHeap_ ) );
    Cpl::Memory::LeanHeap                               haStatefulAllocator( haStatefulHeap_, sizeof( haStatefulHeap_ ) );
    Fxt::Point::Database<MAX_POINTS>                    pointDb;
    Fxt::Component::FactoryDatabase                     componentFactoryDb;
    Fxt::Point::FactoryDatabase                         pointFactoryDb;
    Fxt::Component::Digital::ByteSplitterFactory        byteSplitterFactory( componentFactoryDb );
    Fxt::Component::Digital::And16GateFactory           and16GateFactory( componentFactoryDb );
    Fxt::Type::Error                                    executionSetError;
    Fxt::Point::Factory<Fxt::Point::Uint8>              factoryUint8( pointFactoryDb );
    Fxt::Point::Factory<Fxt::Point::Bool>               factoryBool( pointFactoryDb );

    Cpl::Text::FString<Fxt::Type::Error::MAX_TEXT_LEN>  buf;



    SECTION( "raw create" )
    {

        ExecutionSetApi* uut = new ExecutionSet( generalAllocator, 1, 3 );
        REQUIRE( uut );
        REQUIRE( uut->getExecutionRateMultiplier() == 3 );
        delete uut;
    }

    SECTION( "json create" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, EXESET_DEFINITION );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("json error=%s", err.c_str()) );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant exeSetJsonObj = doc.as<JsonVariant>();
        ExecutionSetApi* uut = ExecutionSetApi::createExecutionSetfromJSON( exeSetJsonObj,
                                                                            componentFactoryDb,
                                                                            generalAllocator,
                                                                            haStatefulAllocator,
                                                                            pointFactoryDb,
                                                                            pointDb,
                                                                            executionSetError );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("exeSet error=%s", executionSetError.toText( buf )) );
        REQUIRE( uut );
        REQUIRE( uut->getErrorCode() == Fxt::Type::Error::SUCCESS() );
    }

    SECTION( "execute" )
    {
        StaticJsonDocument<10240> doc;
        DeserializationError err = deserializeJson( doc, EXESET_DEFINITION );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("json error=%s", err.c_str()) );
        REQUIRE( err == DeserializationError::Ok );

        JsonVariant exeSetJsonObj = doc.as<JsonVariant>();
        ExecutionSetApi* uut = ExecutionSetApi::createExecutionSetfromJSON( exeSetJsonObj,
                                                                            componentFactoryDb,
                                                                            generalAllocator,
                                                                            haStatefulAllocator,
                                                                            pointFactoryDb,
                                                                            pointDb,
                                                                            executionSetError );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("exeSet error=%s", executionSetError.toText( buf )) );
        REQUIRE( uut );
        REQUIRE( uut->getErrorCode() == Fxt::Type::Error::SUCCESS() );
        REQUIRE( uut->getExecutionRateMultiplier() == 2 );

        executionSetError = uut->resolveReferences( pointDb );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("exeSet error=%s", executionSetError.toText( buf )) );
        REQUIRE( executionSetError == Fxt::Type::Error::SUCCESS() );
        REQUIRE( uut->isStarted() == false );

        uint64_t now = Cpl::System::ElapsedTime::milliseconds();
        executionSetError = uut->start( now );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("exeSet error=%s", executionSetError.toText( buf )) );
        REQUIRE( executionSetError == Fxt::Type::Error::SUCCESS() );
        REQUIRE( uut->isStarted() );

        now += 1000;
        executionSetError = uut->execute( now );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("exeSet error=%s", executionSetError.toText( buf )) );
        REQUIRE( executionSetError == Fxt::Type::Error::SUCCESS() );

        // Output should be invalid - since not all of the inputs are valid
        Fxt::Point::Bool* lcOutput = (Fxt::Point::Bool*) pointDb.lookupById( ID_OUTPUT_AND16GATE );
        REQUIRE( lcOutput );
        REQUIRE( lcOutput->isNotValid() );

        // Execute again with valid inputs (but Bit 4 not set)
        Fxt::Point::Uint8* lcInput = (Fxt::Point::Uint8*) pointDb.lookupById( ID_INPUT_BYTE_SPLITTER );
        REQUIRE( lcInput );
        REQUIRE( lcInput->isNotValid() );
        lcInput->write( 0 );

        now += 1000;
        executionSetError = uut->execute( now );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("exeSet error=%s", executionSetError.toText( buf )) );
        REQUIRE( executionSetError == Fxt::Type::Error::SUCCESS() );

        // Output should be valid - and false
        bool outVal;
        bool valid = lcOutput->read( outVal );
        REQUIRE( valid );
        REQUIRE( outVal == false );

        // Execute again with valid inputs WITH Bit 4 set
        lcInput->write( 0x10 );

        now += 1000;
        executionSetError = uut->execute( now );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("exeSet error=%s", executionSetError.toText( buf )) );
        REQUIRE( executionSetError == Fxt::Type::Error::SUCCESS() );

        // Output should be valid - and true
        valid = lcOutput->read( outVal );
        REQUIRE( valid );
        REQUIRE( outVal == true );

        // Stop
        uut->stop();

        // Destroy the LC
        uut->~ExecutionSetApi();
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}