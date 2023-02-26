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

#include "ModelPoints.h"


// Helper macros to simplify the instantiation of the MP instances
#define ALLOC_INVALID( n, t )           t mp::n(  mp::g_modelDatabase, (#n) );  

////////////////////////////////////////////////////////////////////////////////

ALLOC_INVALID( sim2040_ainput1, Cpl::Dm::Mp::Float )
ALLOC_INVALID( sim2040_ainput2, Cpl::Dm::Mp::Float )
ALLOC_INVALID( sim2040_ainput3, Cpl::Dm::Mp::Float )

ALLOC_INVALID( sim2040_adc1LedBrightness, Cpl::Dm::Mp::Float )
ALLOC_INVALID( sim2040_adc2LedBrightness, Cpl::Dm::Mp::Float )
ALLOC_INVALID( sim2040_adc3LedBrightness, Cpl::Dm::Mp::Float )

ALLOC_INVALID( sim2040_dinput1, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( sim2040_dinput2, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( sim2040_dinput3, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( sim2040_dinput4, Cpl::Dm::Mp::Bool )

ALLOC_INVALID( sim2040_buttonAPressed, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( sim2040_buttonBPressed, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( sim2040_buttonALedBrightness, Cpl::Dm::Mp::Float )
ALLOC_INVALID( sim2040_buttonBLedBrightness, Cpl::Dm::Mp::Float )

ALLOC_INVALID( sim2040_doutput1, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( sim2040_doutput2, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( sim2040_doutput3, Cpl::Dm::Mp::Bool )

ALLOC_INVALID( sim2040_relay1, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( sim2040_relay2, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( sim2040_relay3, Cpl::Dm::Mp::Bool )

ALLOC_INVALID( sim2040_connectedBrightness, Cpl::Dm::Mp::Float )

ALLOC_INVALID( sim2040_boardTemp, Cpl::Dm::Mp::Float )
