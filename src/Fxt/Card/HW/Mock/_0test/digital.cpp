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
#include "Fxt/Card/HW/Mock/Digital.h"
#include "Fxt/Point/Bank.h"
#include "Fxt/Point/Database.h"
#include "Cpl/Memory/LeanHeap.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Fxt::Card::HW::Mock;

///
static Fxt::Point::Bank inpInternalBank_;
static Fxt::Point::Bank inpRegisterBank_;
static Fxt::Point::Bank inpVirtualBank_;
static Fxt::Point::Bank outInternalBank_;
static Fxt::Point::Bank outRegisterBank_;
static Fxt::Point::Bank outVirtualBank_;
static Fxt::Card::Banks_T banks ={ &inpInternalBank_, &inpRegisterBank_, &inpVirtualBank_,
                                    &outInternalBank_, &outRegisterBank_, &outVirtualBank_ };

#define MAX_POINT_ALLOCATOR_SIZE_BYTES  (sizeof(Cpl::Point::Bool) * 32)
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
static Fxt::Card::PointAllocators_T allocators_ ={ &inpInternalAlloc_, &inpRegisterAlloc_, &inpVirtualAlloc_,
                                                   &outInternalAlloc_, &outRegisterAlloc_, &outVirtualAlloc_ };

static Cpl::Container::DList<Cpl::Container::DictItem>    buckets_10[10];
static Cpl::Container::Dictionary<Fxt::Point::Descriptor> descriptorDictionary_( buckets_10, 10 );

static size_t                generalMemoryAlloc_[(10 * 1024) / sizeof( size_t )];
static Cpl::Memory::LeanHeap generalAlloc_( generalMemoryAlloc_, sizeof( generalMemoryAlloc_ ) );

static StaticJsonDocument<4 * 1024>   jsonDoc_;

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Database" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();


    SECTION( "create" )
    {
        const char* json =
            "{"
            "  \"guid\": \"59d33888-62c7-45b2-a4d4-9dbc55914ed3\","
            "  \"slot\" : 0,                                      "
            "  \"localId\" : 0,                                   "
            "  \"name\" : \"My Digital Card\",                    "
            "  \"initialInputValueMask\" : 0,                     "
            "  \"initialOutputValueMask\" : 0,                    "
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
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}