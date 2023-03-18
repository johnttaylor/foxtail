#ifndef Fxt_Card_StartStopAsync_h_
#define Fxt_Card_StartStopAsync_h_
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
#include "Fxt/Card/StartStopRequests.h"
#include "Cpl/Memory/Aligned.h"
#include <new>

///
namespace Fxt {
///
namespace Card {

/** This partially concrete class provide boiler plate code for ITC Start/Stop 
    calls.  The child class (aka the 'server') is responsible
    for provide the actual request( xxx msg ) methods.
 */
class StartStopAsync : 
    public StartRequest,
    public StopRequest,
    public StartResponse, 
    public StopResponse
{
protected:
    /// Constructor
    StartStopAsync( Cpl::Itc::PostApi& driverMbox ) noexcept
        : m_driverbox( driverMbox )
        , m_startInProgress( false )
        , m_stopInProgress( false )
    {
    }

    /// Invokes an asynchronous ITC StartRequest
    bool issueAsyncStartRequest( Cpl::Itc::PostApi& chassisMbox, uint64_t currentElapsedTimeUsec, void* optionalArgs=nullptr ) noexcept
    {
        // Only allow one outstanding start request
        if ( !m_startInProgress )
        {
            StartRequest::StartPayload* payload = new(m_memoryStartPayload.m_byteMem) StartRequest::StartPayload( currentElapsedTimeUsec, optionalArgs );
            StartResponse::StartRspMsg* msg     = new(m_memoryStartRspMsg.m_byteMem) StartResponse::StartRspMsg( *this, chassisMbox, *this, *payload );
            m_driverbox.post( msg->getRequestMsg() );
            return true;
        }

        return false;
    }

    /// Used to indicate the ITC transaction completed
    void startTransactionCompleted()
    {
        m_startInProgress = false;
    }

    /// Invokes an asynchronous ITC StopRequest
    bool issueAsyncStopRequest( Cpl::Itc::PostApi& chassisMbox, void* optionalArgs=nullptr ) noexcept
    {
        // Only allow one outstanding stop request
        if ( !m_stopInProgress )
        {
            // See comments above about the placement 'new' operations
            StopRequest::StopPayload* payload = new(m_memoryStartPayload.m_byteMem) StopRequest::StopPayload( optionalArgs );
            StopResponse::StopRspMsg* msg     = new(m_memoryStartRspMsg.m_byteMem) StopResponse::StopRspMsg( *this, chassisMbox, *this, *payload );
            m_driverbox.post( msg->getRequestMsg() );
            return true;
        }

        return false;
    }

    /// Used to indicate the ITC transaction completed
    void stopTransactionCompleted()
    {
        m_stopInProgress = false;
    }

protected:
    /// The driver thread's mailbox reference
    Cpl::Itc::PostApi&  m_driverbox;

    /// Memory for Start Message
    Cpl::Memory::AlignedClass<StartResponse::StartRspMsg>  	m_memoryStartRspMsg;

    /// Memory for the Start Message Payload
    Cpl::Memory::AlignedClass<StartRequest::StartPayload>   m_memoryStartPayload;

    /// Stop Message
    Cpl::Memory::AlignedClass<StopResponse::StopRspMsg>  	m_memoryStopRspMsg;
    
    /// Payload for Stop Message Payload
    Cpl::Memory::AlignedClass<StopRequest::StopPayload>     m_memoryStopPayload;

    /// Start in progress
    bool    m_startInProgress;

    /// Stop in progress
    bool    m_stopInProgress;
};


};      // end namespaces
};
#endif  // end header latch
