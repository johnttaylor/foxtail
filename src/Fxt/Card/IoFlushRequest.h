#ifndef Fxt_Card_IoFlushRequests_h_
#define Fxt_Card_IoFlushRequests_h_
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

    NOTE: The ITC Requests defined in this file SHOULD ALWAYS be use
          Asynchronously.  This is because the assignment of what runs is
          what thread (or how many threads exists) is outside of the control
          of Card - so it must asynchronously semantics to avoid potential 
          deadlock scenarios.
*/

#include "Cpl/Itc/RequestMessage.h"
#include "Cpl/Itc/ResponseMessage.h"
#include "Cpl/Itc/SAP.h"
#include <stdint.h>

///
namespace Fxt {
///
namespace Card {

/** This abstract class define message types and payloads for a set of
    ITC services. The request() method(s) are to be implemented by the
    'server'
 */
class IoFlushRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<IoFlushRequest> SAP;

public:
    /// Payload for Message: IoFlush
    class IoFlushPayload
    {
    public:
        /// Constructor
        IoFlushPayload()
        {
        }
    };


    /// Message Type: IoFlush
    typedef Cpl::Itc::RequestMessage<IoFlushRequest, IoFlushPayload> IoFlushReqMsg;


public:
    /// Request: IoFlush
    virtual void request( IoFlushReqMsg& msg ) = 0;

public:
    ///
    virtual ~IoFlushRequest() {}
};


/** This abstract class define response message types for a set of ITC services.
    The response() method(s) are to be implemented by the 'client'

    NOTE: See note above - about always using asynchronous semantics
 */
class IoFlushResponse
{
public:
    /// Response Message Type: IoFlush
    typedef Cpl::Itc::ResponseMessage<IoFlushResponse,
        IoFlushRequest,
        IoFlushRequest::IoFlushPayload> IoFlushRspMsg;


public:
    /// Response: IoFlush
    virtual void response( IoFlushRspMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~IoFlushResponse() {}
};

};      // end namespaces
};
#endif  // end header latch
