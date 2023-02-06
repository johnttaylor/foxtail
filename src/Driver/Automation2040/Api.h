#ifndef Driver_Automation2040_Api_h_
#define Driver_Automation2040_Api_h_
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


#include "colony_config.h"

///
namespace Driver {
///
namespace Automation2040 {

/** This class defines an interface for accessing the hardware bits (used for
    automation) of the Pimoroni Automation2040W board.

    The class is NOT thread safe.  The application is RESPONSIBLE for ensuring
    thread-safe usage.
 */
class Api
{
public:
    /// Available user buttons
    enum ButtonId_T
    {
        eBOARD_BUTTON_A = 12,   //!< On board user Button labeled 'A'
        eBOARD_BUTTON_B = 13    //!< On board user Button labeled 'A'
    };

    /// Available Relays (2A up to 24V, 1A up to 40V)
    enum RelayId_T
    {
        eRELAY_1 = 9,           //!< The on-board relay labeled with '1'
        eRELAY_2 = 10,          //!< The on-board relay labeled with '2'
        eRELAY_3 = 11           //!< The on-board relay labeled with '3'
    };

    /** Available current sourcing Digital Output signals
            o 4A max continuous current
            o 2A max current at 500Hz PWM
     */
    enum DOutputId_T
    {
        eDOUTPUT_1 = 16,        //!< The on-board output signal labeled with '1'
        eDOUTPUT_2 = 17,        //!< The on-board output signal labeled with '2'
        eDOUTPUT_3 = 18         //!< The on-board output signal labeled with '3'
    };

    /// Available buffer Digital Input signals (40V tolerant)
    enum DInputId_T
    {
        eDINPUT_1 = 19,         //!< The on-board buffered input signal labeled with '1'
        eDINPUT_2 = 20,         //!< The on-board buffered input signal labeled with '2'
        eDINPUT_3 = 21,         //!< The on-board buffered input signal labeled with '3'
        eDINPUT_4 = 22          //!< The on-board buffered input signal labeled with '4'
    };

    /// Available Analog Input signals (up to 40V)
    enum AInputId_T
    {
        eAINPUT_1 = 0,          //!< The on-board ADC signal labeled with '1'
        eAINPUT_2 = 1,          //!< The on-board ADC signal labeled with '2'
        eAINPUT_3 = 2,          //!< The on-board ADC signal labeled with '3'
    };

public:
    /** This method initializes the driver.  It must be called only ONCE and
        BEFORE any of the other methods are called
     */
    static void initialize();

    /** This method puts the driver into a known safe state, i.e. deactivates 
        all outputs, releases all relays, and turns off all user-controllable 
        LEDs.
     */
    static void setSafe();

public:
    /** This method controls the on/off state of the 'connected' LED (upper right
        corner of the board). When set to the true, the LED is on at full 
        brightness level; when set to false the LED is off
     */
    static void setConnectedLED( bool on );

    /** Similar to the above method, except the brightness (0.0 to 100.0) is
        specified.
     */
    static void setConnectedLED( float brightness );


public:
    /** Returns true if the button is currently pressed.  This is raw sampling
        of the button signal state, i.e. no SW debouncing of the signal
     */
    static bool userButtonPressed( ButtonId_T button);

    /** This method controls the on/off state of the LED associated with a
        button.  When set to the true, the LED is on at full brightness level; 
        when set to false the LED is off
     */
    static void setButtonLED( ButtonId_T button, bool on );

    /** Similar to the above method, except the brightness (0.0 to 100.0) is
        specified.
     */
    static void setButtonLED( ButtonId_T button, float brightness );

public:
    /// Returns the true if the specified relay is in the actuated state
    static bool getRelayState( RelayId_T relay );

    /// Sets the state of the specified relay. 
    static void setRelayState( RelayId_T relay, bool actuated );

    /// Convenience method
    static inline void actuateRelay( RelayId_T relay )
    {
        setRelayState( relay, true );
    }
    
    /// Convenience method
    static inline void releaseRelay( RelayId_T relay )
    {
        setRelayState( relay, false );
    }

public:
    /** Returns true if the current commanded state of the specified DO signal 
        is in the asserted state (i.e. physically high)
     */
    static bool getOutputState( DOutputId_T output );

    /// Sets the state of the specified DO signal
    static void setOutputState( DOutputId_T output, bool asserted );

    /// Convenience method
    static inline void setOutputOn( DOutputId_T output )
    {
        setOutputState( output, true );
    }

    /// Convenience method
    static inline void setOutputOff( DOutputId_T output )
    {
        setOutputState( output, false );
    }

public:
    /// Returns true if the specified DI signal is asserted (i.e. physically high)
    static bool getInputState( DInputId_T input );

public:
    /// Returns the voltage (in volts) 
    static float getAnalogValue( AInputId_T adc );

    /** This method enables/disables setting the brightness of the ADC LEDs to 
        proportional to the last ADC value read.  The default is that the LED
        are off.  For maximum sampling rate - disable the LEDs
     */
    static void setAdcLEDBehavior( bool reflectADCValue );

    /// Convenience method
    static inline void disableAdcLEDs()
    {
        setAdcLEDBehavior( false );
    }

    /// Convenience method
    static inline void enableAdcLEDs()
    {
        setAdcLEDBehavior( true );
    }

};

} // End namespace(s)
}

/*--------------------------------------------------------------------------*/
#endif  // end header latch
