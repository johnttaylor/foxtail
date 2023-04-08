#ifndef Fxt_Card_IoScanAsync_h_
#define Fxt_Card_IoScanAsync_h_
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
#include "Fxt/Card/IoScanRequest.h"
#include "Cpl/Memory/Aligned.h"
#include <new>

///
namespace Fxt {
///
namespace Card {

/** This partially concrete class provides boiler plate code for ITC IO Scan
    calls.  The child class (aka the 'server') is responsible
    for provide the actual request( xxx msg ) methods.
 */
class IoScanAsync : 
    public IoScanRequest,
    public IoScanResponse
{
protected:
    /// Constructor
    IoScanAsync( Cpl::Dm::MailboxServer& driverMbox ) noexcept
        : m_driverbox( driverMbox )
        , m_chassisbox( nullptr )
        , m_scanInProgress( false )
    {
    }

protected:
    /// This method should/must be called when the card is started
    void start( Cpl::Itc::PostApi&   chassisMbox )
    {
        m_chassisbox         = &chassisMbox;
        m_chassisInputSeqNum = 0;
        m_driverInputsSeqNum = 0; 
    }

protected:
    /** Invokes an asynchronous ITC IoScanRequest (executes in the driver thread)
        The child class should ONLY (actually must) call this method when there is
        NEW set of Input data available.
     */
    bool issueAsyncIoScanRequest( bool incrementDriverSeqNum = true ) noexcept
    {
        // Default operation is to update the source's sequence number
        if ( incrementDriverSeqNum )
        {
            m_driverInputsSeqNum++;
        }

        // Only allow one outstanding Scan request (and only send on change)
        if ( !m_scanInProgress && m_driverInputsSeqNum != m_chassisInputSeqNum )
        {
            populateInputsTransferBuffer();  
            IoScanRequest::IoScanPayload* payload = new(m_memoryIoScanPayload.m_byteMem) IoScanRequest::IoScanPayload;
            IoScanResponse::IoScanRspMsg* msg     = new(m_memoryIoScanRspMsg.m_byteMem) IoScanResponse::IoScanRspMsg( *this, m_driverbox, *this, *payload );
            m_scanInProgress                      = true;
            m_chassisInputSeqNum                  = m_driverInputsSeqNum;
            m_chassisbox->post( msg->getRequestMsg() );

            return true;
        }

        return false;
    }

    /** This populates the input transfer buffer from the driver's data store
        The concrete card is required to implement this method
     */
    virtual void populateInputsTransferBuffer() noexcept = 0;

public:
    // ITC Request to update the chassis/card with driver data. NOTE: executes in the chassis thread
    void request( IoScanReqMsg& msg )
    {
        // Update the card data
        extractInputsTransferBuffer();
        //memcpy( m_chassisCachedData, m_transferData, m_dataLen );

        // Complete the ITC transaction
        msg.returnToSender();
    }

    /** This populates the chassis/card's INPUT IO Registers from the input 
        transfer buffer.
        The concrete card is required to implement this method
     */
    virtual void extractInputsTransferBuffer() noexcept = 0;

    // ITC Response: executes in the driver thread
    void response( IoScanRspMsg& msg )
    {
        m_scanInProgress = false;

        // Start of new transfer if there is newer data (but don't increment the sequence number)
        issueAsyncIoScanRequest( false );
    }

protected:
    /// The driver thread's mailbox reference
    Cpl::Dm::MailboxServer&  m_driverbox;

    /// The Chassis/card thread's mailbox reference
    Cpl::Itc::PostApi*       m_chassisbox;

    /// Memory for IoScan Message
    Cpl::Memory::AlignedClass<IoScanResponse::IoScanRspMsg>   m_memoryIoScanRspMsg;

    /// Memory for the IoScan Message Payload
    Cpl::Memory::AlignedClass<IoScanRequest::IoScanPayload>   m_memoryIoScanPayload;


    /// Chassis sequence number for Inputs transfers
    uint16_t    m_chassisInputSeqNum;

    /// Sequence number at the time the ITC request was made
    uint16_t    m_driverInputsSeqNum;

    /// Scan in progress
    bool        m_scanInProgress;
};


};      // end namespaces
};
#endif  // end header latch
