#ifndef Fxt_Card_Sensor_I2C_RHTemperatureFactory_h_
#define Fxt_Card_Sensor_I2C_RHTemperatureFactory_h_
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
#include "Fxt/Card/Sensor/I2C/RHTemperature.h"

///
namespace Fxt {
///
namespace Card {
///
namespace Sensor {
///
namespace I2C {


/// Factory class for the RH/Temperature card
class RHTemperatureFactory : public Fxt::Card::Factory<RHTemperature>
{
public:
    /// Constructor -->require a Mailbox (and no extra args used)
    RHTemperatureFactory( FactoryDatabaseApi&  factoryDatabase, Cpl::Dm::MailboxServer* cardMbox )
        : Fxt::Card::Factory<RHTemperature>( factoryDatabase, cardMbox )
    {
    }
};



};      // end namespaces
};
};
};
#endif  // end header latch
