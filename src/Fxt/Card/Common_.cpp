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


#include "Common_.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/Trace.h"
#include "Fxt/Point/Api.h"
#include "Fxt/Logging/Api.h"
#include "Cpl/Itc/SyncReturnHandler.h"

#define SECT_   "Fxt::Card"

///
using namespace Fxt::Card;

//////////////////////////////////////////////////
Common_::Common_( uint16_t maxInputChannels,
                  uint16_t maxOutputChannels )
    : m_inputIoRegisterPoints( nullptr )
    , m_outputIoRegisterPoints( nullptr )
    , m_error( Fxt::Type::Error::SUCCESS() )
    , m_numInputs( 0 )
    , m_numOutputs( 0 )
    , m_maxInputs( maxInputChannels )
    , m_maxOutputs( maxOutputChannels )
    , m_slotNum( 0xFF )
    , m_started( false )
{
    // All of the work is done in the child class
}

bool Common_::initialize( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                          JsonVariant&                       cardObject ) noexcept
{
    // Create the arrays of IO Register point pointers
    if ( m_maxInputs > 0 )
    {
        m_inputIoRegisterPoints = (Fxt::Point::Api**) generalAllocator.allocate( sizeof( Fxt::Point::Api* ) * m_maxInputs );
        if ( !m_inputIoRegisterPoints )
        {
            m_error = fullErr( Err_T::MEMORY_CARD );
            m_error.logIt( getTypeName() );
            return false;
        }
        memset( m_inputIoRegisterPoints, 0, sizeof( Fxt::Point::Api* ) * m_maxInputs );
    }
    if ( m_maxOutputs > 0 )
    {
        m_outputIoRegisterPoints = (Fxt::Point::Api**) generalAllocator.allocate( sizeof( Fxt::Point::Api* ) * m_maxOutputs );
        if ( !m_outputIoRegisterPoints )
        {
            m_error = fullErr( Err_T::MEMORY_CARD );
            m_error.logIt( getTypeName() );
            return false;
        }
        memset( m_outputIoRegisterPoints, 0, sizeof( Fxt::Point::Api* ) * m_maxOutputs );
    }


    // Parse slot assignment
    if ( cardObject["slot"].is<uint8_t>() == false )
    {
        m_error = fullErr( Err_T::MISSING_SLOT );
        m_error.logIt( getTypeName() );
        return false;
    }
    else
    {
        m_slotNum = cardObject["slot"];
    }

    return true;
}

Common_::~Common_()
{
    // ensure stop gets called
    stop();
}

//////////////////////////////////////////////////
bool Common_::start( uint64_t currentElapsedTimeUsec ) noexcept
{
    if ( !m_started && m_error == Fxt::Type::Error::SUCCESS() )
    {
        m_started = setInitialPointValues();
        return m_started;
    }
    return false;
}

void Common_::stop() noexcept
{
    if ( m_started )
    {
        m_started = false;
    }
}

bool Common_::isStarted() const noexcept
{
    return m_started;
}


Fxt::Type::Error Common_::getErrorCode() const noexcept
{
    return m_error;
}

uint8_t Common_::getSlotNumber() const noexcept
{
    return m_slotNum;
}

//////////////////////////////////////////////////
bool Common_::scanInputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    return m_virtualInputs.copyStatefulMemoryFrom( m_ioRegisterInputs );
}

bool Common_::flushOutputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    return m_ioRegisterOutputs.copyStatefulMemoryFrom( m_virtualOutputs );
}


//////////////////////////////////////////////////
bool Common_::parseInputOutputPoints( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                      Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                                      Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                                      Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                                      Fxt::Point::DatabaseApi&           dbForPoints,
                                      JsonVariant&                       cardObject,
                                      bool                               storeByChannelNum,
                                      unsigned                           minInputs,
                                      unsigned                           minOutputs ) noexcept
{
    // Parse channels
    if ( !cardObject["points"].isNull() )
    {
        // INPUTS
        JsonArray inputs = cardObject["points"]["inputs"];
        if ( !inputs.isNull() )
        {
            // Validate supported number of signals
            size_t nInputs = inputs.size();
            if ( nInputs > m_maxInputs )
            {
                m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::TOO_MANY_INPUT_POINTS );
                m_error.logIt( getTypeName() );
                return false;
            }
            m_numInputs = (unsigned) nInputs;

            // Create Virtual Points
            for ( unsigned idx=0; idx < m_numInputs; idx++ )
            {
                JsonObject channelObj = inputs[idx].as<JsonObject>();
                if ( !createPointForChannel( pointFactoryDb,
                                             m_virtualInputs,
                                             false,
                                             channelObj,
                                             generalAllocator,
                                             cardStatefulDataAllocator,
                                             dbForPoints ) )
                {
                    // Stop processing if/when an error occurred
                    return false;
                }

            }

            // Create IO Register Points 
            for ( unsigned idx=0; idx < m_numInputs; idx++ )
            {
                JsonObject channelObj     = inputs[idx].as<JsonObject>();
                Fxt::Point::Api* ioRegPtr = createPointForChannel( pointFactoryDb,
                                                                   m_ioRegisterInputs,
                                                                   true,
                                                                   channelObj,
                                                                   generalAllocator,
                                                                   cardStatefulDataAllocator,
                                                                   dbForPoints );
                if ( ioRegPtr == nullptr )
                {
                    return false;
                }

                // Cache the IO Register Point pointer
                if ( storeByChannelNum )
                {
                    JsonObject elem       = inputs[idx];
                    uint16_t   channelNum = getChannelNumber( elem, 1, m_maxInputs );  // Note: '0' is invalid channel #
                    if ( channelNum == 0 )
                    {
                        return false;
                    }

                    m_inputIoRegisterPoints[channelNum - 1] = ioRegPtr;
                }
                else
                {
                    m_inputIoRegisterPoints[idx] = ioRegPtr;
                }
            }
        }
    }


    // OUTPUTS
    JsonArray outputs = cardObject["points"]["outputs"];
    if ( !outputs.isNull() )
    {
        // Validate supported number of signals
        size_t nOutputs = outputs.size();
        if ( nOutputs > m_maxOutputs )
        {
            m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::TOO_MANY_OUTPUT_POINTS );
            m_error.logIt( getTypeName() );
            return false;
        }
        m_numOutputs = (unsigned) nOutputs;

        // Create Virtual Points
        for ( unsigned idx=0; idx < m_numOutputs; idx++ )
        {
            JsonObject channelObj = outputs[idx].as<JsonObject>();
            if ( !createPointForChannel( pointFactoryDb,
                                         m_virtualOutputs,
                                         false,
                                         channelObj,
                                         generalAllocator,
                                         haStatefulDataAllocator,     // All Output Virtual Points are part of the HA Data set
                                         dbForPoints ) )
            {
                return false;
            }
        }

        // Create IO Register Points
        for ( unsigned idx=0; idx < m_numOutputs; idx++ )
        {
            JsonObject channelObj = outputs[idx].as<JsonObject>();
            Fxt::Point::Api* ioRegPtr = createPointForChannel( pointFactoryDb,
                                                               m_ioRegisterOutputs,
                                                               true,
                                                               channelObj,
                                                               generalAllocator,
                                                               cardStatefulDataAllocator,
                                                               dbForPoints );
            if ( ioRegPtr == nullptr )
            {
                return false;
            }

            // Cache the IO Register Point pointer
            if ( storeByChannelNum )
            {
                JsonObject elem       = outputs[idx];
                uint16_t   channelNum = getChannelNumber( elem, 1, m_maxOutputs );  // Note: '0' is invalid channel #
                if ( channelNum == 0 )
                {
                    return false;
                }

                m_outputIoRegisterPoints[channelNum - 1] = ioRegPtr;
            }
            else
            {
                m_outputIoRegisterPoints[idx] = ioRegPtr;
            }
        }
    }

    // Validate minimum sizes
    if ( m_numInputs < minInputs )
    {
        m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::TOO_FEW_INPUT_POINTS );
        m_error.logIt( getTypeName() );
        return false;
    }
    if ( m_numOutputs < minOutputs )
    {
        m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::TOO_FEW_OUTPUT_POINTS );
        m_error.logIt( getTypeName() );
        return false;
    }
    if ( m_numInputs == 0 && m_numOutputs == 0 )
    {
        m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::NO_POINTS );
        m_error.logIt( getTypeName() );
        return false;
    }

    // IF I get here -->everything worked!
    return true;
}

uint16_t Common_::getChannelNumber( JsonObject& channelObject, uint16_t minChannelNum, uint16_t maxChannelNum ) noexcept
{
    uint16_t channelNum = channelObject["channel"] | 0;  // Note: '0' is invalid channel #
    if ( channelNum  < minChannelNum || channelNum > maxChannelNum )
    {
        m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
        m_error.logIt( getTypeName() );
        return 0;
    }

    return channelNum;
}

Fxt::Point::Api* Common_::createPointForChannel( Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                                                 Fxt::Point::Bank&                  pointBank,
                                                 bool                               isIoRegPt,
                                                 JsonObject&                        channelObject,
                                                 Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                                 Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                                                 Fxt::Point::DatabaseApi&           dbForPoints ) noexcept
{
    // Validate that we have a JSON object
    if ( channelObject.isNull() )
    {
        m_error = fullErr( Err_T::BAD_JSON );
        m_error.logIt( getTypeName() );
        return nullptr;
    }

    // Parse IDs
    const char* idName  = isIoRegPt ? "ioRegId" : "id";
    uint32_t    id      = channelObject[idName] | Fxt::Point::Api::INVALID_ID;
    if ( id == Point::Api::INVALID_ID )
    {
        m_error = fullErr( Err_T::POINT_MISSING_ID );
        m_error.logIt( getTypeName() );
        return nullptr;
    }

    // Create Point
    Fxt::Type::Error pointErr;
    bool result = pointBank.createPoint( pointFactoryDb,
                                         channelObject,
                                         m_error,
                                         generalAllocator,
                                         statefulDataAllocator,
                                         dbForPoints,
                                         idName,
                                         isIoRegPt );
    if ( !result )
    {
        m_error = fullErr( Err_T::POINT_CREATE_ERROR );
        m_error.logIt( "%s. id=%lu", getTypeName(), id );
        return nullptr;
    }

    return dbForPoints.lookupById( id );
}

bool Common_::setInitialPointValues() noexcept
{
    // Initialize input IO Registers
    for ( unsigned i=0; i < m_maxInputs; i++ )
    {
        if ( m_inputIoRegisterPoints[i] )
        {
            m_inputIoRegisterPoints[i]->setInvalid(); // Ensure the Point is invalid when there is NO setter
            m_inputIoRegisterPoints[i]->updateFromSetter();
        }
    }

    // Update the Virtual Input points to match their corresponding IO Registers
    if ( !m_virtualInputs.copyStatefulMemoryFrom( m_ioRegisterInputs ) )
    {
        return false;
    }

    // Initialize OUPUT IO Registers
    for ( unsigned i=0; i < m_maxOutputs; i++ )
    {
        if ( m_outputIoRegisterPoints[i] )
        {
            m_outputIoRegisterPoints[i]->setInvalid(); // Ensure the Point is invalid when there is NO setter
            m_outputIoRegisterPoints[i]->updateFromSetter();
        }
    }

    // Update the Virtual Output points to match their corresponding IO Registers
    if ( !m_virtualOutputs.copyStatefulMemoryFrom( m_ioRegisterOutputs ) )
    {
        return false;
    }

    return true;
}


