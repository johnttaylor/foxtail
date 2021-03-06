#ifndef Storm_Type_CycleInfo_h_
#define Storm_Type_CycleInfo_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/System/ElapsedTime.h"
#include "Storm/Type/CycleStatus.h"

///
namespace Storm {
///
namespace Type {

/** The following typedef is used to store the information about the current
    on/off cycling of the equipment.
 */
struct CycleInfo_T
{
    Cpl::System::ElapsedTime::Precision_T   beginOnTime;                //!< The starting time of current/last on cycle
    Cpl::System::ElapsedTime::Precision_T   beginOffTime;               //!< The starting time of current/last off cycle
    uint32_t                                onTime;                     //!< The calculated time (in seconds) of the current/last on cycle
    uint32_t                                offTime;                    //!< The calculated time (in seconds) of the current/last off cycle
    int                                     mode;                       //!< Current state (of type Storm::Type::CycleStatus) of the current on/off cycle

    /// Constructor (to ensure any pad bytes get zero'd)
    CycleInfo_T()
    {
        memset( (void*) this, 0, sizeof( CycleInfo_T ) );
    }

    /// Copy Constructor (to ensure any pad bytes get zero'd)
    CycleInfo_T( const CycleInfo_T& other )
    {
        memcpy( (void*) this, (void*) &other, sizeof( CycleInfo_T ) );
    }
    
    /// Copy operator
    CycleInfo_T& operator =( const CycleInfo_T& other )
    {
        memcpy( (void*) this, (void*) &other, sizeof( CycleInfo_T ) );
        return *this;
    }

};


};      // end namespaces
};
#endif  // end header latch
