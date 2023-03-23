#ifndef Fxt_Component_Digital_PopulateFactoryDb_h_
#define Fxt_Component_Digital_PopulateFactoryDb_h_
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


#include "Fxt/Component/Digital/And64GateFactory.h"
#include "Fxt/Component/Digital/Not64GateFactory.h"
#include "Fxt/Component/Digital/DemuxFactory.h"
#include "Fxt/Component/Digital/MuxFactory.h"

static Fxt::Component::Digital::And64GateFactory         and64GateFactory_( FXT_MY_APP_COMPONENT_FACTORY_DB );
static Fxt::Component::Digital::Not64GateFactory         not64GateFactory_( FXT_MY_APP_COMPONENT_FACTORY_DB );
static Fxt::Component::Digital::DemuxFactory             demuxFactory_( FXT_MY_APP_COMPONENT_FACTORY_DB );
static Fxt::Component::Digital::MuxFactory               muxFactory_( FXT_MY_APP_COMPONENT_FACTORY_DB );


#endif  // end header latch
