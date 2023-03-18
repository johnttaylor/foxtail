#ifndef Fxt_Node_ExternalRequests_h_
#define Fxt_Node_ExternalRequests_h_
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
          Asynchronously.  This is because the Node can/does use synchronous
          ITC when starting/stopping Cards.  By using asynchronously semantics
          the application will avoid potential deadlock scenarios.
*/

#include "Cpl/Itc/RequestMessage.h"
#include "Cpl/Itc/ResponseMessage.h"
#include "Cpl/Itc/SAP.h"
#include "Cpl/Json/Arduino.h"
#include "Fxt/Point/FactoryDatabaseApi.h"
#include "Fxt/Type/Error.h"
#include "Fxt/Node/Api.h"
#include <stdint.h>

///
namespace Fxt {
///
namespace Node {

/** This abstract class define message types and payloads for a set of
    ITC services. The request() method(s) are to be implemented by the
    'server'
 */
class CreateRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<CreateRequest> SAP;

public:
    /// Payload for Message: Create
    class CreatePayload
    {
    public:
        /// IN: JSON object to create/config the node
        JsonVariant&             m_nodeJsonObject;

        /// IN: Point database to use
        Fxt::Point::DatabaseApi& m_dbForPoints;

        /// OUT: Result/ErrorCode
        Fxt::Type::Error         m_nodeErrorode;

        /// OUT: Pointer to the created Node (will be null if failed)
        Node::Api*               m_nodePtr;

    public:
        /// Constructor
        CreatePayload( JsonVariant& nodeJsonObject, Fxt::Point::DatabaseApi& dbForPoints )
            : m_nodeJsonObject( nodeJsonObject )
            , m_dbForPoints( dbForPoints )
            , m_nodePtr( nullptr )
        {
        }
    };


    /// Message Type: Create
    typedef Cpl::Itc::RequestMessage<CreateRequest, CreatePayload> CreateMsg;


public:
    /// Request: Create
    virtual void request( CreateMsg& msg ) = 0;

public:
    ///
    virtual ~CreateRequest() {}
};

/** This abstract class define response message types for a set of ITC services.
    The response() method(s) are to be implemented by the 'client'

    NOTE: See note above - about always using asynchronous semantics
 */
class CreateResponse
{
public:
    /// Response Message Type: Create
    typedef Cpl::Itc::ResponseMessage<CreateResponse,
        CreateRequest,
        CreateRequest::CreatePayload> CreateMsg;


public:
    /// Response: Create
    virtual void response( CreateMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~CreateResponse() {}
};


///////////////////////////////////////////////////////////////////////////////
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
        /// IN: Starting time in microseconds
        uint64_t m_currentElapsedTimeUsec;

        /// OUT: Result
        bool     m_success;

    public:
        /// Constructor
        StartPayload( uint64_t currentElapsedTimeUsec )
            : m_currentElapsedTimeUsec( currentElapsedTimeUsec )
            , m_success( false )
        {
        }
    };


    /// Message Type: Start
    typedef Cpl::Itc::RequestMessage<StartRequest, StartPayload> StartMsg;


public:
    /// Request: Start
    virtual void request( StartMsg& msg ) = 0;

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
        StartRequest::StartPayload> StartMsg;


public:
    /// Response: Start
    virtual void response( StartMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~StartResponse() {}
};

///////////////////////////////////////////////////////////////////////////////
/** This abstract class define message types and payloads for a set of
    ITC services. The request() method(s) are to be implemented by the
    'server'
 */
class IsStartedRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<IsStartedRequest> SAP;

public:
    /// Payload for Message: IsStarted
    class IsStartedPayload
    {
    public:
        /// OUT: Start status
        bool m_started;

    public:
        /// Constructor
        IsStartedPayload()
            : m_started( false )
        {
        }
    };


    /// Message Type: IsStarted
    typedef Cpl::Itc::RequestMessage<IsStartedRequest, IsStartedPayload> IsStartedMsg;


public:
    /// Request: IsStarted
    virtual void request( IsStartedMsg& msg ) = 0;

public:
    ///
    virtual ~IsStartedRequest() {}
};

/** This abstract class define response message types for a set of ITC services.
    The response() method(s) are to be implemented by the 'client'

    NOTE: See note above - about always using asynchronous semantics
 */
class IsStartedResponse
{
public:
    /// Response Message Type: IsStarted
    typedef Cpl::Itc::ResponseMessage<IsStartedResponse,
        IsStartedRequest,
        IsStartedRequest::IsStartedPayload> IsStartedMsg;


public:
    /// Response: IsStarted
    virtual void response( IsStartedMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~IsStartedResponse() {}
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
        /// Constructor
        StopPayload()
        {
        }
    };


    /// Message Type: Stop
    typedef Cpl::Itc::RequestMessage<StopRequest, StopPayload> StopMsg;


public:
    /// Request: Stop
    virtual void request( StopMsg& msg ) = 0;

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
        StopRequest::StopPayload> StopMsg;


public:
    /// Response: Stop
    virtual void response( StopMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~StopResponse() {}
};


///////////////////////////////////////////////////////////////////////////////
/** This abstract class define message types and payloads for a set of
    ITC services. The request() method(s) are to be implemented by the
    'server'
 */
class DestroyRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<DestroyRequest> SAP;

public:
    /// Payload for Message: Destroy
    class DestroyPayload
    {
    public:
        /// IN: Node to delete/destroy
        Node::Api&  m_node;

    public:
        /// Constructor
        DestroyPayload( Node::Api& nodeToDestroy )
            : m_node( nodeToDestroy )
        {
        }
    };


    /// Message Type: Destroy
    typedef Cpl::Itc::RequestMessage<DestroyRequest, DestroyPayload> DestroyMsg;


public:
    /// Request: Destroy
    virtual void request( DestroyMsg& msg ) = 0;

public:
    ///
    virtual ~DestroyRequest() {}
};

/** This abstract class define response message types for a set of ITC services.
    The response() method(s) are to be implemented by the 'client'

    NOTE: See note above - about always using asynchronous semantics
 */
class DestroyResponse
{
public:
    /// Response Message Type: Destroy
    typedef Cpl::Itc::ResponseMessage<DestroyResponse,
        DestroyRequest,
        DestroyRequest::DestroyPayload> DestroyMsg;


public:
    /// Response: Destroy
    virtual void response( DestroyMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~DestroyResponse() {}
};


};      // end namespaces
};
#endif  // end header latch
