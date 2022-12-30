#ifndef Fxt_LogicChain_Chain_h_
#define Fxt_LogicChain_Chain_h_
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


#include "Fxt/LogicChain/Api.h"
#include "Fxt/LogicChain/Error.h"
#include "Fxt/Point/DatabaseApi.h"
#include "Cpl/Json/Arduino.h"
#include "Cpl/Memory/ContiguousAllocator.h"

///
namespace Fxt {
///
namespace LogicChain {


/** This concrete class implements the LogicChain interface
 */
class Chain : public Api, public Cpl::Itc::MailboxServer
{
public:
    /// Constructor
    Chain( Cpl::Memory::ContiguousAllocator&   generalAllocator,
           uint16_t                            numComponents,
           uint16_t                            numAutoPoints );

    /// Destructor
    ~Chain();

public:
    /** This method is used to resolve Point references once all of the
        configuration (i.e. all Points have been) has been processed. The
        method returns true if all references have been successfully resolved;
        else false is returned.

        This method MUST be called prior the start(...) method being called, i.e.
        a Chassis/LogicChain/Component is not ready to begin execution till all
        symbolic references have been resolved.
     */
    virtual Fxt::Type::Error resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept = 0;

public:
    /** This method is used to start/initialize the Chassis for execution.
        If the Chassis fails to be started the method returns an error code;
        else Fxt::Type::Err_T::SUCCESS is returned.

        The 'currentElapsedTimeUsec' argument represents the current elapsed
        time in microseconds since power-up of the application.

        A Chassis can be started/stopped multiple times, however it should only
        started/stopped when its containing Node is started/stopped. When
        a Chassis is created it is in the 'stopped' state.
     */
    virtual Fxt::Type::Error start( uint64_t currentElapsedTimeUsec ) noexcept = 0;

    /** This method is used to stop/deactivate the Chassis.  If the Chassis fails
        to be stopped the method returns an error code; else Fxt::Type::Err_T::SUCCESS
        is returned.

        A Chassis MUST be in the 'stopped state' before it can be deleted/destroyed
     */
    virtual void stop() noexcept = 0;

    /** This method returns true if the Logic Chain is in the started state
     */
    virtual bool isStarted() const noexcept = 0;

public:
    /** This method is called to have a Chassis execute is contained Components.
        It should be called periodically by the 'Chassis' object

        The 'currentTickUsec' argument represents the current elapsed time
        in microseconds since power-up of the application.  For given execution
        cycle,

        This method return Fxt::Type::Err_T::SUCCESS if the Logic Chain completed
        ALL of its logic for the current processing cycle; else if error occurred
        then an error code is returned. Once the Logic Chain has encountered
        an error, subsequence calls to this method will always fail.
     */
    virtual Fxt::Type::Error execute( int64_t currentTickUsec ) noexcept = 0;


public:
    /// See Fxt::LogicChain::Api
    Fxt::Type::Error resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept;

    /// See Fxt::LogicChain::Api
    Fxt::Type::Error start( uint64_t currentElapsedTimeUsec ) noexcept;

    /// See Fxt::LogicChain::Api
    void stop() noexcept;

    /// See Fxt::LogicChain::Api
    bool isStarted() const noexcept;

    /// See Fxt::LogicChain::Api
    Fxt::Type::Error execute( int64_t currentTickUsec ) noexcept;

    /// See Fxt::LogicChain::Api
    Fxt::Type::Error getErrorCode() const noexcept;

    /// See Fxt::LogicChain::Api
    Fxt::Type::Error add( Fxt::Component::Api& componentToAdd ) noexcept;

    /// See Fxt::LogicChain::Api
    Fxt::Type::Error add( Fxt::Point::Api& autoPointToAdd ) noexcept;

protected:
    /// Array/List of components in the logic chain
    Fxt::Component::Api**               m_components;

    /// Array/List of Auto points in the logic chain
    Fxt::Point::Api**                   m_autoPoints;

    /// Error state. A value of 0 indicates NO error
    Fxt::Type::Error                    m_error;

    /// Number of components
    uint16_t                            m_numComponents;

    /// Number of Auto Point
    uint16_t                            m_numAutoPoints;

    /// Array index for the next Component add operation
    uint16_t                            m_nextComponentIdx;

    /// Array index for the next Auto Points add operation
    uint16_t                            m_nextAutoPtsIdx;

    /// My started state
    bool                                m_started;
};



};      // end namespaces
};
#endif  // end header latch
