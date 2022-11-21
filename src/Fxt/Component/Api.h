#ifndef Fxt_Component_Api_h_
#define Fxt_Component_Api_h_
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


#include "Cpl/Container/Item.h"
#include "Cpl/Type/Guid.h"
#include "Fxt/Type/Error.h"
#include "Fxt/Point/DatabaseApi.h"
#include <stdint.h>


///
namespace Fxt {
///
namespace Component {


/** This abstract class defines the non-type specific operations that can be
    performed on an Component.

    Note: Component semantics are NOT thread-safe.

    Note: Component support being in a Container - however 'that' container
          is it containing Logic Chain.  This means if the application wants to 
          maintain a list of Components - it must provide its own wrapper.

 */
class Api : public Cpl::Container::Item
{
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
    /** This method is used to start/initialize the Component for execution.
        If the Component fails to be started the method returns an error code; 
        else FXT_COMPONENT_ERR_NO_ERROR is returned.

        The 'currentElapsedTimeUsec' argument represents the current elapsed 
        time in microseconds since power-up of the application.

        A Component can be started/stopped multiple times, however it should only
        started/stopped when its containing Logic Chain is started/stopped. When 
        a Component is created it is in the 'stopped' state.
        
        Restarting (after being stopped) a component will clear any existing 
        error conditions.
     */
    virtual Fxt::Type::Error start( uint64_t currentElapsedTimeUsec ) noexcept = 0;

    /** This method is used to stop/deactivate the IO card.  If the Component fails
        to be stopped the method returns an error code; else 
        FXT_COMPONENT_ERR_NO_ERROR is returned.

        A Component MUST be in the 'stopped state' before it can be deleted/destroyed
     */
    virtual void stop() noexcept = 0;

    /** This method returns true if the Component is in the started state
     */
    virtual bool isStarted() const noexcept = 0;


public:
    /** This method is called to have a component perform its actions. It should
        be called periodically, i.e. every time its containing Logic Chain 
        executes.

        The 'currentTickUsec' argument represents the current elapsed time
        in microseconds since power-up of the application.  For given execution
        cycle, A Logic Chain should pass the same 'currentTickUsec' value to 
        ALL of its contained components for that cycle.

        This method return FXT_COMPONENT_ERR_NO_ERROR if the Component completed 
        ALL of its logic for the current processing cycle; else if error occurred
        then an error code is returned. Once the component has encountered
        an error, subsequence calls to this method will always fail. 
     */
    virtual Fxt::Type::Error execute( int64_t currentTickUsec ) noexcept = 0;

public:
    /** This method returns the Component's GUID (that identifies its type) as a
        text string in 8-4-4-4-12 format
     */
    virtual const char* getTypeGuid() const noexcept = 0;

    /// Returns the Components's 'human readable' type name (note: this is NOT guaranteed to be unique)
    virtual const char* getTypeName() const noexcept = 0;

public:
    /** This method returns the current error state of the Component.  A value
        of zero/ERR_NO_ERROR indicates the card is operating properly
     */
    virtual Fxt::Type::Error getErrorCode() const noexcept = 0;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~Api() {}
};


};      // end namespaces
};
#endif  // end header latch
