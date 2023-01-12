#ifndef Cpl_Logging_Api_h_
#define Cpl_Logging_Api_h_
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

#include "Cpl/Logging/CategoryId.h"
#include "Cpl/Logging/TimeApi.h"
#include "Cpl/Logging/Private_.h"
#include <stdint.h>

 
 ///
namespace Cpl {
///
namespace Logging {


/*---------------------------------------------------------------------------*/
/** This method is used to enable one or more log Categories.  Returns the
    previous enabled/disabled mask.  

    The method is thread safe
 */
uint32_t enableCategories( uint32_t categoryMask ) noexcept;

/** This method is used to disable one or more log Categories.  Returns the
    previous enabled/disabled mask.

    The method is thread safe
 */
uint32_t disableCategories( uint32_t categoryMask ) noexcept;

/** This method returns the current enabled/disabled Category mask

    The method is thread safe
 */
uint32_t getCategoryEnabledMask() noexcept;


/*---------------------------------------------------------------------------*/
/** This method is used to create log entry. The method has printf() semantics.

    The method is thread safe.

    NOTE: It is recommend that the application define convenience methods for
          each of the MESSAGE_ID types.  For example:
          \code

          Given:
            BETTER_ENUM( EventMsg, uint32_t, .... );
                             
            void logf( EventMsg msgId, const char* msgTextFormat, ... ) noexcept
            {
                va_list ap;
                va_start( ap, msgTextFormat );
                logf<EventMsg>( Cpl::Logging::CategoryId::EVENT, msgId, msgTextFormat, ap );
                va_end( ap );
            }
          
          \endcode
 */
template <class MESSAGE_ID>
void logf( CategoryId catId, MESSAGE_ID msgId, const char* msgTextFormat, ... ) noexcept
{
    // Check to see if the entry queue is full
    if ( !isQueFull_( catId ) )
    {
        // Ignore entry when not enabled
        if ( isEnabled_( catId ) )
        {
            // Generate and 'log' the entry
            va_list ap;
            va_start( ap, msgTextFormat );
            bool addresult = createAndAddLogEntry_( catId, msgId, msgId._to_string(), msgTextFormat, ap );
            va_end( ap );

            // Business logic for when the Queue is full
            if ( addresult == false )
            {
                manageFullQue_();
            }
        }
    }
}

/// Similar to logf() above - but accepts a va_list argument
template <class MESSAGE_ID>
void vlogf( CategoryId catId, MESSAGE_ID msgId, const char* msgTextFormat, va_list ap ) noexcept
{
    // Check to see if the entry queue is full
    if ( !isQueFull_( catId ) )
    {
        // Ignore entry when not enabled
        if ( isEnabled_( catId ) )
        {
            // Generate and 'log' the entry
            bool addresult = createAndAddLogEntry_( catId, msgId, msgId._to_string(), msgTextFormat, ap );

            // Business logic for when the Queue is full
            if ( addresult == false )
            {
                manageFullQue_();
            }
        }
    }
}


};      // end namespaces
};
#endif  // end header latch
