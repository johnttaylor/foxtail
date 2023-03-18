#ifndef Fxt_Component_Analog_PopulateFactoryDb_h_
#define Fxt_Component_Analog_PopulateFactoryDb_h_
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

    This is header file is used to populate a Component Factory Database with
    all known (to this namespace) Components.  To use the application must
    define the symbol FXT_MY_APP_COMPONENT_FACTORY_DB with an instance of
    FactoryDatabaseApi.  This file should ONLY be include ONCE in a SINGLE file.
*/


#include "Fxt/Component/Analog/SimpleScaler8Factory.h"

static Fxt::Component::Analog::SimpleScaler8Factory         simpleScaler8Factory_( FXT_MY_APP_COMPONENT_FACTORY_DB );


#endif  // end header latch
