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
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "Cpl/System/Trace.h"
#include <math.h>

#define SECT_                           "_0test"

#define MAX_PWM                         65535
#define OFF_PWM                         0
#define ON_PWM                          MAX_PWM

#define CONN_LED_PIN                    3

#define ONBOARD_TEMP_SENSOR_ADC_CHANNEL 4

#define ADC_PIN_TO_LED_PIN(n)           ((n)-20)
#define ADC_PIN_TO_ADC_CHANNEL(n)       ((n)-26)
#define BUTTON_PIN_TO_LED_PIN(n)        ((n)+2)
#define ADC_REF_VOLTAGE                 3.3f
#define VOLTAGE_GAIN                    0.06f
#define VOLTAGE_OFFSET                  -0.06f
#define MAX_ADC_LED_VOLTAGE             45.0f

#define VOLTS_TO_CENTRIGRADE(v)         (27 - (((v) - 0.706)/ 0.001721))

using namespace Driver::Automation2040;

static bool adc1LedEnabled_;
static bool adc2LedEnabled_;
static bool adc3LedEnabled_;

static void configureDOutputPin( unsigned pin );
static void configurePwmOutputPin( unsigned pin );
static void configureDInputPin( unsigned pin );
static void configureDInputPinWithPullup( unsigned pin );
static void configureAdc( unsigned pin );

#define CLAMP_RANGE(v,lo,hi)    (v) > (hi) ? (hi) : (v) < (lo)? (lo): (v)
#define CLAMP_POSITIVE(v)       (v) < 0.0f? 0.0f: (v)

////////////////////////////////////////////////////////
void Driver::Automation2040::Api::initialize()
{
    // Initialize the ADC hardware
    adc_init();
    adc1LedEnabled_ = false;
    adc2LedEnabled_ = false;
    adc3LedEnabled_ = false;
    
    // Set up the relay pins
    configureDOutputPin( eRELAY_1 );
    configureDOutputPin( eRELAY_2 );
    configureDOutputPin( eRELAY_3 );

    // Set up the output pins
    configureDOutputPin( eDOUTPUT_1 );
    configureDOutputPin( eDOUTPUT_2 );
    configureDOutputPin( eDOUTPUT_3 );

    // Set up the input pins
    configureDInputPin( eDINPUT_1 );
    configureDInputPin( eDINPUT_2 );
    configureDInputPin( eDINPUT_3 );

    // Set up ADC inputs
    configureAdc( eAINPUT_1 );
    configureAdc( eAINPUT_2 );
    configureAdc( eAINPUT_3 );

    // Set up the ADC leds
    configurePwmOutputPin( ADC_PIN_TO_LED_PIN( eAINPUT_1 ) );
    configurePwmOutputPin( ADC_PIN_TO_LED_PIN( eAINPUT_2 ) );
    configurePwmOutputPin( ADC_PIN_TO_LED_PIN( eAINPUT_3 ) );

    // Set up the user switches
    configureDInputPinWithPullup( eBOARD_BUTTON_A );
    configureDInputPinWithPullup( eBOARD_BUTTON_B );

    // Set up the user LEDs
    configurePwmOutputPin( BUTTON_PIN_TO_LED_PIN( eBOARD_BUTTON_A ) );
    configurePwmOutputPin( BUTTON_PIN_TO_LED_PIN( eBOARD_BUTTON_B ) );

    // Set up the connectivity LED
    configurePwmOutputPin( CONN_LED_PIN );
}
void Driver::Automation2040::Api::start()
{
    adc_set_temp_sensor_enabled( true );
}

void Driver::Automation2040::Api::stop()
{
    adc_set_temp_sensor_enabled( false );
    setSafe();
}

void Driver::Automation2040::Api::setSafe()
{
    // Reset the relays
    releaseRelay( eRELAY_1 );
    releaseRelay( eRELAY_2 );
    releaseRelay( eRELAY_3 );

    // Reset the outputs
    setOutputOff( eDOUTPUT_1 );
    setOutputOff( eDOUTPUT_2 );
    setOutputOff( eDOUTPUT_3 );

    // Reset the ADC LEDs
    pwm_set_gpio_level( ADC_PIN_TO_LED_PIN( eAINPUT_1 ), 0 );
    pwm_set_gpio_level( ADC_PIN_TO_LED_PIN( eAINPUT_2 ), 0 );
    pwm_set_gpio_level( ADC_PIN_TO_LED_PIN( eAINPUT_3 ), 0 );

    // Reset the switch LEDs
    pwm_set_gpio_level( BUTTON_PIN_TO_LED_PIN( eBOARD_BUTTON_A ), 0 );
    pwm_set_gpio_level( BUTTON_PIN_TO_LED_PIN( eBOARD_BUTTON_B ), 0 );

    // Reset the connectivity LED
    pwm_set_gpio_level( CONN_LED_PIN, 0 );
}


void Driver::Automation2040::Api::setConnectedLED( bool on )
{
    pwm_set_gpio_level( CONN_LED_PIN, on ? ON_PWM : OFF_PWM );
}

void Driver::Automation2040::Api::setConnectedLED( float brightness )
{
    brightness        = CLAMP_RANGE( brightness, 0.0f, 100.0f );
    const float gamma = 2.8f;
    uint16_t    value = (uint16_t) (powf( brightness / 100.0f, gamma ) * MAX_PWM + 0.5f);
    pwm_set_gpio_level( CONN_LED_PIN, value );
}

bool Driver::Automation2040::Api::userButtonPressed( ButtonId_T button )
{
    return !gpio_get( button );
}

void Driver::Automation2040::Api::setButtonLED( ButtonId_T button, bool on )
{
    pwm_set_gpio_level( button, on ? ON_PWM : OFF_PWM );
}

void Driver::Automation2040::Api::setButtonLED( ButtonId_T button, float brightness )
{
    brightness        = CLAMP_RANGE( brightness, 0.0f, 100.0f );
    const float gamma = 2.8f;
    uint16_t    value = (uint16_t) (powf( brightness / 100.0f, gamma ) * MAX_PWM + 0.5f);
    pwm_set_gpio_level( BUTTON_PIN_TO_LED_PIN(button), value );
}

bool Driver::Automation2040::Api::getRelayState( RelayId_T relay )
{
    return gpio_get( relay );
}

void Driver::Automation2040::Api::setRelayState( RelayId_T relay, bool actuated )
{
    gpio_put( relay, actuated );
}

bool Driver::Automation2040::Api::getOutputState( DOutputId_T output )
{
    return gpio_get( output );
}

void Driver::Automation2040::Api::setOutputState( DOutputId_T output, bool asserted )
{
    gpio_put( output, asserted );
}

bool Driver::Automation2040::Api::getInputState( DInputId_T input )
{
    return gpio_get( input );
}

static void setADCLedState( int adcPin, float volts )
{
    const float gamma = 2.8f;
    uint16_t    value = (uint16_t) (powf( volts / MAX_ADC_LED_VOLTAGE, gamma ) * MAX_PWM + 0.5f);
    pwm_set_gpio_level( adcPin, value );
}


float Driver::Automation2040::Api::getAnalogValue( AInputId_T adc )
{
    adc_select_input( ADC_PIN_TO_ADC_CHANNEL(adc) );
    uint16_t adcBits = adc_read();
    float    volts   = CLAMP_POSITIVE( (((adcBits * ADC_REF_VOLTAGE) / (1 << 12)) + VOLTAGE_OFFSET) / VOLTAGE_GAIN );

    if ( adc1LedEnabled_ )
    {
        setADCLedState( ADC_PIN_TO_LED_PIN( adc ), volts );
    }
    else if ( adc2LedEnabled_ )
    {
        setADCLedState( ADC_PIN_TO_LED_PIN( adc ), volts );
    }
    else if ( adc3LedEnabled_ )
    {
        setADCLedState( ADC_PIN_TO_LED_PIN( adc ), volts );
    }

    return volts;
}

void Driver::Automation2040::Api::setAdcLEDBehavior( AInputId_T adc, bool reflectADCValue )
{
    if ( adc == eAINPUT_1 )
    {
        adc1LedEnabled_ = reflectADCValue;
    }
    else if ( adc == eAINPUT_2 )
    {
        adc2LedEnabled_ = reflectADCValue;
    }
    else
    {
        adc3LedEnabled_ = reflectADCValue;
    }

    if ( reflectADCValue == false )
    {
        pwm_set_gpio_level( ADC_PIN_TO_LED_PIN( adc ), OFF_PWM );
    }
}

float Driver::Automation2040::Api::getBoardTemperature()
{
    // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
    const float conversion_factor = 3.3f / (1 << 12);

    adc_select_input( ONBOARD_TEMP_SENSOR_ADC_CHANNEL );
    uint16_t result = adc_read();
    return VOLTS_TO_CENTRIGRADE( result * conversion_factor );
}

//////////////////////////////
void configureDOutputPin( unsigned pin )
{
    gpio_init( pin );
    gpio_set_dir( pin, GPIO_OUT );
    gpio_put( pin, false );
}

void configurePwmOutputPin( unsigned pin )
{
    gpio_put( pin, false );
    pwm_config cfg = pwm_get_default_config();
    pwm_set_wrap( pwm_gpio_to_slice_num( pin ), MAX_PWM );
    pwm_init( pwm_gpio_to_slice_num( pin ), &cfg, true );
    gpio_set_function( pin, GPIO_FUNC_PWM );
    pwm_set_gpio_level( pin, OFF_PWM );
}

void configureDInputPin( unsigned pin )
{
    gpio_init( pin );
    gpio_set_dir( pin, GPIO_IN );
}

void configureDInputPinWithPullup( unsigned pin )
{
    configureDInputPin( pin );
    gpio_pull_up( pin );
}

void configureAdc( unsigned pin )
{
    adc_gpio_init( pin );
}