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
#include "Fxt/Card/Database.h"
#include "Cpl/Memory/LeanHeap.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Card;

/// Mock objects
namespace {

class MockCard : public Api
{
public:
    uint32_t m_id;
    const char* m_guid;
    const char* m_name;

    MockCard( const char* name, uint32_t localId, const char* guid ) :m_id( localId ), m_guid( guid ), m_name( name ) {}
    MockCard( const char* name, uint32_t localId, const char* guid, Database& cardDb ) :m_id( localId ), m_guid( guid ), m_name( name ) { cardDb.insert_( *this ); }

public:
    bool start() noexcept { return true; }
    bool stop() noexcept { return true; }
    bool isStarted() const noexcept { return true; }
    bool scanInputs() noexcept { return true; }
    bool flushOutputs() noexcept { return true; }
    uint32_t getLocalId() const noexcept { return m_id; };
    const char* getName() const noexcept { return m_name; };
    const char* getGuid() const noexcept { return m_guid; };
    const char* getTypeName() const noexcept { return nullptr; };
    uint16_t getSlot() const noexcept { return 0; }
    uint32_t getErrorCode() const noexcept { return 0; }
    const char* getErrorText( uint32_t errCode ) const noexcept { return nullptr; };
};

class MockFactory : public FactoryApi
{
public:
    const char* m_guid;

    MockFactory(const char* guid ) :m_guid( guid ) {}

    MockFactory( const char* guid, Database& factoryDb ) :m_guid( guid ) { factoryDb.insert_( *this ); }

public:
    bool create( JsonVariant&                                                   cardObject,
                 Banks_T&                                                       pointBanks,
                 PointAllocators_T&                                             pointAllocators,
                 Fxt::Point::Database&                                          pointDatabase,
                 Cpl::Container::Dictionary<Cpl::Container::KeyUinteger32_T>&   descriptorDatabase,
                 Cpl::Memory::ContiguousAllocator&                              allocator ) noexcept
    {
        return true;
    }

    void destroy( Api& cardToDestory ) noexcept {}
    const char* getGuid() const noexcept { return m_guid; }
};

}; // end anonymous namespace

//// Static UUT
//static Database uut_( "ignoreThisParameter_usedToCreateAUniqueConstructor" );
//
//// Static Cards an factories
#define APPLE_GUID  "9f88d3a1-6b10-4fd5-865f-aaeef1cc2060"
#define APPLE1_ID   1
#define APPLE2_ID   2
//static MockCard     cardApple1_( "apple", APPLE1_ID, APPLE_GUID, uut_ );
//static MockFactory  factoryApple_( APPLE_GUID, uut_ );
//
#define ORANGE_GUID "fabd0cfd-dfe7-49ca-9e3a-e188dd7a4be6"
#define ORANGE_ID   3
//static MockCard     cardOrange_( "orange", ORANGE_ID, ORANGE_GUID, uut_ );
//static MockFactory  factoryOrange_( ORANGE_ID, uut_ );
//
#define CHERRY_GUID "fabd0cfd-dfe7-49ca-9e3a-e188dd7a4be6"
#define CHERRY_ID   4
//static MockCard     cardCherry_( "cherry", CHERRY_ID, CHERRY_GUID, uut_ );
//static MockFactory  factoryCherry_( CHERRY_GUID, uut_ );


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Database" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    MockCard     cardApple1( "apple", APPLE1_ID, APPLE_GUID );
    MockCard     cardApple2( "apple", APPLE2_ID, APPLE_GUID );
    MockFactory  factoryApple( APPLE_GUID );

    MockCard     cardOrange( "orange", ORANGE_ID, ORANGE_GUID );
    MockFactory  factoryOrange( APPLE_GUID );

    MockCard     cardCherry( "cherry", CHERRY_ID, CHERRY_GUID );
    MockFactory  factoryCherry( CHERRY_GUID );

    SECTION( "cards" )
    {
        Database uut;

        // Empty DB
        REQUIRE( uut.lookupCard( APPLE1_ID ) == 0 );
        REQUIRE( uut.getFirstCard() == 0 );

        // Populate the DB
        uut.insert_( cardApple1 );
        uut.insert_( cardApple2 );
        uut.insert_( cardOrange );
        uut.insert_( cardCherry );

        REQUIRE( uut.lookupCard( APPLE1_ID ) == &cardApple1 );
        Api* item =  uut.getFirstCard();
        REQUIRE( item == &cardApple1 );
        item =  uut.getNextCard( *item );
        REQUIRE( item == &cardApple2 );
        item =  uut.getNextCard( *item );
        REQUIRE( item == &cardOrange );
        item =  uut.getNextCard( *item );
        REQUIRE( item == &cardCherry );
        item =  uut.getNextCard( *item );
        REQUIRE( item == 0 );

        // Remove some cards
        uut.remove_( cardApple1 );
        uut.remove_( cardOrange );
        REQUIRE( uut.lookupCard( APPLE2_ID ) == &cardApple2 );
        item =  uut.getFirstCard();
        REQUIRE( item == &cardApple2 );
        item =  uut.getNextCard( *item );
        REQUIRE( item == &cardCherry );
        item =  uut.getNextCard( *item );
        REQUIRE( item == 0 );

        // Add some cards
        uut.insert_( cardApple1 );
        uut.insert_( cardOrange );
        REQUIRE( uut.lookupCard( APPLE1_ID ) == &cardApple1 );
        item =  uut.getFirstCard();
        REQUIRE( item == &cardApple2 );
        item =  uut.getNextCard( *item );
        REQUIRE( item == &cardCherry );
        item =  uut.getNextCard( *item );
        REQUIRE( item == &cardApple1 );
        item =  uut.getNextCard( *item );
        REQUIRE( item == &cardOrange );
        item =  uut.getNextCard( *item );
        REQUIRE( item == 0 );
    }

    SECTION( "factories" )
    {
        Database uut;

        // Empty DB
        REQUIRE( uut.lookupFactory( APPLE_GUID ) == 0 );
        REQUIRE( uut.getFirstFactory() == 0 );

        // Populate the DB
        uut.insert_( factoryApple );
        uut.insert_( factoryOrange );
        uut.insert_( factoryCherry );

        REQUIRE( uut.lookupFactory( APPLE_GUID ) == &factoryApple );
        FactoryApi* item =  uut.getFirstFactory();
        REQUIRE( item == &factoryApple );
        item =  uut.getNextFactory( *item );
        REQUIRE( item == &factoryOrange );
        item =  uut.getNextFactory( *item );
        REQUIRE( item == &factoryCherry );
        item =  uut.getNextFactory( *item );
        REQUIRE( item == 0 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}