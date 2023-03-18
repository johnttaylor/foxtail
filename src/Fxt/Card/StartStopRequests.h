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
/** @file */

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
    typedef Cpl::Itc::RequestMessage<StartRequest, StartPayload> StartMsg;


public:
    /// Request: Start
    virtual void request( StartMsg& msg ) = 0;

public:
    ///
    virtual ~StartRequest() {}
};

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
    typedef Cpl::Itc::RequestMessage<StopRequest, StopPayload> StopMsg;


public:
    /// Request: Stop
    virtual void request( StopMsg& msg ) = 0;

public:
    ///
    virtual ~StopRequest() {}
};




};      // end namespaces
};
#endif  // end header latch
