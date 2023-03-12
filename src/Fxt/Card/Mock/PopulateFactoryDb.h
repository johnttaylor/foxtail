#ifndef Fxt_Card_Mock_PopulateFactoryDb_h_
#define Fxt_Card_Mock_PopulateFactoryDb_h_
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

    This is header file is used to populate a Card Factory Database with
    all known (to this namespace) Cards.  To use the application must
    define the symbol FXT_MY_APP_CARD_FACTORY_DB with an instance of
    FactoryDatabaseApi.  This file should ONLY be include ONCE in a SINGLE file.
*/


#include "Fxt/Card/Mock/AnalogIn8Factory.h"
#include "Fxt/Card/Mock/Digital8Factory.h"

static Fxt::Card::Mock::AnalogIn8Factory    analogIn8Factory_( FXT_MY_APP_CARD_FACTORY_DB );
static Fxt::Card::Mock::Digital8Factory     digital8Factory_( FXT_MY_APP_CARD_FACTORY_DB );

#endif  // end header latch
