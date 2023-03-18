#ifndef Fxt_Chassis_ServerApi_h_
#define Fxt_Chassis_ServerApi_h_
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


#include "Cpl/Itc/CloseSync.h"
#include "Fxt/System/PeriodApi.h"

///
namespace Fxt {
///
namespace Chassis {

/** This struct is used to pass the Period arrays used for the Chassis's
    periodic scheduling
 */
struct ChassisPeriods_T
{
    Fxt::System::PeriodApi**    inputPeriods;       //!< Variable length array of Input periods.  End of array is marked using a nullptr
    Fxt::System::PeriodApi**    executionPeriods;   //!< Variable length array of Execution periods.  End of array is marked using a nullptr
    Fxt::System::PeriodApi**    outputPeriods;      //!< Variable length array of Output periods.  End of array is marked using a nullptr
};

/** This class defines the public interface for start/stopping a Chassis 
    Server.

    Note: The public method are the generic open()/close method.  However,
          calling the open() method, a pointer to a populated ChassisPeriods_T
          instance must be passed on the 'args' argument.
 */
class ServerApi : public Cpl::Itc::CloseSync
{
public:
    /// Constructor  
    ServerApi( Cpl::Itc::PostApi& myMbox ) 
        : Cpl::Itc::CloseSync( myMbox )
    {
    }

public:
    /// Public accessor
    Cpl::Itc::PostApi& getMailbox() noexcept
    {
        reutrn m_mbox;
    }
};


};      // end namespaces
};
#endif  // end header latch
