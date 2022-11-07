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
                  Fxt::Point::DatabaseApi&                              pointDatabase,
                  Cpl::Memory::ContiguousAllocator&                     generalAllocator )
    : Fxt::Card::Common_( generalAllocator )
    , m_initInputVals( 0 )
    , m_numInputs( 0 )
    , m_numOutputs( 0 )
{
    CPL_SYSTEM_ASSERT( pointAllocators.internalInputs );
    CPL_SYSTEM_ASSERT( pointAllocators.registerInputs );
    CPL_SYSTEM_ASSERT( pointAllocators.virtualInputs );
    CPL_SYSTEM_ASSERT( pointAllocators.internalOutputs );
    CPL_SYSTEM_ASSERT( pointAllocators.registerOutputs );
    CPL_SYSTEM_ASSERT( pointAllocators.virtualOutputs );

    memset( &m_externalInDescriptors, 0, sizeof( m_externalInDescriptors ) );
    memset( &m_ioRegInDescriptors, 0, sizeof( m_ioRegInDescriptors ) );
    memset( &m_intennalInDescriptors, 0, sizeof( m_intennalInDescriptors ) );
    memset( &m_externalOutDescriptors, 0, sizeof( m_externalOutDescriptors ) );
    memset( &m_ioRegOutDescriptors, 0, sizeof( m_ioRegOutDescriptors ) );
    memset( &m_intennalOutDescriptors, 0, sizeof( m_intennalOutDescriptors ) );
    memset( &m_inputChannels, 0xFF, sizeof( m_inputChannels ) );
    memset( &m_outputChannels, 0xFF, sizeof( m_outputChannels ) );

    parseConfiguration( cardObject );
    createPoints( pointDatabase, pointAllocators, generalAllocator );
}

Digital::~Digital()
{
    // NOTE: Call the destructors on ALL dynamically allocated objects - but 
    //       the Application is responsible for freeing the actual memory

    // Destroy any created Descriptors
    for ( unsigned i=0; i < MAX_CHANNELS; i++ )
    {
        if ( m_externalInDescriptors[i] != nullptr )
        {
            m_externalInDescriptors[i]->~Descriptor();
        }
        if ( m_ioRegInDescriptors[i] != nullptr )
        {
            m_ioRegInDescriptors[i]->~Descriptor();
        }
        if ( m_intennalInDescriptors[i] != nullptr )
        {
            m_intennalInDescriptors[i]->~Descriptor();
        }

        if ( m_externalOutDescriptors[i] != nullptr )
        {
            m_externalOutDescriptors[i]->~Descriptor();
        }
        if ( m_ioRegOutDescriptors[i] != nullptr )
        {
            m_ioRegOutDescriptors[i]->~Descriptor();
        }
        if ( m_intennalOutDescriptors[i] != nullptr )
        {
            m_intennalOutDescriptors[i]->~Descriptor();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Digital::parseConfiguration( JsonVariant& obj ) noexcept
{
    if ( parseCommon( obj, GUID_STRING ) )
    {
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
                    m_error     = FXT_CARD_ERR_TOO_MANY_INPUT_POINTS;
                    m_numInputs = 0;
                    return;
                }

                // Create the Input descriptors
                if ( !createDescriptors( m_inDescriptors, m_inputChannels, inputs, m_numInputs, FXT_CARD_ERR_MEMORY_INPUT_DESCRIPTORS ) )
                {
                    return;
                }

                // Get the initial state
                m_initInputVals  = obj["initialInputValueMask"] | 0;
            }

            // Parse Output descriptors
            JsonArray outputs = obj["points"]["outputs"];
            if ( !outputs.isNull() )
            {
                // Validate supported number of signals
                m_numOutputs = outputs.size();
                if ( m_numOutputs > MAX_CHANNELS )
                {
                    m_error      = FXT_CARD_ERR_TOO_MANY_OUTPUT_POINTS;
                    m_numOutputs = 0;
                    return;
                }

                // Create the Input descriptors
                if ( !createDescriptors( m_outDescriptors, m_outputChannels, outputs, m_numOutputs, FXT_CARD_ERR_MEMORY_OUTPUT_DESCRIPTORS ) )
                {
                    return;
                }
            }
        }
    }
}

#define INVALID_POINT_ID        ((uint32_t)-1)

bool Digital::createDescriptors( Fxt::Point::Descriptor* externalDesc[],
                                 Fxt::Point::Descriptor* ioRegDesc[],
                                 Fxt::Point::Descriptor* internalDesc[],
                                 uint32_t                channels[], 
                                 JsonArray&              json, 
                                 size_t                  numDescriptors, 
                                 uint32_t                errCode ) noexcept
{
    // Initialize the descriptor elements
    for ( size_t i=0; i < numDescriptors; i++ )
    {
        // Parse ID, Channel, and Name
        uint32_t id         = json[i]["id"] | INVALID_POINT_ID;
        uint32_t ioRegId    = json[i]["idRegId"] | INVALID_POINT_ID;
        uint32_t internalId = json[i]["internalId"] | INVALID_POINT_ID;
        uint16_t channel    = json[i]["channel"] | (MAX_CHANNELS + 1);
        if ( id == INVALID_POINT_ID || ioRegId == INVALID_POINT_ID || internalId == INVALID_POINT_ID )
        {
            m_error = FXT_CARD_ERR_POINT_MISSING_ID;
            return false;
        }
        if ( channel > MAX_CHANNELS || channel == 0 )
        {
            m_error = FXT_CARD_ERR_BAD_CHANNEL_ASSIGNMENTS;
            return false;
        }
        if ( channels[channel] != INVALID_POINT_ID )
        {
            m_error = FXT_CARD_ERR_BAD_CHANNEL_ASSIGNMENTS;
            return false;
        }
        channels[channel-1]       = internalId;

        // Parse and Allocate memory for the point name
        const char* name          = json[i]["name"] | emptyString_;
        char*       nameMemoryPtr = (char*) m_allocator.allocate( strlen( name ) + 1 );
        if ( nameMemoryPtr == nullptr )
        {
            m_error = errCode;
            return false;
        }
        strcpy( nameMemoryPtr, name );

        // Create external Register descriptor
        Fxt::Point::Descriptor* memDescriptor = (Fxt::Point::Descriptor*) m_allocator.allocate( sizeof( Fxt::Point::Descriptor ) );
        if ( memDescriptor == nullptr )
        {
            m_error = errCode;
            return false;
        }
        externalDesc[i] = new(memDescriptor) Fxt::Point::Descriptor( id, nameMemoryPtr, Fxt::Point::Bool::create );

        // Create IO Register descriptor
        memDescriptor = (Fxt::Point::Descriptor*) m_allocator.allocate( sizeof( Fxt::Point::Descriptor ) );
        if ( memDescriptor == nullptr )
        {
            m_error = errCode;
            return false;
        }
        ioRegDesc[i] = new(memDescriptor) Fxt::Point::Descriptor( ioRegId, nameMemoryPtr, Fxt::Point::Bool::create );

        // Create internal Register descriptor
        memDescriptor = (Fxt::Point::Descriptor*) m_allocator.allocate( sizeof( Fxt::Point::Descriptor ) );
        if ( memDescriptor == nullptr )
        {
            m_error = errCode;
            return false;
        }
        internalDesc[i] = new(memDescriptor) Fxt::Point::Descriptor( internalId, nameMemoryPtr, Fxt::Point::Bool::create );
  
        // TODO: Create Setter when there is "initial":{obj} object
        // Get JSON
        // Call Setter::create()
    }
    return true;
}

void Digital::createPoints( Fxt::Point::DatabaseApi&          pointDb,
                            PointAllocators_T&                pointAllocators,
                            Cpl::Memory::ContiguousAllocator& generalAllocator ) noexcept
{
    // Create Input points
    if ( m_numInputs > 0 )
    {
        // Create internal Points
        m_banks.internalInputs->populate( m_inDescriptors, *pointAllocators.internalInputs, pointDb, generalAllocator );
        for ( size_t i=0; i < m_numInputs; i++ )
        {
            m_inputChannels[i].point = (Fxt::Point::Bool*) m_inDescriptors[i]->getPoint();
        }

        // Create IO Registers
        m_banks.registerInputs->populate( m_inDescriptors, *pointAllocators.registerInputs, pointDb, generalAllocator );

        // Create visible Points. This must be done LAST so that the descriptors 
        // contain the 'correct' Point references for the 'visible' points
        m_banks.virtualInputs->populate( m_inDescriptors, *pointAllocators.virtualInputs, pointDb, generalAllocator );
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
            m_outputChannels[i].point = (Cpl::Point::Bool*) m_outDescriptors[i]->getPoint();
        }

        // Create IO Registers
        m_banks.registerOutputs->populate( m_outDescriptors, *allocators.registerOutputs, pointDb );

        // Create visible Points. This must be done LAST so that the descriptors 
        // contain the 'correct' PointId/Point references for the 'visible' points
        m_banks.virtualOutputs->populate( m_outDescriptors, *allocators.virtualOutputs, pointDb );
        for ( size_t i=0; i < m_numOutputs; i++ )
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
    return TYPE_NAME;
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

bool Digital::getOutputs( uint32_t& valueMask )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_internalLock );
    readOutputRegisters();  // Get the current Register values
    bool validStatus = pointsToMask();
    valueMask        = m_outputVals;
    return validStatus;
}

bool Digital::pointsToMask() noexcept
{
    bool atLeastOneInvalid = false;
    m_outputVals           = 0;
    for ( uint16_t i=0, mask=0x1; i < m_numOutputs; i++, mask <<= 1 )
    {
        Cpl::Point::Bool* point = getPointByChannel( m_outputChannels, i );
        if ( point )
        {
            bool pointVal;
            if ( point->read( pointVal ) )
            {
                if ( pointVal )
                {
                    m_outputVals |= mask;
                }
            }
            else
            {
                atLeastOneInvalid = true;
            }
        }
    }

    return !atLeastOneInvalid;
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
