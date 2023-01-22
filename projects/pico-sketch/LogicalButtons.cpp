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

#include "Driver/PicoDisplay/Api.h" // Must be first #include (because of the Pimoroni/Pico SDK)
#include "app.h"
#include "LogicalButtons.h"
#include "ModelPoints.h"
#include "Cpl/System/Trace.h"

#define STATE_IDLE                  0
#define STATE_KEYPRESSED            1    
#define STATE_HELD_DOWN             2
#define STATE_WAITING_ALL_RELEASED  3

#define EDGE_NONE                   0
#define EDGE_PRESSED                -1
#define EDGE_RELEASED               1
#define EDGE_TIME                   2   // Pseudo edge

static int detectEdges( bool currentPressedState, bool& prevPressedSate );
static Cpl::Dm::Mp::Bool* getLogicalKey( int edgeA, int edgeB, int edgeX, int edgeY );
static void generateKeyEvent( Cpl::Dm::Mp::Bool* keyModelPoint );

static unsigned             delayCounter_;
static bool                 buttonAPressed_;
static bool                 buttonBPressed_;
static bool                 buttonXPressed_;
static bool                 buttonYPressed_;
static bool                 releaseEvent_;
static bool                 pressedEvent_;
static bool                 isSingleKeyEvent_;
static unsigned             state_;
static Cpl::Dm::Mp::Bool*   latchedKey_;

/*---------------------------------------------------------------------------*/
void intializeLogicalButtons()
{
    // Initialize the model points
    mp::buttonEventAlt.write( false );
    mp::buttonEventRight.write( false );
    mp::buttonEventLeft.write( false );
    mp::buttonEventUp.write( false );
    mp::buttonEventUp.write( false );
    mp::buttonEventSize.write( false );
    mp::buttonEventColor.write( false );
    mp::buttonEventClear.write( false );

    // Housekeeping
    state_          = STATE_IDLE;
    buttonAPressed_ = false;
    buttonBPressed_ = false;
    buttonXPressed_ = false;
    buttonYPressed_ = false;
    releaseEvent_   = false;
    pressedEvent_   = false;
}

void processLogicalButtons( Cpl::System::ElapsedTime::Precision_T currentTick,
                            Cpl::System::ElapsedTime::Precision_T currentInterval )
{
    // De-bounce the button Inputs
    Driver::PicoDisplay::Api::buttonA().sample();
    Driver::PicoDisplay::Api::buttonB().sample();
    Driver::PicoDisplay::Api::buttonX().sample();
    Driver::PicoDisplay::Api::buttonY().sample();

    // Detect release events
    releaseEvent_ = false;
    pressedEvent_ = false;
    int edgeA     = detectEdges( Driver::PicoDisplay::Api::buttonA().isPressed(), buttonAPressed_ );
    int edgeB     = detectEdges( Driver::PicoDisplay::Api::buttonB().isPressed(), buttonBPressed_ );
    int edgeX     = detectEdges( Driver::PicoDisplay::Api::buttonX().isPressed(), buttonXPressed_ );
    int edgeY     = detectEdges( Driver::PicoDisplay::Api::buttonY().isPressed(), buttonYPressed_ );

    // Convert to button events
    isSingleKeyEvent_ = true;
    switch ( state_ )
    {
    case STATE_IDLE:
        if ( pressedEvent_ )
        {
            delayCounter_ = OPTION_MY_APP_LOGICAL_BUTTON_HOLD_COUNT;
            state_        = STATE_KEYPRESSED;
        }
        break;

    case STATE_KEYPRESSED:
        if ( releaseEvent_ )
        {
            generateKeyEvent( getLogicalKey( edgeA, edgeB, edgeX, edgeY ) );
            state_ = isSingleKeyEvent_ ? STATE_IDLE : STATE_WAITING_ALL_RELEASED;
        }
        else if ( --delayCounter_ == 0 )
        {
            latchedKey_   = getLogicalKey( EDGE_TIME, edgeB, edgeX, edgeY );
            generateKeyEvent( latchedKey_ );
            delayCounter_ = OPTION_MY_APP_LOGICAL_BUTTON_REPEAT_COUNT;
            state_        = STATE_HELD_DOWN;
        }
        break;

    case STATE_WAITING_ALL_RELEASED:
        if ( !Driver::PicoDisplay::Api::buttonB().isPressed() &&
             !Driver::PicoDisplay::Api::buttonX().isPressed() &&
             !Driver::PicoDisplay::Api::buttonY().isPressed() )
        {
            state_ = STATE_IDLE;
        }
        break;

    case STATE_HELD_DOWN:
        if ( releaseEvent_ )
        {
            generateKeyEvent( latchedKey_ );
            state_ = isSingleKeyEvent_ ? STATE_IDLE : STATE_WAITING_ALL_RELEASED;
        }
        
        // Do NOT allow repeat event for the ALT key
        else if ( latchedKey_ != &mp::buttonEventAlt )
        {
            if ( --delayCounter_ == 0 )
            {
                generateKeyEvent( latchedKey_ );
                delayCounter_ = OPTION_MY_APP_LOGICAL_BUTTON_REPEAT_COUNT;
            }
        }
        break;
    }
}

/*---------------------------------------------------------------------------*/
int detectEdges( bool currentPressedState, bool& prevPressedSate )
{
    int edge = EDGE_NONE;
    if ( currentPressedState && !prevPressedSate )
    {
        pressedEvent_ = true;
        edge          = EDGE_PRESSED;
    }
    else if ( !currentPressedState && prevPressedSate )
    {
        releaseEvent_ = true;
        edge          = EDGE_RELEASED;
    }
    prevPressedSate = currentPressedState;
    return edge;
}

static Cpl::Dm::Mp::Bool*lookUpLogicalKey( bool pressedA, bool pressedB, bool pressedX, bool pressedY )
{
    unsigned mask = (pressedA ? 1 : 0) | ((pressedB ? 1 : 0) << 1) | ((pressedX ? 1 : 0) << 2) | ((pressedY ? 1 : 0) << 3);
    switch ( mask )
    {
    case 0b0001: return &mp::buttonEventAlt;
    case 0b0010: return &mp::buttonEventColor;
    case 0b0011: isSingleKeyEvent_ = false;  return &mp::buttonEventSize;
    case 0b0100: return &mp::buttonEventRight;
    case 0b0101: isSingleKeyEvent_ = false;  return &mp::buttonEventLeft;
    case 0b1000: return &mp::buttonEventDown;
    case 0b1001: isSingleKeyEvent_ = false;  return &mp::buttonEventUp;
    case 0b0111: isSingleKeyEvent_ = false;  return &mp::buttonEventClear;
    default:
        return nullptr;
    }
}


Cpl::Dm::Mp::Bool* getLogicalKey( int edgeA, int edgeB, int edgeX, int edgeY )
{
    if ( edgeA == EDGE_RELEASED )
    {
        return lookUpLogicalKey( true, Driver::PicoDisplay::Api::buttonB().isPressed(), Driver::PicoDisplay::Api::buttonX().isPressed(), Driver::PicoDisplay::Api::buttonY().isPressed() );
    }
    else if ( edgeB == EDGE_RELEASED )
    {
        return lookUpLogicalKey( Driver::PicoDisplay::Api::buttonA().isPressed(), true, Driver::PicoDisplay::Api::buttonX().isPressed(), Driver::PicoDisplay::Api::buttonY().isPressed() );
    }
    else if ( edgeX == EDGE_RELEASED )
    {
        return lookUpLogicalKey( Driver::PicoDisplay::Api::buttonA().isPressed(), Driver::PicoDisplay::Api::buttonB().isPressed(), true, Driver::PicoDisplay::Api::buttonY().isPressed() );
    }
    else if ( edgeY == EDGE_RELEASED )
    {
        return lookUpLogicalKey( Driver::PicoDisplay::Api::buttonA().isPressed(), Driver::PicoDisplay::Api::buttonB().isPressed(), Driver::PicoDisplay::Api::buttonX().isPressed(), true );
    }
    else if ( edgeA == EDGE_TIME )
    {
        return lookUpLogicalKey( Driver::PicoDisplay::Api::buttonA().isPressed(), Driver::PicoDisplay::Api::buttonB().isPressed(), Driver::PicoDisplay::Api::buttonX().isPressed(), Driver::PicoDisplay::Api::buttonY().isPressed() );
    }

    return nullptr;
}

void generateKeyEvent( Cpl::Dm::Mp::Bool * keyModelPoint )
{
    if ( keyModelPoint )
    {
        keyModelPoint->write( true );
        keyModelPoint->touch(); // Ensure the is always a change notification
    }
}
