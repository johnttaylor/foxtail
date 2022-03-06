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
/** @file */


#include "Digital.h"
#include "Cpl/System/Assert.h"
#include <stdint.h>
#include <new>

///
using namespace Fxt::Card::HW::Mock;

static const char emptyString_[1] ={ '\0' };

///////////////////////////////////////////////////////////////////////////////
Digital::Digital( JsonVariant&                                          cardObject,
                  PointAllocators_T&                                    pointAllocators,
                  Fxt::Point::Database&                                 pointDatabase,
                  Cpl::Container::Dictionary<Fxt::Point::Descriptor>&   descriptorDatabase,
                  Cpl::Memory::ContiguousAllocator&                     generalAllocator )
    : Fxt::Card::Common_( generalAllocator )
    , m_allocator( generalAllocator )
    , m_numInputs( 0 )
    , m_numOutputs( 0 )
    , m_initInputVals( 0 )
{
    CPL_SYSTEM_ASSERT( pointAllocators.internalInputs );
    CPL_SYSTEM_ASSERT( pointAllocators.registerInputs );
    CPL_SYSTEM_ASSERT( pointAllocators.virtualInputs );
    CPL_SYSTEM_ASSERT( pointAllocators.internalOutputs );
    CPL_SYSTEM_ASSERT( pointAllocators.registerOutputs );
    CPL_SYSTEM_ASSERT( pointAllocators.virtualOutputs );

    memset( &m_inDescriptors, 0, sizeof( m_inDescriptors ) );
    memset( &m_outDescriptors, 0, sizeof( m_outDescriptors ) );
    memset( &m_inputChannels, 0xFF, sizeof( m_inputChannels ) );
    memset( &m_outputChannels, 0xFF, sizeof( m_outputChannels ) );

    parseConfiguration( cardObject );
    createPoints( descriptorDatabase, pointDatabase, pointAllocators );
}

Digital::~Digital()
{
    // NOTE: Call the destructors on ALL dynamically allocated objects - but 
    //       the Application is responsible for freeing the actual memory

    // Destroy any created Descriptors
    for ( int i=0; i < MAX_CHANNELS; i++ )
    {
        if ( m_inDescriptors[i] != nullptr )
        {
            m_inDescriptors[i]->~Descriptor();
        }
        if ( m_outDescriptors[i] != nullptr )
        {
            m_outDescriptors[i]->~Descriptor();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Digital::parseConfiguration( JsonVariant& obj ) noexcept
{
    // Validate the card type
    if ( obj["guid"].isNull() || strcmp( obj["guid"], GUID_STRING ) != 0 )
    {
        m_error = ERR_GUID_WRONG_TYPE;
        return;
    }

    // Ensure that a LocalId has been assigned
    if ( obj["localId"].isNull() )
    {
        m_error = ERR_CARD_MISSING_LOCAL_ID;
        return;
    }
    m_localId = obj["localId"];

    // Get the Text label (not a required field)
    const char* name = obj["name"];
    if ( name == nullptr )
    {
        m_cardName = (char*) emptyString_;
    }
    else
    {
        m_cardName = (char*) m_allocator.allocate( strlen( name ) + 1 );
        if ( m_cardName == nullptr )
        {
            m_cardName = (char*) emptyString_;
            m_error    = ERR_MEMORY_CARD_NAME;
            return;
        }
        strcpy( m_cardName, name );
    }

    // Parse point Descriptors
    if ( !obj["points"].isNull() )
    {
        JsonArray inputs = obj["points"]["inputs"];
        if ( !inputs.isNull() )
        {
            // Validate supported number of signals
            m_numInputs = inputs.size();
            if ( m_numInputs > MAX_CHANNELS )
            {
                m_error     = ERR_TOO_MANY_INPUT_POINTS;
                m_numInputs = 0;
                return;
            }

            // Create the Input descriptors
            if ( !createDescriptors( m_inDescriptors, m_inputChannels, inputs, m_numInputs, ERR_MEMORY_INPUT_DESCRIPTORS ) )
            {
                return;
            }

            // Get the initial state
            m_initInputVals  = obj["initialInputValueMask"] | 0;
        }

        // Parse Output descriptors
        JsonArray outputs = obj["descriptors"]["outputs"];
        if ( !outputs.isNull() )
        {
            // Validate supported number of signals
            m_numOutputs = outputs.size();
            if ( m_numOutputs > MAX_CHANNELS )
            {
                m_error      = ERR_TOO_MANY_OUTPUT_POINTS;
                m_numOutputs = 0;
                return;
            }

            // Create the Input descriptors
            if ( !createDescriptors( m_outDescriptors, m_outputChannels, outputs, m_numOutputs, ERR_MEMORY_OUTPUT_DESCRIPTORS ) )
            {
                return;
            }
        }
    }
}

bool Digital::createDescriptors( Fxt::Point::Descriptor* descriptorList[], ChannelInfo_T* channels, JsonArray& json, size_t numDescriptors, uint32_t errCode ) noexcept
{
    // Initialize the descriptor elements
    for ( size_t i=0; i < numDescriptors; i++ )
    {
        if ( json[i]["id"].isNull() )
        {
            m_error = ERR_POINT_MISSING_LOCAL_ID;
            return false;
        }

        // Parse ID, Channel, and Name
        uint32_t    localId       = json[i]["id"];
        uint16_t    channel       = json[i]["channel"] | MAX_CHANNELS + 1;
        if ( channel > MAX_CHANNELS )
        {
            m_error = ERR_BAD_CHANNEL_ASSIGNMENTS;
            return false;
        }
        if ( channels[i].channel != 0xFFFF )
        {
            m_error = ERR_BAD_CHANNEL_ASSIGNMENTS;
            return false;
        }
        channels[i].channel = channel;
        const char* name          = json[i]["name"] | emptyString_;
        char*       nameMemoryPtr = (char*) m_allocator.allocate( strlen( name ) + 1 );
        if ( nameMemoryPtr == nullptr )
        {
            m_error = errCode;
            return false;
        }
        strcpy( nameMemoryPtr, name );

        // Create the descriptor
        Fxt::Point::Descriptor* memDescriptor = (Fxt::Point::Descriptor*) m_allocator.allocate( sizeof( Fxt::Point::Descriptor ) );
        if ( memDescriptor == nullptr )
        {
            m_error = errCode;
            return false;
        }
        descriptorList[i] = new(memDescriptor) Fxt::Point::Descriptor( nameMemoryPtr, localId, Cpl::Point::Bool::create );
    }
    return true;
}

void Digital::createPoints( Cpl::Container::Dictionary<Fxt::Point::Descriptor>& descriptorDb, Fxt::Point::Database& pointDb, PointAllocators_T& allocators ) noexcept
{
    // Create Input points
    if ( m_numInputs > 0 )
    {
        // Create internal Points
        m_banks.internalInputs->populate( m_inDescriptors, *allocators.internalInputs, pointDb );
        for ( size_t i=0; i < m_numInputs; i++ )
        {
            m_inputChannels[i].point = (Cpl::Point::Bool*) m_inDescriptors[i]->getPoint();
        }

        // Create IO Registers
        m_banks.registerInputs->populate( m_inDescriptors, *allocators.registerInputs, pointDb );

        // Create visible Points. This must be done LAST so that the descriptors 
        // contain the 'correct' Point references for the 'visible' points
        m_banks.virtualInputs->populate( m_inDescriptors, *allocators.virtualInputs, pointDb );
        for ( size_t i=0; i < m_numInputs; i++ )
        {
            descriptorDb.insert( *(m_inDescriptors[i]) );
        }
    }

    // Create Output points
    if ( m_numOutputs > 0 )
    {
        // Create internal Points
        m_banks.internalOutputs->populate( m_outDescriptors, *allocators.internalOutputs, pointDb );
        for ( size_t i=0; i < m_numOutputs; i++ )
        {
            m_outputChannels[i].point = (Cpl::Point::Bool*) m_inDescriptors[i]->getPoint();
        }

        // Create IO Registers
        m_banks.registerOutputs->populate( m_outDescriptors, *allocators.registerOutputs, pointDb );

        // Create visible Points. This must be done LAST so that the descriptors 
        // contain the 'correct' PointId/Point references for the 'visible' points
        m_banks.virtualOutputs->populate( m_outDescriptors, *allocators.virtualOutputs, pointDb );
        for ( size_t i=0; i < m_numInputs; i++ )
        {
            descriptorDb.insert( *(m_outDescriptors[i]) );
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
bool Digital::start() noexcept
{
    m_inputVals  = m_initInputVals;
    maskToPoints();
    return Common_::start();
}

bool Digital::stop() noexcept
{
    return Common_::stop();
}

const char* Digital::getGuid() const noexcept
{
    return GUID_STRING;
}

const char* Digital::getTypeName() const noexcept
{
    return "Fxt::Card::HW::Mock::Digital";
}

const char* Digital::getErrorText( uint32_t errCode ) const noexcept
{
    switch ( errCode )
    {
    case ERR_MEMORY_POINT_BANKS:        return "Unable to allocate memory for the card's Point Banks.";
    case ERR_MEMORY_INPUT_DESCRIPTORS:  return "Unable to allocate memory for the card's Input Point Descriptors.";
    case ERR_MEMORY_OUTPUT_DESCRIPTORS: return "Unable to allocate memory for the card's Output Point Descriptors.";
    case ERR_GUID_WRONG_TYPE:           return "Configuration contains the wrong GUID (i.e. the JSON object calls out a different card type).";
    case ERR_CARD_MISSING_LOCAL_ID:     return "Configuration does NOT contain a LocalId value for the card.";
    case ERR_MEMORY_CARD_NAME:          return "Unable to allocate memory for the card's name.";
    case ERR_POINT_MISSING_LOCAL_ID:    return "Configuration does NOT contain a LocalId value for one of it Points.";
    case ERR_TOO_MANY_INPUT_POINTS:     return "Configuration contains TOO many input Points (max is 32).";
    case ERR_TOO_MANY_OUTPUT_POINTS:    return "Configuration contains TOO many output Points (max is 32).";
    case ERR_BAD_CHANNEL_ASSIGNMENTS:   return "Configuration contains duplicate or our-of-range Channel IDs.";
    default:
        return nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Digital::setInputs( uint32_t bitMaskToOR )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_internalLock );
    m_inputVals |= bitMaskToOR;
    maskToPoints();

    // Immediately update my Input Registers
    updateInputRegisters();
}


void Digital::clearInputs( uint32_t bitMaskToAND )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_internalLock );
    m_inputVals &= bitMaskToAND;
    maskToPoints();

    // Immediately update my Input Registers
    updateInputRegisters();
}

void Digital::toggleInputs( uint32_t bitMaskToXOR )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_internalLock );
    m_inputVals ^= bitMaskToXOR;
    maskToPoints();

    // Immediately update my Input Registers
    updateInputRegisters();
}

uint32_t Digital::getOutputs()
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_internalLock );
    readOutputRegisters();  // Get the current Register values
    pointsToMask();
    return m_outputVals;
}

void Digital::pointsToMask() noexcept
{
    m_outputVals = 0;
    for ( uint16_t i=0, mask=0x1; i < m_numOutputs; i++, mask <<= 1 )
    {
        Cpl::Point::Bool* point = getPointByChannel( m_outputChannels, i );
        if ( point )
        {
            bool pointVal = 0;  // Invalid Point values are treated as off/Signal-low
            point->read( pointVal );
            if ( pointVal )
            {
                m_outputVals |= mask;
            }
        }
    }
}

void Digital::maskToPoints() noexcept
{
    for ( uint16_t i=0, mask=0x1; i < m_numInputs; i++, mask <<= 1 )
    {
        Cpl::Point::Bool* point = getPointByChannel( m_inputChannels, i );
        if ( point )
        {
            if ( (m_inputVals & mask) == mask )
            {
                point->set();
            }
            else

            {
                point->clear();

            }
        }
    }
}

Cpl::Point::Bool* Digital::getPointByChannel( ChannelInfo_T * channels, uint16_t channelIndex ) noexcept
{
    for ( uint16_t i=0; i < MAX_CHANNELS; i++ )
    {
        if ( channels[i].channel == channelIndex )
        {
            return channels[i].point;
        }
    }
    return nullptr;
}
