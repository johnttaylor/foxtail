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
        , m_chassisbox( nullptr )
        , m_flushInProgress( false )
    {
    }


protected:
    /// This method should/must be called when the card is started
    void start( Cpl::Itc::PostApi&   chassisMbox )
    {
        m_chassisbox          = &chassisMbox;
        m_chassisOutputSeqNum = 0;
        m_driverOutputSeqNum  = 0;
    }

protected:
    /** Invokes an asynchronous ITC IoFlushRequest (executes in the Chassis thread)
        The child class should ONLY (actually must) call this method when there is 
        NEW output IO Register data available.
     */
    bool issueAsyncIoFlushRequest( bool incrementChassisSeqNum = true ) noexcept
    {
        // Default operation is to update the source's sequence number
        if ( incrementChassisSeqNum )
        {
            m_chassisOutputSeqNum++;
        }

        // Only allow one outstanding Flush request (and only send on change
        if ( !m_flushInProgress && m_chassisOutputSeqNum != m_driverOutputSeqNum )
        {
            popuplateOutputsTransferBuffer();
            IoFlushRequest::IoFlushPayload* payload = new(m_memoryIoFlushPayload.m_byteMem) IoFlushRequest::IoFlushPayload();
            IoFlushResponse::IoFlushRspMsg* msg     = new(m_memoryIoFlushRspMsg.m_byteMem) IoFlushResponse::IoFlushRspMsg( *this, *m_chassisbox, *this, *payload );
            m_flushInProgress                       = true;
            m_driverOutputSeqNum                    = m_chassisOutputSeqNum;
            m_driverbox.post( msg->getRequestMsg() );

            return true;
        }

        return false;
    }

    /** This populates the transfer buffer from the data stored in the Output
        IO Registers.  The concrete card is required to implement this method
     */
    virtual void popuplateOutputsTransferBuffer() noexcept = 0;

public:
    // ITC Request to update the driver with card data. NOTE: executes in the driver thread
    void request( IoFlushReqMsg& msg )
    {
        // Update the driver data
        extractOutputsTransferBuffer();

        // Complete the ITC transaction
        msg.returnToSender();
    }

    /** This populates the driver's data store from the output transfer buffer.  
        The concrete card is required to implement this method
     */
    virtual void extractOutputsTransferBuffer() noexcept = 0;

    // ITC Response: executes in the Chassis thread
    void response( IoFlushRspMsg& msg )
    {
        m_flushInProgress = false;

        // Start of new transfer if there is newer data (but don't increment the sequence number)
        issueAsyncIoFlushRequest( false );
    }

protected:
    /// The driver thread's mailbox reference
    Cpl::Dm::MailboxServer&  m_driverbox;

    /// The Chassis/card thread's mailbox reference
    Cpl::Itc::PostApi*       m_chassisbox;

    /// Memory for IoFlush Message
    Cpl::Memory::AlignedClass<IoFlushResponse::IoFlushRspMsg>  	m_memoryIoFlushRspMsg;

    /// Memory for the IoFlush Message Payload
    Cpl::Memory::AlignedClass<IoFlushRequest::IoFlushPayload>   m_memoryIoFlushPayload;

    /// Chassis sequence number for Output transfers
    uint16_t    m_chassisOutputSeqNum;

    /// Sequence number at the time the ITC request was made
    uint16_t    m_driverOutputSeqNum;

    /// Flush in progress
    bool        m_flushInProgress;
};


};      // end namespaces
};
#endif  // end header latch
