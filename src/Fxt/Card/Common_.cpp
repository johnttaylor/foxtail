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
#include <new>

#define SECT_   "Fxt::Card"

///
using namespace Fxt::Card;

//////////////////////////////////////////////////
Common_::Common_( uint16_t                           maxInputChannels,
                  uint16_t                           maxOutputChannels,
                  Cpl::Memory::ContiguousAllocator&  generalAllocator,
                  JsonVariant&                       cardObject )
    : m_inputIoRegisterPoints( nullptr )
    , m_outputIoRegisterPoints( nullptr )
    , m_error( Fxt::Type::Error::SUCCESS() )
    , m_numInputs( 0 )
    , m_numOutputs( 0 )
    , m_slotNum( 0xFF )
    , m_started( false )
{
    // Create the arrays of IO Register point pointers
    if ( maxInputChannels > 0 )
    {
        m_inputIoRegisterPoints = (Fxt::Point::Api**) generalAllocator.allocate( sizeof( Fxt::Point::Api* ) * maxInputChannels );
        if ( !m_inputIoRegisterPoints )
        {
            m_error = fullErr( Err_T::MEMORY_CARD );
            m_error.logIt();
        }
        memset( m_inputIoRegisterPoints, 0, sizeof( Fxt::Point::Api* ) * maxInputChannels );
    }
    if ( maxOutputChannels > 0 )
    {
        m_outputIoRegisterPoints = (Fxt::Point::Api**) generalAllocator.allocate( sizeof( Fxt::Point::Api* ) * maxOutputChannels );
        if ( !m_outputIoRegisterPoints )
        {
            m_error = fullErr( Err_T::MEMORY_CARD );
            m_error.logIt();
        }
        memset( m_outputIoRegisterPoints, 0, sizeof( Fxt::Point::Api* ) * maxOutputChannels );
    }


    // Parse slot assignment
    if ( cardObject["slot"].is<uint8_t>() == false )
    {
        m_error = fullErr( Err_T::MISSING_SLOT );
        m_error.logIt();
    }
    else
    {
        m_slotNum = cardObject["slot"];
    }
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

Fxt::Point::Api* Common_::createPointForChannel( Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                                                 Fxt::Point::Bank&                  pointBank,
                                                 const char*                        expectedGUID,
                                                 bool                               isIoRegPt,
                                                 JsonObject&                        channelObject,
                                                 Fxt::Type::Error&                  cardErrorCode,
                                                 uint16_t                           minChannelNumber,
                                                 uint16_t                           maxChannelNumber,
                                                 uint16_t&                          channelNum,
                                                 Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                                 Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                                                 Fxt::Point::DatabaseApi&           dbForPoints ) noexcept
{
    // Validate that we have a JSON object
    if ( channelObject.isNull() )
    {
        cardErrorCode = fullErr( Err_T::BAD_JSON );
        cardErrorCode.logIt( getTypeName() );
        return nullptr;
    }

    // Parse IDs
    const char* idName  = isIoRegPt ? "ioRegId" : "id";
    uint32_t    id      = channelObject[idName] | Fxt::Point::Api::INVALID_ID;
    if ( id == Point::Api::INVALID_ID )
    {
        cardErrorCode = fullErr( Err_T::POINT_MISSING_ID );
        m_error.logIt( getTypeName() );
        return nullptr;
    }

    // Parse channel Number
    channelNum          = channelObject["channel"] | 0;   // Zero is NOT a valid channel number
    if ( channelNum == 0 || channelNum < minChannelNumber || channelNum > maxChannelNumber )
    {
        m_error = fullErr( Err_T::BAD_CHANNEL_ASSIGNMENTS );
        m_error.logIt( "%s. ch=%d", getTypeName(), channelNum );
        return nullptr;
    }

    // Create Point
    Fxt::Type::Error pointErr;
    bool result = pointBank.createPoint( pointFactoryDb,
                                         channelObject,
                                         pointErr,
                                         generalAllocator,
                                         statefulDataAllocator,
                                         dbForPoints,
                                         idName,
                                         isIoRegPt );
    if ( !result )
    {
        m_error = fullErr( Err_T::FAILED_POINT_CREATED );
        m_error.logIt( "%s. ch=%d", getTypeName(), channelNum );
        return nullptr;
    }
    if ( pointErr != Fxt::Type::Error::SUCCESS() )
    {
        m_error = fullErr( Err_T::POINT_CREATE_ERROR );
        m_error.logIt( "%s. ch=%d", getTypeName(), channelNum );
        return nullptr;
    }

    // Validate the point type
    Fxt::Point::Api* ptPtr = dbForPoints.lookupById( id );
    if ( !Fxt::Point::Api::validatePointTypes( &ptPtr, 1, expectedGUID ) )
    {
        m_error = fullErr( Err_T::POINT_WRONG_TYPE );
        m_error.logIt( "%s. ch=%d", getTypeName(), channelNum );
        return nullptr;
    }

    return ptPtr;
}

bool Common_::setInitialPointValues() noexcept
{
    // Initialize input IO Registers
    for ( unsigned i=0; i < m_numInputs; i++ )
    {
        m_inputIoRegisterPoints[i]->updateFromSetter();
    }

    // Update the Virtual Input points to match their corresponding IO Registers
    if ( !m_virtualInputs.copyStatefulMemoryFrom( m_ioRegisterInputs ) )
    {
        return false;
    }

    // Initialize OUPUT IO Registers
    for ( unsigned i=0; i < m_numOutputs; i++ )
    {
        m_outputIoRegisterPoints[i]->updateFromSetter();
    }

    // Update the Virtual Output points to match their corresponding IO Registers
    if ( !m_virtualOutputs.copyStatefulMemoryFrom( m_ioRegisterOutputs) )
    {
        return false;
    }

    return true;
}


