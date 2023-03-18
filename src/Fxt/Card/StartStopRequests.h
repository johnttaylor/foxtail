#ifndef Fxt_Card_StartStopRequests_h_
#define Fxt_Card_StartStopRequests_h_
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
class StartRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<StartRequest> SAP;

public:
    /// Payload for Message: Start
    class StartPayload
    {
    public:
        /// Pass/Fail result of the class request
        bool        m_success;

        /// Optional arguments
        void*       m_args;

        /// Time of when start is called
        uint64_t    m_currentElapsedTimeUsec;

    public:
        /// Constructor
        StartPayload( uint64_t currentElapsedTimeUsec, void* args = nullptr ) 
            : m_success( true ) 
            , m_args( args ) 
            , m_currentElapsedTimeUsec( currentElapsedTimeUsec)
        {
        }
    };


    /// Message Type: Start
    typedef Cpl::Itc::RequestMessage<StartRequest, StartPayload> StartReqMsg;


public:
    /// Request: Start
    virtual void request( StartReqMsg& msg ) = 0;

public:
    ///
    virtual ~StartRequest() {}
};


/** This abstract class define response message types for a set of ITC services.
    The response() method(s) are to be implemented by the 'client'

    NOTE: See note above - about always using asynchronous semantics
 */
class StartResponse
{
public:
    /// Response Message Type: Start
    typedef Cpl::Itc::ResponseMessage<StartResponse,
        StartRequest,
        StartRequest::StartPayload> StartRspMsg;


public:
    /// Response: Start
    virtual void response( StartRspMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~StartResponse() {}
};


///////////////////////////////////////////////////////////////////////////////
/** This abstract class define message types and payloads for a set of
    ITC services. The request() method(s) are to be implemented by the
    'server'
 */
class StopRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<StopRequest> SAP;

public:
    /// Payload for Message: Stop
    class StopPayload
    {
    public:
        /// Optional arguments
        void* m_args;

    public:
        /// Constructor
        StopPayload( void* args = nullptr ) : m_args( args ) {}
    };


    /// Message Type: Stop
    typedef Cpl::Itc::RequestMessage<StopRequest, StopPayload> StopReqMsg;


public:
    /// Request: Stop
    virtual void request( StopReqMsg& msg ) = 0;

public:
    ///
    virtual ~StopRequest() {}
};


/** This abstract class define response message types for a set of ITC services.
    The response() method(s) are to be implemented by the 'client'

    NOTE: See note above - about always using asynchronous semantics
 */
class StopResponse
{
public:
    /// Response Message Type: Stop
    typedef Cpl::Itc::ResponseMessage<StopResponse,
        StopRequest,
        StopRequest::StopPayload> StopRspMsg;


public:
    /// Response: Stop
    virtual void response( StopRspMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~StopResponse() {}
};



};      // end namespaces
};
#endif  // end header latch
