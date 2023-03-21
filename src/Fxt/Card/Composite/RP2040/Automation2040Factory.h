#ifndef Fxt_Card_Composiste_RP2040_Factory_h_
#define Fxt_Card_Composiste_RP2040_Factory_h_
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

#include "Fxt/Card/FactoryCommon_.h"
#include "Fxt/Card/Composite/RP2040/Automation2040.h"

///
namespace Fxt {
///
namespace Card {
///
namespace Composite {
///
namespace RP2040 {


/// Define factory type
typedef Factory<Automation2040> Automation2040Factory;


}       // end namespaces
}
}
}
#endif  // end header latch
