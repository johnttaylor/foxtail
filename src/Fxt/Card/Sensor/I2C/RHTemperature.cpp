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


#include "Dio30.h"
#include "Cpl/System/Assert.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

///
using namespace Fxt::Node::SBC::PiPicoDemo;

#define MAX_INPUT_CHANNELS      30
#define INPUT_POINT_OFFSET      0

#define MAX_OUTPUT_CHANNELS     30
#define OUTPUT_POINT_OFFSET     (MAX_INPUT_CHANNELS)

#define TOTAL_MAX_CHANNELS      (MAX_INPUT_CHANNELS + MAX_OUTPUT_CHANNELS)


///////////////////////////////////////////////////////////////////////////////
Dio30::Dio30( Cpl::Memory::ContiguousAllocator&  generalAllocator,
              Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
              Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
              Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
              Fxt::Point::DatabaseApi&           dbForPoints,
              JsonVariant&                       cardObject )
    : Fxt::Card::Common_( TOTAL_MAX_CHANNELS, generalAllocator, cardObject )
{
    if ( m_error == Fxt::Type::Error::SUCCESS() )
    {
        parseConfiguration( generalAllocator, cardStatefulDataAllocator, haStatefulDataAllocator, pointFactoryDb, dbForPoints, cardObject );
    }
}



///////////////////////////////////////////////////////////////////////////////
void Dio30::parseConfiguration( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                                Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                                Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                                Fxt::Point::DatabaseApi&           dbForPoints,
                                JsonVariant&                       cardObject ) noexcept
{
    // Parse channels
    if ( !cardObject["points"].isNull() )
    {
        // INPUTS
        JsonArray inputs = cardObject["points"]["inputs"];
        if ( !inputs.isNull() )
        {
            // Validate supported number of signals
            size_t numInputs = inputs.size();
            if ( numInputs > MAX_INPUT_CHANNELS )
            {
                m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::TOO_MANY_INPUT_POINTS );
                m_error.logIt();
                return;
            }

            // Create Virtual Points
            for ( size_t idx=0; idx < numInputs; idx++ )
            {
                // Stop processing if/when an error occurred
                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }

                uint16_t   channelNum_notUsed;
                JsonObject channelObj = inputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_virtualInputs,
                                       Fxt::Point::Bool::GUID_STRING,
                                       false,
                                       channelObj,
                                       m_error,
                                       MAX_INPUT_CHANNELS,
                                       INPUT_POINT_OFFSET,
                                       channelNum_notUsed,
                                       generalAllocator,
                                       cardStatefulDataAllocator,
                                       dbForPoints );
            }

            // Create IO Register Points
            for ( size_t idx=0; idx < numInputs; idx++ )
            {
                // Stop processing if/when an error occurred
                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }

                uint16_t   channelNum_notUsed;
                JsonObject channelObj = inputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_ioRegisterInputs,
                                       Fxt::Point::Bool::GUID_STRING,
                                       true,
                                       channelObj,
                                       m_error,
                                       MAX_INPUT_CHANNELS,
                                       INPUT_POINT_OFFSET,
                                       channelNum_notUsed,
                                       generalAllocator,
                                       cardStatefulDataAllocator,
                                       dbForPoints );
            }
        }

        // OUTPUTS
        JsonArray outputs = cardObject["points"]["outputs"];
        if ( !outputs.isNull() )
        {
            // Validate supported number of signals
            size_t numOutputs = outputs.size();
            if ( numOutputs > MAX_OUTPUT_CHANNELS )
            {
                m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::TOO_MANY_OUTPUT_POINTS );
                m_error.logIt();
                return;
            }

            // Create Virtual Points
            for ( size_t idx=0; idx < numOutputs; idx++ )
            {
                uint16_t   channelNum_notUsed;
                JsonObject channelObj = outputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_virtualOutputs,
                                       Fxt::Point::Bool::GUID_STRING,
                                       false,
                                       channelObj,
                                       m_error,
                                       MAX_OUTPUT_CHANNELS,
                                       OUTPUT_POINT_OFFSET,
                                       channelNum_notUsed,
                                       generalAllocator,
                                       haStatefulDataAllocator,     // All Output Virtual Points are part of the HA Data set
                                       dbForPoints );

                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }
            }

            // Create IO Register Points
            for ( size_t idx=0; idx < numOutputs; idx++ )
            {
                uint16_t   channelNum_notUsed;
                JsonObject channelObj = outputs[idx].as<JsonObject>();
                createPointForChannel( pointFactoryDb,
                                       m_ioRegisterOutputs,
                                       Fxt::Point::Bool::GUID_STRING,
                                       true,
                                       channelObj,
                                       m_error,
                                       MAX_OUTPUT_CHANNELS,
                                       OUTPUT_POINT_OFFSET,
                                       channelNum_notUsed,
                                       generalAllocator,
                                       cardStatefulDataAllocator,
                                       dbForPoints );

                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return;
                }
            }
        }

        // Make sure that no IO pin has been 'doubled-up'
        for ( int idx = 0; idx < MAX_INPUT_CHANNELS; idx++ )
        {
            if ( m_ioRegisterPoints[idx + INPUT_POINT_OFFSET] != nullptr &&
                 m_ioRegisterPoints[idx + OUTPUT_POINT_OFFSET] != nullptr )
            {
                m_error = Fxt::Card::fullErr( Fxt::Card::Err_T::BAD_CHANNEL_ASSIGNMENTS );
                m_error.logIt();
                return;
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
bool Dio30::start( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Call the parent's start-up actions
    if ( Common_::start( currentElapsedTimeUsec ) )
    {
        // Initialize inputs 
        for ( unsigned i=0; i < MAX_INPUT_CHANNELS; i++ )
        {
            if ( m_ioRegisterPoints[i+ INPUT_POINT_OFFSET] != nullptr )
            {
                // Configure Inputs
                gpio_init( i );
                gpio_set_dir( i, GPIO_IN );
                gpio_pull_up( i );

                // Set the initial IO Register values
                m_ioRegisterPoints[i+ INPUT_POINT_OFFSET]->updateFromSetter();
            }
        }

        // Initialize Outputs 
        for ( unsigned i=0; i < MAX_OUTPUT_CHANNELS; i++ )
        {
            if ( m_ioRegisterPoints[i+ OUTPUT_POINT_OFFSET] != nullptr )
            {
                // Configure Outputs
                gpio_init( i );
                gpio_set_dir( i, GPIO_OUT );

                // Set the initial IO Register values
                m_ioRegisterPoints[i + OUTPUT_POINT_OFFSET]->updateFromSetter();
            }
        }


        // If I get here -->everything worked            
        return true;
    }

    // Start FAILED
    return false;
}



void Dio30::stop() noexcept
{
    // Release Input pins
    for ( unsigned i=0; i < MAX_INPUT_CHANNELS; i++ )
    {
        if ( m_ioRegisterPoints[i+ INPUT_POINT_OFFSET] != nullptr )
        {
            gpio_deinit( i );
            gpio_disable_pulls( i );
        }
    }

    // Release Output pins
    for ( unsigned i=0; i < MAX_OUTPUT_CHANNELS; i++ )
    {
        if ( m_ioRegisterPoints[i+ OUTPUT_POINT_OFFSET] != nullptr )
        {
            gpio_deinit( i );
        }
    }

    // Call my parent's stop
    Common_::stop();
}

const char* Dio30::getTypeGuid() const noexcept
{
    return GUID_STRING;
}

const char* Dio30::getTypeName() const noexcept
{
    return TYPE_NAME;
}


///////////////////////////////////////////////////////////////////////////////
bool Dio30::scanInputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Sample Inputs
    for ( unsigned i=0; i < MAX_INPUT_CHANNELS; i++ )
    {
        if ( m_ioRegisterPoints[i + INPUT_POINT_OFFSET] != nullptr )
        {
            Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_ioRegisterPoints[i + INPUT_POINT_OFFSET];
            pt->write( gpio_get( i ) );
        }
    }

    // Call parent class to manage the transfer between IO Registers and Virtual Points
    return Common_::scanInputs( currentElapsedTimeUsec );
}


bool Dio30::flushOutputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Call parent class to manage the transfer between IO Registers and Virtual Points
    bool result = Common_::flushOutputs( currentElapsedTimeUsec );

    // Update outputs
    if ( result )
    {
        for ( unsigned i=0; i < MAX_OUTPUT_CHANNELS; i++ )
        {
            if ( m_ioRegisterPoints[i + OUTPUT_POINT_OFFSET] != nullptr )
            {
                Fxt::Point::Bool* pt = (Fxt::Point::Bool*) m_ioRegisterPoints[i + OUTPUT_POINT_OFFSET];
                bool val;
                if ( pt->read( val ) )
                {
                    gpio_put( i, val );
                }
            }
        }
    }

    return result;
}

