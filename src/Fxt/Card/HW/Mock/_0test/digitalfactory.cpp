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
#include "Fxt/Card/Api.h"
#include "Fxt/Card/HW/Mock/Digital.h"
#include "Fxt/Card/HW/Mock/DigitalFactory.h"
#include "Fxt/Point/Bank.h"
#include "Fxt/Point/Database.h"
#include "Cpl/Memory/LeanHeap.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Card::HW::Mock;

#define MAX_CHANNELS        32

#define MAX_POINT_ALLOCATOR_SIZE_BYTES  (sizeof(Cpl::Point::Bool) * MAX_CHANNELS)
static size_t                inpMemoryInternalAlloc_[MAX_POINT_ALLOCATOR_SIZE_BYTES / sizeof( size_t )];
static Cpl::Memory::LeanHeap inpInternalAlloc_( inpMemoryInternalAlloc_, sizeof( inpMemoryInternalAlloc_ ) );
static size_t                inpMemoryRegisterAlloc_[MAX_POINT_ALLOCATOR_SIZE_BYTES / sizeof( size_t )];
static Cpl::Memory::LeanHeap inpRegisterAlloc_( inpMemoryRegisterAlloc_, sizeof( inpMemoryRegisterAlloc_ ) );
static size_t                inpMemoryVirtualAlloc_[MAX_POINT_ALLOCATOR_SIZE_BYTES / sizeof( size_t )];
static Cpl::Memory::LeanHeap inpVirtualAlloc_( inpMemoryVirtualAlloc_, sizeof( inpMemoryVirtualAlloc_ ) );
static size_t                outMemoryInternalAlloc_[MAX_POINT_ALLOCATOR_SIZE_BYTES / sizeof( size_t )];
static Cpl::Memory::LeanHeap outInternalAlloc_( outMemoryInternalAlloc_, sizeof( outMemoryInternalAlloc_ ) );
static size_t                outMemoryRegisterAlloc_[MAX_POINT_ALLOCATOR_SIZE_BYTES / sizeof( size_t )];
static Cpl::Memory::LeanHeap outRegisterAlloc_( outMemoryRegisterAlloc_, sizeof( outMemoryRegisterAlloc_ ) );
static size_t                outMemoryVirtualAlloc_[MAX_POINT_ALLOCATOR_SIZE_BYTES / sizeof( size_t )];
static Cpl::Memory::LeanHeap outVirtualAlloc_( outMemoryVirtualAlloc_, sizeof( outMemoryVirtualAlloc_ ) );
static Fxt::Card::PointAllocators_T pointAllocators_ ={ &inpInternalAlloc_, &inpRegisterAlloc_, &inpVirtualAlloc_,
                                                        &outInternalAlloc_, &outRegisterAlloc_, &outVirtualAlloc_ };

static Cpl::Container::DList<Cpl::Container::DictItem>    buckets_10[10];
static Cpl::Container::Dictionary<Fxt::Point::Descriptor> descriptorDictionary_( buckets_10, 10 );

static Fxt::Point::Database pointDb_( MAX_CHANNELS * 6 );

static Fxt::Card::Database  cardDb_( "ignoreThisParameter_usedToCreateAUniqueConstructor" );

static size_t                generalMemoryAlloc_[(10 * 1024) / sizeof( size_t )];
static Cpl::Memory::LeanHeap generalAlloc_( generalMemoryAlloc_, sizeof( generalMemoryAlloc_ ) );

static StaticJsonDocument<4 * 1024>   jsonDoc_;

static DigitalFactory factoryUut( cardDb_, descriptorDictionary_, pointDb_, pointAllocators_, generalAlloc_ );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "DigitalFactory" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Ensure all of the allocators start with all of their memory available
    generalAlloc_.reset();
    inpInternalAlloc_.reset();
    inpRegisterAlloc_.reset();
    inpVirtualAlloc_.reset();
    outInternalAlloc_.reset();
    outRegisterAlloc_.reset();
    outVirtualAlloc_.reset();

    SECTION( "create" )
    {
        const char* json =
            "{"
            "  \"guid\": \"59d33888-62c7-45b2-a4d4-9dbc55914ed3\","
            "  \"slot\" : 1,                                    "
            "  \"localId\" : 10,                                  "
            "  \"name\" : \"My Digital Card\",                    "
            "  \"initialInputValueMask\" : 0,                     "
            "  \"points\" : {                                     "
            "    \"inputs\": [                                    "
            "      {                                              "
            "        \"channel\": 1,                              "
            "        \"localId\" : 0,                             "
            "        \"name\" : \"Input 1\"                       "
            "      },                                             "
            "      {                                              "
            "        \"channel\": 0,                              "
            "        \"localId\" : 1,                             "
            "        \"name\" : \"Input 0\"                       "
            "      },                                             "
            "      {                                              "
            "        \"channel\": 2,                              "
            "        \"localId\" : 2,                             "
            "        \"name\" : \"Input 2\"                       "
            "      },                                             "
            "      {                                              "
            "        \"channel\": 3,                              "
            "        \"localId\" : 5,                             "
            "        \"name\" : \"Input 2\"                       "
            "      }                                              "
            "    ] ,                                              "
            "    \"outputs\": [                                   "
            "      {                                              "
            "        \"channel\": 1,                              "
            "        \"localId\" : 3,                             "
            "        \"name\" : \"Output 1\"                      "
            "      },                                             "
            "      {                                              "
            "        \"channel\": 0,                              "
            "        \"localId\" : 4,                             "
            "        \"name\" : \"Output 0\"                      "
            "      }                                              "
            "    ]                                                "
            "  }                                                  "
            "}";

        auto err = deserializeJson( jsonDoc_, json );
        REQUIRE( err == DeserializationError::Ok );
        const char* guid = jsonDoc_["guid"];
        REQUIRE( guid != nullptr );

        Fxt::Card::FactoryApi* factory = cardDb_.lookupFactory( guid );
        REQUIRE( factory != nullptr );

        JsonVariant rootObject = jsonDoc_.as<JsonVariant>();
        bool result = factory->create( rootObject );
        REQUIRE( result );

        // Get Card info
        uint32_t localid = jsonDoc_["localId"] | ((uint32_t) (-1));
        REQUIRE( localid == 10 );
        Fxt::Card::Api* card = cardDb_.lookupCard( localid );
        REQUIRE( card != nullptr );
        REQUIRE( card->getSlot() == 1 );
        REQUIRE( strcmp( card->getName(), "My Digital Card" ) == 0 );
        REQUIRE( strcmp( card->getGuid(), Fxt::Card::HW::Mock::Digital::GUID_STRING ) == 0 );
        REQUIRE( strcmp( card->getTypeName(), Fxt::Card::HW::Mock::Digital::TYPE_NAME ) == 0 );
        REQUIRE( card->getErrorCode() == Fxt::Card::Api::ERR_NO_ERROR );
        REQUIRE( card->getErrorText( card->getErrorCode() ) == nullptr );
        REQUIRE( card->start() );
        REQUIRE( card->stop() );

    }

    SECTION( "create-missing-guid" )
    {
        const char* json =
            "{"
            "  \"slot\" : 1,                                    "
            "  \"localId\" : 10,                                  "
            "  \"name\" : \"My Digital Card\",                    "
            "  \"initialInputValueMask\" : 0                      "
            "}";

        auto err = deserializeJson( jsonDoc_, json );
        REQUIRE( err == DeserializationError::Ok );
        const char* guid = jsonDoc_["guid"];
        REQUIRE( guid == nullptr );
    }

    SECTION( "create-missing-slot" )
    {
        const char* json =
            "{"
            "  \"guid\": \"59d33888-62c7-45b2-a4d4-9dbc55914ed3\","
            "  \"localId\" : 10,                                  "
            "  \"name\" : \"My Digital Card\",                    "
            "  \"initialInputValueMask\" : 0                      "
            "}";

        auto err = deserializeJson( jsonDoc_, json );
        REQUIRE( err == DeserializationError::Ok );
        const char* guid = jsonDoc_["guid"];
        REQUIRE( guid != nullptr );

        Fxt::Card::FactoryApi* factory = cardDb_.lookupFactory( guid );
        REQUIRE( factory != nullptr );

        JsonVariant rootObject = jsonDoc_.as<JsonVariant>();
        bool result = factory->create( rootObject );
        REQUIRE( result );
        uint32_t localid = jsonDoc_["localId"] | ((uint32_t) (-1));
        Fxt::Card::Api* card = cardDb_.lookupCard( localid );
        REQUIRE( card->getErrorCode() == Fxt::Card::Api::ERR_CARD_MISSING_SLOT_ID );
        printf( "error=[%s]\n", card->getErrorText( Fxt::Card::Api::ERR_CARD_MISSING_SLOT_ID) );

        REQUIRE( card->start() == false );
        REQUIRE( card->isStarted() == false );
        REQUIRE( card->stop() == false );
    }

    SECTION( "create-missing-name" )
    {
        const char* json =
            "{"
            "  \"slot\" : 1,                                    "
            "  \"guid\": \"59d33888-62c7-45b2-a4d4-9dbc55914ed3\","
            "  \"localId\" : 10,                                  "
            "  \"initialInputValueMask\" : 0                      "
            "}";

        auto err = deserializeJson( jsonDoc_, json );
        REQUIRE( err == DeserializationError::Ok );
        const char* guid = jsonDoc_["guid"];
        REQUIRE( guid != nullptr );

        Fxt::Card::FactoryApi* factory = cardDb_.lookupFactory( guid );
        REQUIRE( factory != nullptr );

        JsonVariant rootObject = jsonDoc_.as<JsonVariant>();
        bool result = factory->create( rootObject );
        REQUIRE( result );
        uint32_t localid = jsonDoc_["localId"] | ((uint32_t) (-1));
        Fxt::Card::Api* card = cardDb_.lookupCard( localid );
        REQUIRE( card->getErrorCode() == Fxt::Card::Api::ERR_CARD_MISSING_NAME );
        printf( "error=[%s]\n", card->getErrorText( Fxt::Card::Api::ERR_CARD_MISSING_NAME ) );
    }

    SECTION( "create-missing-localid" )
    {
        const char* json =
            "{"
            "  \"guid\": \"59d33888-62c7-45b2-a4d4-9dbc55914ed3\","
            "  \"initialInputValueMask\" : 0                      "
            "}";

        auto err = deserializeJson( jsonDoc_, json );
        REQUIRE( err == DeserializationError::Ok );
        const char* guid = jsonDoc_["guid"];
        REQUIRE( guid != nullptr );

        Fxt::Card::FactoryApi* factory = cardDb_.lookupFactory( guid );
        REQUIRE( factory != nullptr );

        JsonVariant rootObject = jsonDoc_.as<JsonVariant>();
        bool result = factory->create( rootObject );
        REQUIRE( result );
        Fxt::Card::Api* card = cardDb_.getFirstCard();
        REQUIRE( card->getErrorCode() == Fxt::Card::Api::ERR_CARD_MISSING_LOCAL_ID );
        printf( "error=[%s]\n", card->getErrorText( Fxt::Card::Api::ERR_CARD_MISSING_LOCAL_ID ) );
    }

    SECTION( "create-bad-guid" )
    {
        const char* json =
            "{"
            "  \"guid\": \"59d33888-0000-0000-0000-9dbc55914ed3\","
            "  \"localId\" : 10,                                  "
            "  \"name\" : \"My Digital Card\",                    "
            "  \"initialInputValueMask\" : 0                      "
            "}";

        auto err = deserializeJson( jsonDoc_, json );
        REQUIRE( err == DeserializationError::Ok );
        const char* guid = jsonDoc_["guid"];
        REQUIRE( guid != nullptr );

        Fxt::Card::FactoryApi* factory = cardDb_.lookupFactory( guid );
        REQUIRE( factory == nullptr );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}