#ifndef Driver_Automation2040_Simulated_ModelPoints_h_
#define Driver_Automation2040_Simulated_ModelPoints_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Apps Project.  The Colony.Apps Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.apps/license.txt
*
* Copyright (c) 2015-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file 

    This file defines all of the Model Points for the Simulated Automatation2040
    driver.  
              
    All of the model points are placed into the 'mp' namespace.  The model
    point names map 1-to-1 with the instance names.

    The Application/Client is RESPONSIBLE for ensuring input values are with
    the defined range for the model point instance.
 */

#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/Bool.h"
#include "Cpl/Dm/Mp/Float.h"

/** Encapsulate all Model Points in the 'mp' namespace to prevent polluting
    the global name space
 */
namespace mp {

/// Simulated Analog input value.  Units are volts. Range is: 0 to 51.62V
extern Cpl::Dm::Mp::Float     sim2040_ainput1;

/// Simulated Analog input value.  Units are volts. Range is: 0 to 51.62V
extern Cpl::Dm::Mp::Float     sim2040_ainput2;

/// Simulated Analog input value.  Units are volts. Range is: 0 to 51.62V
extern Cpl::Dm::Mp::Float     sim2040_ainput3;


/// Simulated ADC LED. Brightness is a-function-of/set-by the ADC input voltage. Range: 0.0 to 100.0
extern Cpl::Dm::Mp::Float     sim2040_adc1LedBrightness;

/// Simulated ADC LED. Brightness is a-function-of/set-by the ADC input voltage. Range: 0.0 to 100.0
extern Cpl::Dm::Mp::Float     sim2040_adc2LedBrightness;

/// Simulated ADC LED. Brightness is a-function-of/set-by the ADC input voltage. Range: 0.0 to 100.0
extern Cpl::Dm::Mp::Float     sim2040_adc3LedBrightness;


/// Simulated DInput value
extern Cpl::Dm::Mp::Bool      sim2040_dinput1;

/// Simulated DInput value
extern Cpl::Dm::Mp::Bool      sim2040_dinput2;

/// Simulated DInput #1 value
extern Cpl::Dm::Mp::Bool      sim2040_dinput3;

/// Simulated DInput #1 value
extern Cpl::Dm::Mp::Bool      sim2040_dinput4;


/// Simulated button input
extern Cpl::Dm::Mp::Bool      sim2040_buttonAPressed;

/// Simulated button input
extern Cpl::Dm::Mp::Bool      sim2040_buttonBPressed;


/// Simulated Button LED (is an output). Brightness Range: 0.0 to 100.0
extern Cpl::Dm::Mp::Float     sim2040_buttonALedBrightness;

/// Simulated Button LED (is an output). Brightness Range: 0.0 to 100.0
extern Cpl::Dm::Mp::Float     sim2040_buttonBLedBrightness;


/// Simulated DOutput value
extern Cpl::Dm::Mp::Bool      sim2040_doutput1;

/// Simulated DOutput value
extern Cpl::Dm::Mp::Bool      sim2040_doutput2;

/// Simulated DOutput value
extern Cpl::Dm::Mp::Bool      sim2040_doutput3;


/// Simulated Relay value
extern Cpl::Dm::Mp::Bool      sim2040_relay1;

/// Simulated Relay value
extern Cpl::Dm::Mp::Bool      sim2040_relay2;

/// Simulated Relay value
extern Cpl::Dm::Mp::Bool      sim2040_relay3;


/// Simulated Connection LED (is an output). Brightness Range: 0.0 to 100.0
extern Cpl::Dm::Mp::Float     sim2040_connectedBrightness;


/// Simulated Board Temperature value.  Units are degrees Centigrade.
extern Cpl::Dm::Mp::Float     sim2040_boardTemp;

/*---------------------------------------------------------------------------*/
/// The Application's Model Point Database
extern Cpl::Dm::ModelDatabase   g_modelDatabase;


}       // end namespace
#endif  // end header latch
