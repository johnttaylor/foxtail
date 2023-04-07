#ifndef Fxt_Card_IoFlushAsync_h_
#define Fxt_Card_IoFlushAsync_h_
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

#include "Cpl/Dm/MailboxServer.h"
#include "Fxt/Card/IoFlushRequest.h"
#include "Cpl/Memory/Aligned.h"
#include <new>

///
namespace Fxt {
///
namespace Card {

/** This partially concrete class provides boiler plate code for ITC IO Flush
    calls.  The child class (aka the 'server') is responsible
    for provide the actual request( xxx msg ) methods.
 */
class IoFlushAsync : 
    public IoFlushRequest,
    public IoFlushResponse
{
protected:
    /// Constructor
    IoFlushAsync( Cpl::Dm::MailboxServer& driverMbox ) noexcept
        : m_driverbox( driverMbox )
        , m_driverOutputSeqNum( 0 )
        , m_flushInProgress( false )
    {
    }


protected:
    /// Invokes an asynchronous ITC IoFlushRequest (executes in the Chassis thread)
    bool issueAsyncIoFlushRequest( Cpl::Itc::PostApi&   chassisMbox, 
                                   uint16_t             currentChassisOutputSeqNum ) noexcept
    {
        // Only allow one outstanding Flush request (and only send on change
        if ( !m_flushInProgress && currentChassisOutputSeqNum != m_driverOutputSeqNum )
        {
            populateOutputTransferBuffer();
            IoFlushRequest::IoFlushPayload* payload = new(m_memoryIoFlushPayload.m_byteMem) IoFlushRequest::IoFlushPayload;
            IoFlushResponse::IoFlushRspMsg* msg     = new(m_memoryIoFlushRspMsg.m_byteMem) IoFlushResponse::IoFlushRspMsg( *this, chassisMbox, *this, *payload );
            m_flushInProgress                       = true;
            m_driverOutputSeqNum                    = currentChassisOutputSeqNum;
            m_driverbox.post( msg->getRequestMsg() );

            return true;
        }

        return false;
    }

    /** Populates the transfer buffer from the cards cached output buffer.
      */
    virtual void populateOutputTransferBuffer() noexcept = 0;

protected:
    /// Used to indicate the ITC transaction completed (executes in the Chassis thread)
    void flushTransactionCompleted( Cpl::Itc::PostApi&   chassisMbox,
                                    uint16_t             currentChassisOutputSeqNum )
    {
        m_flushInProgress = false;
        issueAsyncIoFlushRequest( chassisMbox, currentChassisOutputSeqNum );    // Start of new transfer if there is newer data
    }

protected:
    /// The driver thread's mailbox reference
    Cpl::Dm::MailboxServer&  m_driverbox;

    /// Memory for IoFlush Message
    Cpl::Memory::AlignedClass<IoFlushResponse::IoFlushRspMsg>  	m_memoryIoFlushRspMsg;

    /// Memory for the IoFlush Message Payload
    Cpl::Memory::AlignedClass<IoFlushRequest::IoFlushPayload>   m_memoryIoFlushPayload;

    /// Sequence number at the time the ITC request was made
    uint16_t    m_driverOutputSeqNum;

    /// Flush in progress
    bool        m_flushInProgress;
};


};      // end namespaces
};
#endif  // end header latch
