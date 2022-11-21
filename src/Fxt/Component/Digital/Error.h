#ifndef Fxt_Component_Digital_Error_h_
#define Fxt_Component_Digital_Error_h_
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
#include "Fxt/Component/Error.h"
#include "Error.h"




///
namespace Fxt {
///
namespace Component {
///
namespace Digital {

/** This enumeration defines error codes that are unique to the Digital
    Components Sub-system

        Note: Each error code symbol must be less than 32 characters
           123456789 123456789 12345678 12

    @param GlobalErrCodes                   Enum

    @param SUCCESS                          NO ERROR
    @param SPLITTER_INVALID_BIT_OFFSET      An invalid/out-of-range bit offset was specified for xxxSplitter component
 */
BETTER_ENUM( Err_T, uint8_t
             , SUCCESS = 0
             , SPLITTER_INVALID_BIT_OFFSET = 1
);

/** This method has 'PACKAGE Scope' in that is should only be called by
    other classes in the Fxt namespace.  It is ONLY public to avoid
    the tight coupling of C++ friend mechanism.
        
    This function is used to help with convert a full qualified error code
    into text
 */
void errorCodetoText_( Cpl::Text::String& buffer, Fxt::Type::Error errorCode, uint8_t levelIndex ) noexcept;

/// Helper method to build a 'complete/full' error from a Digital component specific error code
inline Fxt::Type::Error fullErr( Err_T localErr )
{
    return Fxt::Type::Error( Fxt::Type::Err_T::COMPONENT, Fxt::Component::Err_T::DIGITAL, localErr );
}

};      // end namespaces
};
};
#endif  // end header latch
