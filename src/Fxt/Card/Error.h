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

    @param GlobalErrCodes                   Enum

    @param SUCCESS                          NO ERROR
    @param MEMORY_CARD                      Unable to allocate memory for the card
    @param MEMORY_DESCRIPTORS               Out-of-memory when parsing/creating Descriptors
    @param UNKNOWN_GUID                     Configuration contains the unknown/unsupported card type GUID 
    @param CARD_MISSING_ID                  Configuration does NOT contain a valid ID value
    @param POINT_MISSING_ID                 Configuration does NOT contain a valid ID value for one of it Points
    @param TOO_MANY_INPUT_POINTS            Configuration contains TOO many input Points
    @param TOO_MANY_OUTPUT_POINTS           Configuration contains TOO many output Points
    @param BAD_CHANNEL_ASSIGNMENTS          Configuration contains duplicate or our-of-range Channel IDs
    @param CARD_SETTER_ERROR                Error occurred while parsing/creating a Setter instance
    @param MEMORY_DESCRIPTOR_NAME           Out-of-memory when parsing/creating a Descriptor's name
    @param CARD_INVALID_ID                  The Card ID is invalid/out-of-range
    @param MOCK                             A Mock Card sub-system specific error
 */
BETTER_ENUM( Err_T, uint8_t
             , SUCCESS = 0
             , MEMORY_CARD
             , MEMORY_DESCRIPTORS
             , UNKNOWN_GUID
             , CARD_MISSING_ID           
             , POINT_MISSING_ID          
             , TOO_MANY_INPUT_POINTS     
             , TOO_MANY_OUTPUT_POINTS    
             , BAD_CHANNEL_ASSIGNMENTS   
             , CARD_SETTER_ERROR         
             , MEMORY_DESCRIPTOR_NAME    
             , CARD_INVALID_ID           
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
