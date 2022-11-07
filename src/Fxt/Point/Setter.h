#ifndef Fxt_Point_Setter_h_
#define Fxt_Point_Setter_h_
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

#include "Fxt/Point/Api.h"
#include "Fxt/Point/DatabaseApi.h"
#include "Cpl/Memory/ContiguousAllocator.h"


///
namespace Fxt {
///
namespace Point {

/** This concrete class defines a NON-TYPE safe interface that is used
    to set the value (and valid/invalid state) of Point by using a reference
    to a Generic point (instead of concrete, fully type safe reference).

    NOTE: Because the interface is not type-safe - the Application is
          RESPONSIBLE for ensuring that the CreateFunc_T function used to
          create the Setter instance is the 'same type' as the Point being
          updated/set
 */
class Setter
{
public:
    /// Define the function signature for creating a concrete point
    typedef Fxt::Point::Api* (*CreateFunc_T)(DatabaseApi& db, Cpl::Memory::Allocator& allocatorForPoints, uint32_t pointId, const char* pointName, Cpl::Memory::ContiguousAllocator& allocatorForPointStatefulData);

public:
    /// Constructor
    Setter( Fxt::Point::Api* srcPoint=nullptr );

public:
    /** This method update the data/state of 'dstPoint'.  The value/state
        being written is dependent on the concrete instance.
     */
    void setValue( Fxt::Point::Api& dstPoint ) noexcept;


public:
    /** This method is used to create the internal point that is used to
        store the source value that will be used in the setValue()
        operation.  The 'valueSrc' argument is reference to JSON object
        that is a Point's value, e.g "val:32", "val:"eENUM_RED", etc.
     */
    static Setter* create( DatabaseApi&                         db,
                           CreateFunc_T                         pointFactoryMethod,
                           uint32_t                             pointId,
                           const char*                          pointName,
                           Cpl::Memory::Allocator&              allocatorForPoints,
                           Cpl::Memory::ContiguousAllocator&    allocatorForPointStatefulData,
                           JsonVariant&                         valueSrc ) noexcept;

protected:
    /// Internal point
    Fxt::Point::Api* m_srcPoint;
};


};      // end namespaces
};
#endif  // end header latch
