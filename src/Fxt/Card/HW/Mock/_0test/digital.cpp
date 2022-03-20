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

// Anonymous namespace
namespace {
class MyUut : public Fxt::Card::HW::Mock::Digital
{
public:
    MyUut( JsonVariant&                                          cardObject,
           Fxt::Card::PointAllocators_T&                         pointAllocators,
           Fxt::Point::Database&                                 pointDatabase,
           Cpl::Container::Dictionary<Fxt::Point::Descriptor>&   descriptorDatabase,
           Cpl::Memory::ContiguousAllocator&                     allocator )
        :Fxt::Card::HW::Mock::Digital( cardObject, pointAllocators, pointDatabase, descriptorDatabase, allocator )
    {
    }


    // Get card input point
    Cpl::Point::Bool* getInput( uint16_t channelIndex )
    {
        return getPointByChannel( m_inputChannels, channelIndex );
    }

    // Get card output point
    Cpl::Point::Bool* getOutput( uint16_t channelIndex )
    {
        return getPointByChannel( m_outputChannels, channelIndex );
    }

    // Update Input IO Registers 
    bool updateInputIORegisters() noexcept
    {
        return updateInputRegisters();
    }

    /// Read from the Output IO Registers
    bool readOuputIORegisters() noexcept
    {
        return readOutputRegisters();
    }

};

}; // end anonymous namespace

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Digital" )
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

    const char* json =
        "{"
        "  \"type\": \"Fxt::Card::HW::Mock::Digital\",        "
        "  \"guid\": \"59d33888-62c7-45b2-a4d4-9dbc55914ed3\","
        "  \"name\" : \"My Digital Card\",                    "
        "  \"slot\" : 1,                                      "
        "  \"cardId\" : 10,                                   "
        "  \"name\" : \"My Digital Card\",                    "
        "  \"initialInputValueMask\" : 5,                     "
        "  \"points\" : {                                     "
        "    \"inputs\": [                                    "
        "      {                                              "
        "        \"type\": \"Fxt::Point::Bool\",              "
        "        \"channel\": 1,                              "
        "        \"pointId\" : 0,                             "
        "        \"name\" : \"Input 1\"                       "
        "      },                                             "
        "      {                                              "
        "        \"type\": \"Fxt::Point::Bool\",              "
        "        \"channel\": 0,                              "
        "        \"pointId\" : 1,                             "
        "        \"name\" : \"Input 0\"                       "
        "      },                                             "
        "      {                                              "
        "        \"type\": \"Fxt::Point::Bool\",              "
        "        \"channel\": 2,                              "
        "        \"pointId\" : 2,                             "
        "        \"name\" : \"Input 2\"                       "
        "      },                                             "
        "      {                                              "
        "        \"type\": \"Fxt::Point::Bool\",              "
        "        \"channel\": 3,                              "
        "        \"pointId\" : 5,                             "
        "        \"name\" : \"Input 3\"                       "
        "      }                                              "
        "    ] ,                                              "
        "    \"outputs\": [                                   "
        "      {                                              "
        "        \"type\": \"Fxt::Point::Bool\",              "
        "        \"channel\": 1,                              "
        "        \"pointId\" : 3,                             "
        "        \"name\" : \"Output 1\"                      "
        "      },                                             "
        "      {                                              "
        "        \"type\": \"Fxt::Point::Bool\",              "
        "        \"channel\": 0,                              "
        "        \"pointId\" : 4,                             "
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

    SECTION( "start/stop" )
    {
        REQUIRE( card->start() );
        REQUIRE( card->start() == false );
        REQUIRE( card->isStarted() );
        REQUIRE( card->stop() );
        REQUIRE( card->isStarted() == false );
        REQUIRE( card->stop() == false );
    }

    SECTION( "inputs" )
    {
        REQUIRE( card->start() );

        // Get virtual input point DESCRIPTORS: (channel, value, localId):  0,1,1  1,0,0  2,1,2  3,0,5
        Cpl::Container::KeyUinteger32_T key( 1 );
        Fxt::Point::Descriptor* vin1 = descriptorDictionary_.find( key );
        REQUIRE( vin1 != nullptr );
        key.setValue( 0 );
        Fxt::Point::Descriptor* vin0 = descriptorDictionary_.find( key );
        REQUIRE( vin0 != nullptr );
        key.setValue( 2 );
        Fxt::Point::Descriptor* vin2 = descriptorDictionary_.find( key );
        REQUIRE( vin2 != nullptr );
        key.setValue( 5 );
        Fxt::Point::Descriptor* vin5 = descriptorDictionary_.find( key );
        REQUIRE( vin5 != nullptr );
        key.setValue( 5555 );
        REQUIRE( descriptorDictionary_.find( key ) == nullptr );

        REQUIRE( vin1->getPoint()->isNotValid() );
        REQUIRE( strcmp( vin1->getSymbolicName(), "Input 0" ) == 0 );
        REQUIRE( vin0->getPoint()->isNotValid() );
        REQUIRE( strcmp( vin0->getSymbolicName(), "Input 1" ) == 0 );
        REQUIRE( vin2->getPoint()->isNotValid() );
        REQUIRE( strcmp( vin2->getSymbolicName(), "Input 2" ) == 0 );
        REQUIRE( vin5->getPoint()->isNotValid() );
        REQUIRE( strcmp( vin5->getSymbolicName(), "Input 3" ) == 0 );

        MyUut* uut = (MyUut*) card;
        bool val;
        REQUIRE( uut->getInput( 0 )->read( val ) );
        REQUIRE( val == true );
        REQUIRE( uut->getInput( 1 )->read( val ) );
        REQUIRE( val == false );
        REQUIRE( uut->getInput( 2 )->read( val ) );
        REQUIRE( val == true );
        REQUIRE( uut->getInput( 3 )->read( val ) );
        REQUIRE( val == false );

        Cpl::Point::Bool* pt1 = (Cpl::Point::Bool*) vin1->getPoint();
        REQUIRE( pt1->isNotValid() );
        Cpl::Point::Bool* pt0 = (Cpl::Point::Bool*) vin0->getPoint();
        REQUIRE( pt0->isNotValid() );
        Cpl::Point::Bool* pt2= (Cpl::Point::Bool*) vin2->getPoint();
        REQUIRE( pt2->isNotValid() );
        Cpl::Point::Bool* pt5= (Cpl::Point::Bool*) vin5->getPoint();
        REQUIRE( pt5->isNotValid() );

        // Update virtual input points (from the card)
        REQUIRE( uut->updateInputIORegisters() );
        REQUIRE( card->scanInputs() );
        REQUIRE( pt1->read( val ) );
        REQUIRE( val == true );
        REQUIRE( pt0->read( val ) );
        REQUIRE( val == false );
        REQUIRE( pt2->read( val ) );
        REQUIRE( val == true );
        REQUIRE( pt5->read( val ) );
        REQUIRE( val == false );

        // Set all value to ones
        uut->setInputs( 10 );
        REQUIRE( card->scanInputs() );
        REQUIRE( pt1->read( val ) );
        REQUIRE( val == true );
        REQUIRE( pt0->read( val ) );
        REQUIRE( val == true );
        REQUIRE( pt2->read( val ) );
        REQUIRE( val == true );
        REQUIRE( pt5->read( val ) );
        REQUIRE( val == true );

        // Set all value to zero
        uut->toggleInputs( 0xF );
        REQUIRE( card->scanInputs() );
        REQUIRE( pt1->read( val ) );
        REQUIRE( val == false );
        REQUIRE( pt0->read( val ) );
        REQUIRE( val == false );
        REQUIRE( pt2->read( val ) );
        REQUIRE( val == false );
        REQUIRE( pt5->read( val ) );
        REQUIRE( val == false );

        // Set bits 3 and 1
        uut->toggleInputs( 10 );
        REQUIRE( card->scanInputs() );
        REQUIRE( pt1->read( val ) );
        REQUIRE( val == false );
        REQUIRE( pt0->read( val ) );
        REQUIRE( val == true );
        REQUIRE( pt2->read( val ) );
        REQUIRE( val == false );
        REQUIRE( pt5->read( val ) );
        REQUIRE( val == true );

        // Set all value to zero
        uut->toggleInputs( 10 );
        REQUIRE( card->scanInputs() );
        REQUIRE( pt1->read( val ) );
        REQUIRE( val == false );
        REQUIRE( pt0->read( val ) );
        REQUIRE( val == false );
        REQUIRE( pt2->read( val ) );
        REQUIRE( val == false );
        REQUIRE( pt5->read( val ) );
        REQUIRE( val == false );

        REQUIRE( card->stop() );
    }

    SECTION( "outputs" )
    {
        REQUIRE( card->start() );

        // Get virtual output point DESCRIPTORS: (channel, localId):  1,3  0,4
        Cpl::Container::KeyUinteger32_T key( 3 );
        Fxt::Point::Descriptor* vout3 = descriptorDictionary_.find( key );
        REQUIRE( vout3 != nullptr );
        key.setValue( 4 );
        Fxt::Point::Descriptor* vout4 = descriptorDictionary_.find( key );
        REQUIRE( vout4 != nullptr );

        REQUIRE( vout3->getPoint()->isNotValid() );
        REQUIRE( strcmp( vout3->getSymbolicName(), "Output 1" ) == 0 );
        REQUIRE( vout4->getPoint()->isNotValid() );
        REQUIRE( strcmp( vout4->getSymbolicName(), "Output 0" ) == 0 );

        // Get output points
        Cpl::Point::Bool* pt1 = (Cpl::Point::Bool*) vout3->getPoint();
        REQUIRE( pt1->isNotValid() );
        Cpl::Point::Bool* pt0 = (Cpl::Point::Bool*) vout4->getPoint();
        REQUIRE( pt0->isNotValid() );

        // Get the card's internal output points
        MyUut* uut = (MyUut*) card;
        REQUIRE( uut->getOutput( 0 )->isNotValid() );
        REQUIRE( uut->getOutput( 1 )->isNotValid() );
    
        // 'Application' update of the outputs
        pt1->write( true );
        REQUIRE( card->flushOutputs() );
        uint32_t maskValue;
        REQUIRE( uut->getOutputs( maskValue ) == false );
        bool   val;
        REQUIRE( uut->getOutput( 0 )->isNotValid() );
        REQUIRE( uut->getOutput( 1 )->read( val ) );
        REQUIRE( val == true );

        // 'Application' update of the outputs
        pt1->write( false );
        pt0->write( true );
        REQUIRE( card->flushOutputs() );
        REQUIRE( uut->getOutputs( maskValue ) );
        REQUIRE( maskValue == 1 );
        REQUIRE( uut->getOutput( 0 )->read( val ) );
        REQUIRE( val == true );
        REQUIRE( uut->getOutput( 1 )->read( val ) );
        REQUIRE( val == false );

        REQUIRE( card->stop() );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}