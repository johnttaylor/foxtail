#ifndef Fxt_Point_FactoryCommon_h_
#define Fxt_Point_FactoryCommon_h_
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


#include "Fxt/Point/FactoryApi.h"
#include "Fxt/Point/FactoryDatabaseApi.h"
#include "Fxt/Point/Error.h"
#include "Fxt/Point/DatabaseApi.h"
#include "Cpl/Memory/ContiguousAllocator.h"

///
namespace Fxt {
///
namespace Point {


/** This partially concrete class provide common infrastructure for a Point
    Factory.
 */
class FactoryCommon_ : public Fxt::Point::FactoryApi
{
public:
    /// Constructor
    FactoryCommon_( FactoryDatabaseApi&  factoryDatabase );

public:
    /// See Fxt::Point::FactoryApi
    Api* create( JsonObject&                        pointObject,
                 Fxt::Type::Error&                  pointErrorCode,
                 Cpl::Memory::ContiguousAllocator&  generalAllocator,
                 Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                 Fxt::Point::DatabaseApi&           dbForPoints,
                 const char*                        pointIdKeyName   = "id" ) noexcept;
};

};      // end namespaces
};
#endif  // end header latch
