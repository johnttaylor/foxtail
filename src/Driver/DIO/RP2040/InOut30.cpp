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
/** @file

    This file implements the DIO::InOut driver that provides access to GPIO0
    through GPIO29.

*/

#include "Driver/DIO/InOut.h"
#include "InOut30.h"
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define MAX_IO      30

using namespace Driver::DIO::RP2040;

static const Driver::DIO::InOut::Config_T*  inputCfg_;
static const Driver::DIO::InOut::Config_T*  outputCfg_;
static uint8_t                              numInputs_;
static uint8_t                              numOutputs_;
static bool                                 started_;


static void configGpio( unsigned pin, bool output, size_t pullsOptions );


///////////////////////////////////////////////////////////
bool Driver::DIO::InOut::start( uint8_t          numInputs,
                                const Config_T   inputCfg[],
                                uint8_t          numOutputs,
                                const Config_T   outputCfg[] )
{
    // Ignore if already started
    if ( !started_ )
    {
        // Enforce physical limits
        if ( numInputs + numOutputs > MAX_IO )
        {
            return false;
        }

        // Cache the configuration
        inputCfg_   = inputCfg;
        outputCfg_  = outputCfg;
        numInputs_  = numInputs;
        numOutputs_ = numOutputs;

        // TODO: Validate PIN Identifiers. Valid range and no duplicates

        // Configure inputs
        for ( uint8_t idx=0; idx < numInputs_; idx++ )
        {
            configGpio( inputCfg_[idx].pin, false, inputCfg_[idx].blob );
        }

        // Configure outputs
        for ( uint8_t idx=0; idx < numOutputs_; idx++ )
        {
            configGpio( outputCfg_[idx].pin, true, outputCfg_[idx].blob );
            gpio_put( outputCfg_[idx].pin, false );
        }

        started_ = true;
        return true;
    }

    return false;
}

void Driver::DIO::InOut::stop()
{
    started_ = false;
    
    // Release inputs
    for ( uint8_t idx=0; idx < numInputs_; idx++ )
    {
        gpio_deinit( inputCfg_[idx].pin );
    }

    // Release outputs
    for ( uint8_t idx=0; idx < numOutputs_; idx++ )
    {
        gpio_put( outputCfg_[idx].pin, false );
        gpio_deinit( outputCfg_[idx].pin );
    }
}

bool Driver::DIO::InOut::getOutput( uint8_t outputIndex, bool& assertedOut )
{
    if ( started_ && outputIndex < numOutputs_ )
    {
        assertedOut = gpio_get_out_level( outputCfg_[outputIndex].pin );
        return true;
    }

    return false;
}

bool Driver::DIO::InOut::setOutput( uint8_t outputIndex, bool asserted )
{
    if ( started_ && outputIndex < numOutputs_ )
    {
        gpio_put( outputCfg_[outputIndex].pin, asserted );
        return true;
    }

    return false;
}

bool Driver::DIO::InOut::getInput( uint8_t inputIndex, bool& assertedOut )
{
    if ( started_ && inputIndex < numInputs_ )
    {
        assertedOut = gpio_get( inputCfg_[inputIndex].pin );
        return true;
    }

    return false;
}

//////////////////////////////////////////
void configGpio( unsigned pin, bool output, size_t pullsOptions )
{
    gpio_init( pin );
    gpio_set_dir( pin, output );

    if ( (pullsOptions & INOUT_CFG_PULL_UPDOWN_MASK) == INOUT_CFG_PULL_UP )
    {
        gpio_pull_up( pin );
    }
    else if ( (pullsOptions & INOUT_CFG_PULL_UPDOWN_MASK) == INOUT_CFG_PULL_DOWN )
    {
        gpio_pull_down( pin );
    }
    else
    {
        gpio_disable_pulls( pin );
    }
}