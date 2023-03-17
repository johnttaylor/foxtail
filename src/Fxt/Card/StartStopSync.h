#ifndef Fxt_Card_StartStopSync_h_
#define Fxt_Card_StartStopSync_h_
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

#include "Cpl/Itc/PostApi.h"
#include "Cpl/Itc/SyncReturnHandler.h"
#include "Fxt/Card/StartStopRequests.h"

///
namespace Fxt {
///
namespace Card {

/** This partially concrete class provide boiler plate code for Synchronous
    ITC Start/Stop calls.  The child class (aka the 'server') is responsible
    for provide the actual request( xxx msg ) methods.
 */
class StartStopSync : public StartRequest, public StopRequest
{
protected:
    /// Constructor
    StartStopSync( Cpl::Itc::PostApi& myMbox ) noexcept
        : m_mbox( myMbox )
    {
    }

    /// Invokes a synchronous ITC StartRequest
    bool issueStartRequest( uint64_t currentElapsedTimeUsec, void* optionalArgs=nullptr ) noexcept
    {
        StartRequest::StartPayload  payload( currentElapsedTimeUsec, optionalArgs );
        Cpl::Itc::SyncReturnHandler srh;
        StartRequest::StartMsg 	    msg( *this, payload, srh );
        m_mbox.postSync( msg );
        return payload.m_success;
    }


    /// Invokes a synchronous ITC StopRequest
    void issueStopRequest( void* optionalArgs=nullptr ) noexcept
    {
        StopRequest::StopPayload    payload( optionalArgs );
        Cpl::Itc::SyncReturnHandler srh;
        StopRequest::StopMsg 	    msg( *this, payload, srh );
        m_mbox.postSync( msg );
        return;
    }


protected:
    /// My mailbox reference
    Cpl::Itc::PostApi&  m_mbox;
};


};      // end namespaces
};
#endif  // end header latch
