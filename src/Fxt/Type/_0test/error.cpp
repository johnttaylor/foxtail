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
#include "Fxt/Type/Error.h"
#include "Fxt/Card/Error.h"
#include "Fxt/Card/Mock/Error.h"
#include "Fxt/Component/Error.h"
#include "Fxt/Component/Digital/Error.h"
#include "Cpl/Text/FString.h"
#include "Cpl/System/Trace.h"

#define SECT_   "_0test"

/// 
using namespace Fxt::Type;



////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Error" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Text::FString<Error::MAX_TEXT_LEN> buf;

    SECTION( "Root Errors" )
    {
        {
            Error uut;
            REQUIRE( uut.errVal == 0 );
            REQUIRE( uut == fullErr( Err_T::SUCCESS ) );
            buf = uut.toText( buf );
            CPL_SYSTEM_TRACE_MSG( SECT_, ("toText: [%s]", buf.getString()) );
            REQUIRE( buf == "SUCCESS" );
        }
        {
            Error uut( Err_T::CARD );
            REQUIRE( uut.errVal == Err_T::CARD );
            REQUIRE( uut == fullErr( Err_T::CARD ) );
            buf = uut.toText( buf );
            CPL_SYSTEM_TRACE_MSG( SECT_, ("toText: [%s]", buf.getString()) );
            REQUIRE( buf == "CARD:SUCCESS" );
        }
        {
            Error uut( Err_T::COMPONENT );
            REQUIRE( uut.errVal == Err_T::COMPONENT );
            REQUIRE( uut == fullErr( Err_T::COMPONENT ) );
            buf = uut.toText( buf );
            CPL_SYSTEM_TRACE_MSG( SECT_, ("toText: [%s]", buf.getString()) );
            REQUIRE( buf == "COMPONENT:SUCCESS" );
        }
    }

    SECTION( "Card Errors" )
    {
        {
            Error uut = fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
            REQUIRE( uut == fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS ) );
            buf = uut.toText( buf );
            CPL_SYSTEM_TRACE_MSG( SECT_, ("toText: [%s]", buf.getString()) );
            REQUIRE( buf == "CARD:BAD_CHANNEL_ASSIGNMENTS" );
        }
        {
            Error uut = fullErr( Fxt::Card::Err_T::MOCK );;
            REQUIRE( uut == fullErr( Fxt::Card::Err_T::MOCK ) );
            buf = uut.toText( buf );
            CPL_SYSTEM_TRACE_MSG( SECT_, ("toText: [%s]", buf.getString()) );
            REQUIRE( buf == "CARD:MOCK:<unknown>" );
        }
    }

    SECTION( "Component Errors" )
    {
        {
            Error uut = fullErr( Fxt::Component::Err_T::BAD_OUTPUT_REFERENCE );
            REQUIRE( uut == fullErr( Fxt::Component::Err_T::BAD_OUTPUT_REFERENCE ) );
            buf = uut.toText( buf );
            CPL_SYSTEM_TRACE_MSG( SECT_, ("toText: [%s]", buf.getString()) );
            REQUIRE( buf == "COMPONENT:BAD_OUTPUT_REFERENCE" );
        }
        {
            Error uut = fullErr( Fxt::Component::Err_T::DIGITAL );;
            REQUIRE( uut == fullErr( Fxt::Component::Err_T::DIGITAL ) );
            buf = uut.toText( buf );
            CPL_SYSTEM_TRACE_MSG( SECT_, ("toText: [%s]", buf.getString()) );
            REQUIRE( buf == "COMPONENT:DIGITAL:SUCCESS" );
        }
    }

    SECTION( "Digital Errors" )
    {
        {
            Error uut = fullErr( Fxt::Component::Digital::Err_T::SPLITTER_INVALID_BIT_OFFSET );
            REQUIRE( uut == fullErr( Fxt::Component::Digital::Err_T::SPLITTER_INVALID_BIT_OFFSET ) );
            buf = uut.toText( buf );
            CPL_SYSTEM_TRACE_MSG( SECT_, ("toText: [%s]", buf.getString()) );
            REQUIRE( buf == "COMPONENT:DIGITAL:SPLITTER_INVALID_BIT_OFFSET" );
        }
    }


    SECTION( "print all" )
    {
        size_t count = Error::printAllErrorCodes();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Total Error Count=%lu", count) );
        REQUIRE( count != 0 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}