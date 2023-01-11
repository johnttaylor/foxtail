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
            REQUIRE( uut == Fxt::Type::Error::SUCCESS() );
            buf = Error::toText( uut.errVal, buf );
            CPL_SYSTEM_TRACE_MSG( SECT_, ("toText: [%s]", buf.getString()) );
            REQUIRE( buf == "SUCCESS" );
        }
        {
            Error uut = Fxt::Card::fullErr( Fxt::Card::Err_T::SUCCESS );
            REQUIRE( uut == Fxt::Type::Error::SUCCESS() );
            buf = Error::toText( uut.errVal, buf );
            CPL_SYSTEM_TRACE_MSG( SECT_, ("toText: [%s]", buf.getString()) );
            REQUIRE( buf == "SUCCESS" );
        }
        {
            Error uut = Fxt::Component::fullErr( Fxt::Component::Err_T::SUCCESS );
            REQUIRE( uut == Fxt::Type::Error::SUCCESS() );
            buf = Error::toText( uut.errVal, buf );
            CPL_SYSTEM_TRACE_MSG( SECT_, ("toText: [%s]", buf.getString()) );
            REQUIRE( buf == "SUCCESS" );
        }
    }

    SECTION( "Card Errors" )
    {
        {
            Error uut = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
            buf = Error::toText( uut.errVal, buf );
            CPL_SYSTEM_TRACE_MSG( SECT_, ("toText: [%s]", buf.getString()) );
            REQUIRE( buf == "CARD:BAD_CHANNEL_ASSIGNMENTS" );
        }
        {
            Error uut( (uint32_t) 0x3200 | Fxt::Card::ErrCategory::g_theOne.getCategoryIdentifier() );
            buf = Error::toText( uut.errVal, buf );
            CPL_SYSTEM_TRACE_MSG( SECT_, ("toText: [%s]", buf.getString()) );
            REQUIRE( buf == "CARD:<unknown>" );
        }
    }

    SECTION( "Component Errors" )
    {
        {
            Error uut = Fxt::Component::fullErr( Fxt::Component::Err_T::BAD_OUTPUT_REFERENCE );
            REQUIRE( uut == fullErr( Fxt::Component::Err_T::BAD_OUTPUT_REFERENCE ) );
            buf = Error::toText( uut.errVal, buf );
            CPL_SYSTEM_TRACE_MSG( SECT_, ("toText: [%s]", buf.getString()) );
            REQUIRE( buf == "COMPONENT:BAD_OUTPUT_REFERENCE" );
        }
        {
            Error uut = Fxt::Component::Digital::fullErr( Fxt::Component::Digital::Err_T::SUCCESS );
            REQUIRE( uut == Fxt::Type::Error::SUCCESS() );
            buf = Error::toText( uut.errVal, buf );
            CPL_SYSTEM_TRACE_MSG( SECT_, ("toText: [%s]", buf.getString()) );
            REQUIRE( buf == "SUCCESS" );
        }
        {
            Error uut( (uint32_t) 0x320000 | ((uint32_t)(Fxt::Component::Digital::ErrCategory::g_theOne.getCategoryIdentifier())) << 8 | Fxt::Component::ErrCategory::g_theOne.getCategoryIdentifier() );
            buf = Error::toText( uut.errVal, buf );
            CPL_SYSTEM_TRACE_MSG( SECT_, ("toText: [%s]", buf.getString()) );
            REQUIRE( buf == "COMPONENT:DIGITAL:<unknown>" );
        }
    }

    SECTION( "Digital Errors" )
    {
        {
            Error uut = Fxt::Component::Digital::fullErr( Fxt::Component::Digital::Err_T::DEMUX_INVALID_BIT_OFFSET );
            buf = Error::toText( uut.errVal, buf );
            CPL_SYSTEM_TRACE_MSG( SECT_, ("toText: [%s]", buf.getString()) );
            REQUIRE( buf == "COMPONENT:DIGITAL:DEMUX_INVALID_BIT_OFFSET" );
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