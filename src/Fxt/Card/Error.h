#ifndef Fxt_Card_Error_h_
#define Fxt_Card_Error_h_
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


#include "Fxt/Type/Error.h"




///
namespace Fxt {
///
namespace Card {

/** This enumeration defines error codes that are unique to the Card Sub-system

        Note: Each error code symbol must be less than 32 characters
           123456789 123456789 12345678 12

    @param Err_T                            Enum

    @param SUCCESS                          NO ERROR
    @param MEMORY_CARD                      Unable to allocate memory for the card
    @param FAILED_POINT_CREATED             Failed to create the a Point for a channel
    @param UNKNOWN_GUID                     Configuration contains the unknown/unsupported card type GUID 
    @param POINT_MISSING_ID                 Configuration does NOT contain a valid ID value for one of it Points
    @param TOO_MANY_INPUT_POINTS            Configuration contains TOO many input Points
    @param TOO_MANY_OUTPUT_POINTS           Configuration contains TOO many output Points
    @param BAD_CHANNEL_ASSIGNMENTS          Configuration contains duplicate or our-of-range Channel IDs
    @param BAD_JSON                         Invalid JSON object syntax for a Channel Object
    @param MISSING_SLOT                     Configuration does NOT contain a slot number
    @param MOCK                             A Mock Card sub-system specific error
 */
BETTER_ENUM( Err_T, uint8_t
             , SUCCESS = 0
             , MEMORY_CARD
             , FAILED_POINT_CREATED
             , UNKNOWN_GUID
             , POINT_MISSING_ID          
             , TOO_MANY_INPUT_POINTS     
             , TOO_MANY_OUTPUT_POINTS    
             , BAD_CHANNEL_ASSIGNMENTS   
             , BAD_JSON
             , MISSING_SLOT
             , MOCK
);

/** This method has 'PACKAGE Scope' in that is should only be called by
    other classes in the Fxt namespace.  It is ONLY public to avoid
    the tight coupling of C++ friend mechanism.
        
    This function is used to help with convert a full qualified error code
    into text
 */
void errorCodetoText_( Cpl::Text::String& buffer, Fxt::Type::Error errorCode, uint8_t levelIndex ) noexcept;

/// Helper method to build a 'complete/full' error from a card specific error code
inline Fxt::Type::Error fullErr( Err_T localErr )
{
    return Fxt::Type::Error( Fxt::Type::Err_T::CARD, localErr );
}



};      // end namespaces
};
#endif  // end header latch
