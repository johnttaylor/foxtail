#ifndef Fxt_Point_FactoryDatabaseApi_h_
#define Fxt_Point_FactoryDatabaseApi_h_
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
#include "Fxt/Point/FactoryApi.h"
#include "Cpl/Container/SList.h"


///
namespace Fxt {
///
namespace Point {


/** This abstract class defines the interface for the Point Factory Database.
    The database contains the list of Factory instances for all support Point
    types.

    Note: There is no guaranteed order Point Factory list.

    The semantics of the Database interface is NOT thread safe.
 */
class FactoryDatabaseApi: public Cpl::Container::SList<FactoryApi>
{
public:
    /** This method attempts to parse the provided JSON Object that represents
        a Point and create binary representation of the defined Point.  If there
        is an error (e.g. Point type not supported, missing key/value pairs,
        etc.) the method returns nullptr; else a pointer to the newly created
        Point object is returned.  When an error occurs, details about the
        specific point error is returned via 'pointErrorode' argument.

        See the Fxt::Point::FactoryApi interface for the required JSON Object
        fields.

        This method should ONLY be called after all Factory instances have been
        registered with the Factory Database.
     */
    virtual Api* createPointfromJSON( JsonObject&                        pointObject,
                                      Fxt::Type::Error&                  pointErrorCode,
                                      Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                      Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                                      Fxt::Point::DatabaseApi&           dbForPoints,
                                      const char*                        pointIdKeyName = "id",
                                      bool                               createSetter   = true ) noexcept = 0;

public:
    /** This method looks-up the Point Factory by its GUID.  If the
        Point Factory GUID cannot be found (i.e. card type not supported by
        the platform), THEN the method returns 0.

        The 'guidCardTypeId' is null terminated string contain a 8-4-4-4-12
        formated GUID.
     */
    virtual FactoryApi* lookup( const char* guidPointTypeId ) noexcept = 0;

protected:
    /// Constructor
    FactoryDatabaseApi() : Cpl::Container::SList<FactoryApi>() {}

    /** This is a special constructor for when the Database is
        statically declared (i.e. it is initialized as part of C++ startup
        BEFORE main() is executed.  C/C++ does NOT guarantee the order of static
        initializers.  Since the Database contains a list of IO Card factory
        that are typically statically created/initialized - we need to do
        something to ensure my internal list is properly initialized for this
        scenario - hence this constructor.
     */
    FactoryDatabaseApi( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) : Cpl::Container::SList<FactoryApi>( ignoreThisParameter_usedToCreateAUniqueConstructor ) {}

public:
    /// Virtual destructor to make the compiler happy
    virtual ~FactoryDatabaseApi() {}
};


};      // end namespaces
};
#endif  // end header latch
