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
    @param DEMUX_INVALID_BIT_OFFSET         An invalid/out-of-range bit offset was specified for Demux component
    @param MUX_INVALID_BIT_OFFSET           An invalid/out-of-range bit offset was specified for Mux component
 */
BETTER_ENUM( Err_T, uint8_t
             , SUCCESS = 0
             , DEMUX_INVALID_BIT_OFFSET
             , MUX_INVALID_BIT_OFFSET
);

/** This concrete class defines the Error Category for the Digital namespace.
    This class is designed as SINGLETON.
 */
class ErrCategory : public Fxt::Type::ErrorCategory<Err_T>
{
public:
    /// Constructor
    ErrCategory( Fxt::Type::ErrorBase& parent = Fxt::Component::ErrCategory::g_theOne )
        :Fxt::Type::ErrorCategory<Err_T>( "DIGITAL", 2, parent )
    {
    }

public:
    /// Singleton
    static ErrCategory g_theOne;
};

/// Helper method to build a 'complete/full' error from a specific error code
inline Fxt::Type::Error fullErr( Err_T localErr )
{
    return Fxt::Type::Error( ErrCategory::g_theOne, localErr );
}


};      // end namespaces
};
};
#endif  // end header latch
