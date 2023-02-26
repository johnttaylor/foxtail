/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Driver/Automation2040/Api.h"
#include "Cpl/System/Trace.h"
#include "ModelPoints.h"
#include <math.h>

#define SECT_                           "_0test"

#define OFF_PWM                         0.0F
#define ON_PWM                          100.0F

#define MAX_ADC_LED_VOLTAGE             45.0f

using namespace Driver::Automation2040;

static bool adc1LedEnabled_;
static bool adc2LedEnabled_;
static bool adc3LedEnabled_;

#define CLAMP_RANGE(v,lo,hi)    (v) > (hi) ? (hi) : (v) < (lo)? (lo): (v)
#define CLAMP_POSITIVE(v)       (v) < 0.0f? 0.0f: (v)

////////////////////////////////////////////////////////
void Driver::Automation2040::Api::initialize()
{
    // Initialize the ADC hardware
    adc1LedEnabled_ = false;
    adc2LedEnabled_ = false;
    adc3LedEnabled_ = false;
    
    // Default input values to 'zero'
    mp::sim2040_ainput1.write( 0.0F );
    mp::sim2040_ainput2.write( 0.0F );
    mp::sim2040_ainput3.write( 0.0F );
    mp::sim2040_adc1LedBrightness.write( 0.0F );
    mp::sim2040_adc2LedBrightness.write( 0.0F );
    mp::sim2040_adc3LedBrightness.write( 0.0F );
    
    mp::sim2040_dinput1.write( false );
    mp::sim2040_dinput2.write( false );
    mp::sim2040_dinput3.write( false );
    mp::sim2040_dinput4.write( false );

    mp::sim2040_buttonAPressed.write( false );
    mp::sim2040_buttonBPressed.write( false );

    // Default outputs
    mp::sim2040_doutput1.write( false );
    mp::sim2040_doutput2.write( false );
    mp::sim2040_doutput3.write( false );

    mp::sim2040_relay1.write( false );
    mp::sim2040_relay2.write( false );
    mp::sim2040_relay3.write( false );

    mp::sim2040_buttonALedBrightness.write( 0.0F );
    mp::sim2040_buttonBLedBrightness.write( 0.0F );

    mp::sim2040_connectedBrightness.write( 0.0F );

    // default board temperature (room temperature)
    mp::sim2040_boardTemp.write( 25.0F );
}


void Driver::Automation2040::Api::start()
{
    // Nothing needed
}

void Driver::Automation2040::Api::stop()
{
    setSafe();
}

void Driver::Automation2040::Api::setSafe()
{
    mp::sim2040_doutput1.write( false );
    mp::sim2040_doutput2.write( false );
    mp::sim2040_doutput3.write( false );

    mp::sim2040_relay1.write( false );
    mp::sim2040_relay2.write( false );
    mp::sim2040_relay3.write( false );

    mp::sim2040_buttonALedBrightness.write( 0.0F );
    mp::sim2040_buttonBLedBrightness.write( 0.0F );

    mp::sim2040_connectedBrightness.write( 0.0F );
}


void Driver::Automation2040::Api::setConnectedLED( bool on )
{
    setConnectedLED( on ? ON_PWM : OFF_PWM );
}

void Driver::Automation2040::Api::setConnectedLED( float brightness )
{
    brightness = CLAMP_RANGE( brightness, 0.0F, 100.0F );
    mp::sim2040_connectedBrightness.write( brightness );
}

bool Driver::Automation2040::Api::userButtonPressed( ButtonId_T button )
{
    bool pressed = false;
    if ( button == Driver::Automation2040::Api::eBOARD_BUTTON_A )
    {
        mp::sim2040_buttonAPressed.read( pressed );
    }
    else
    {
        mp::sim2040_buttonBPressed.read( pressed );
    }

    return pressed;
}

void Driver::Automation2040::Api::setButtonLED( ButtonId_T button, bool on )
{
    setButtonLED( button, on? ON_PWM: OFF_PWM );
}

void Driver::Automation2040::Api::setButtonLED( ButtonId_T button, float brightness )
{
    brightness = CLAMP_RANGE( brightness, 0.0F, 100.0F );
    if ( button == Driver::Automation2040::Api::eBOARD_BUTTON_A )
    {
        mp::sim2040_buttonALedBrightness.write( brightness );
    }
    else
    {
        mp::sim2040_buttonBLedBrightness.write( brightness );
    }
}

bool Driver::Automation2040::Api::getRelayState( RelayId_T relay )
{
    bool state = false;
    if ( relay == Driver::Automation2040::Api::eRELAY_1 )
    {
        mp::sim2040_relay1.read( state );
    }
    else if ( relay == Driver::Automation2040::Api::eRELAY_2 )
    {
        mp::sim2040_relay2.read( state );
    }
    else
    {
        mp::sim2040_relay3.read( state );
    }

    return state;
}

void Driver::Automation2040::Api::setRelayState( RelayId_T relay, bool actuated )
{
    if ( relay == Driver::Automation2040::Api::eRELAY_1 )
    {
        mp::sim2040_relay1.write( actuated );
    }
    else if ( relay == Driver::Automation2040::Api::eRELAY_2 )
    {
        mp::sim2040_relay2.write( actuated );
    }
    else
    {
        mp::sim2040_relay3.write( actuated );
    }
}

bool Driver::Automation2040::Api::getOutputState( DOutputId_T output )
{
    bool state = false;
    if ( output == Driver::Automation2040::Api::eDOUTPUT_1 )
    {
        mp::sim2040_doutput1.read( state );
    }
    else if ( output == Driver::Automation2040::Api::eDOUTPUT_2 )
    {
        mp::sim2040_doutput2.read( state );
    }
    else
    {
        mp::sim2040_doutput3.read( state );
    }

    return state;
}

void Driver::Automation2040::Api::setOutputState( DOutputId_T output, bool asserted )
{
    if ( output == Driver::Automation2040::Api::eDOUTPUT_1 )
    {
        mp::sim2040_doutput1.write( asserted );
    }
    else if ( output == Driver::Automation2040::Api::eDOUTPUT_2 )
    {
        mp::sim2040_doutput2.write( asserted );
    }
    else
    {
        mp::sim2040_doutput3.write( asserted );
    }
}

bool Driver::Automation2040::Api::getInputState( DInputId_T input )
{
    bool state = false;
    if ( input == Driver::Automation2040::Api::eDINPUT_1 )
    {
        mp::sim2040_dinput1.read( state );
    }
    else if ( input == Driver::Automation2040::Api::eDINPUT_2 )
    {
        mp::sim2040_dinput2.read( state );
    }
    else if ( input == Driver::Automation2040::Api::eDINPUT_3 )
    {
        mp::sim2040_dinput3.read( state );
    }
    else
    {
        mp::sim2040_dinput4.read( state );
    }

    return state;
}


float Driver::Automation2040::Api::getAnalogValue( AInputId_T adc )
{
    float value = 0.0F;
    if ( adc == Driver::Automation2040::Api::eAINPUT_1 )
    {
        mp::sim2040_ainput1.read( value );
        if ( adc1LedEnabled_ )
        {
            mp::sim2040_adc1LedBrightness.write( (value / MAX_ADC_LED_VOLTAGE) * ON_PWM );
        }
    }
    else if ( adc == Driver::Automation2040::Api::eAINPUT_2 )
    {
        mp::sim2040_ainput2.read( value );
        if ( adc2LedEnabled_ )
        {
            mp::sim2040_adc2LedBrightness.write( (value / MAX_ADC_LED_VOLTAGE) * ON_PWM );
        }
    }
    else
    {
        mp::sim2040_ainput3.read( value );
        if ( adc3LedEnabled_ )
        {
            mp::sim2040_adc3LedBrightness.write( (value / MAX_ADC_LED_VOLTAGE) * ON_PWM );
        }
    }

    return value;
}

void Driver::Automation2040::Api::setAdcLEDBehavior( AInputId_T adc, bool reflectADCValue )
{
    if ( adc == eAINPUT_1 )
    {
        adc1LedEnabled_ = reflectADCValue;
        if ( reflectADCValue == false )
        {
            mp::sim2040_adc1LedBrightness.write( OFF_PWM );
        }
    }
    else if ( adc == eAINPUT_2 )
    {
        adc2LedEnabled_ = reflectADCValue;
        if ( reflectADCValue == false )
        {
            mp::sim2040_adc2LedBrightness.write( OFF_PWM );
        }
    }
    else
    {
        adc3LedEnabled_ = reflectADCValue;
        if ( reflectADCValue == false )
        {
            mp::sim2040_adc3LedBrightness.write( OFF_PWM );
        }
    }
}

float Driver::Automation2040::Api::getBoardTemperature()
{
    float value = 0.0F;
    mp::sim2040_boardTemp.read( value );
    return value;
}

