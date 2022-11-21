#ifndef Fxt_Card_Mock_Error_h_
#define Fxt_Card_Mock_Error_h_
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
///
namespace Mock {

/// No Sub-system defined errors at this time
#if 0 
/** This enumeration defines error codes that are unique to the Mock'd Card 
    Sub-system

        Note: Each error code symbol must be less than 32 characters
           123456789 123456789 12345678 12

    @param GlobalErrCodes                   Enum

    @param MEMORY_CARD                      Unable to allocate memory for the card
 */
BETTER_ENUM( Err_T, uint8_t
             , MEMORY_CARD = 1
);
#endif

/** This method has 'PACKAGE Scope' in that is should only be called by
    other classes in the Fxt namespace.  It is ONLY public to avoid
    the tight coupling of C++ friend mechanism.
        
    This function is used to help with convert a full qualified error code
    into text.
 */
void errorCodetoText_( Cpl::Text::String& buffer, Fxt::Type::Error errorCode, uint8_t levelIndex ) noexcept;

#if 0
/// Helper method to build a 'complete/full' error from a card specific error code
inline Fxt::Type::Error fullErr( Err_T localErr )
{
    return Fxt::Type::Error( Fxt::Type::Err_T::CARD, Fxt::Type::Err_T::MOCK, localErr );
}
#endif

};      // end namespaces
};
};
#endif  // end header latch
