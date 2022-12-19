#ifndef Fxt_Point_FactoryDatabase_h_
#define Fxt_Point_FactoryDatabase_h_
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


#include "Fxt/Point/FactoryDatabaseApi.h"
#include "Cpl/Container/SList.h"

///
namespace Fxt {
///
namespace Point {


/** This concrete class implements the FactoryDatabaseApi

    NOTE: This class is NOT thread safe
 */
class FactoryDatabase : public FactoryDatabaseApi
{
public:
    /// Constructor
    FactoryDatabase() noexcept;

    /** This is a special constructor for when the Database is
        statically declared (i.e. it is initialized as part of C++ startup
        BEFORE main() is executed.  C/C++ does NOT guarantee the order of static
        initializers.  Since the Database contains a list of IO Card factory
        that are typically statically created/initialized - we need to do
        something to ensure my internal list is properly initialized for this
        scenario - hence this constructor.
     */
    FactoryDatabase( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept;


public:
    /// See Fxt::Point::FactoryDatabaseApi
    FactoryApi* lookup( const char* guidPointTypeId ) noexcept;

    /// See Fxt::Point::FactoryDatabaseApi
    FactoryApi* first() noexcept;

    /// See Fxt::Point::FactoryDatabaseApi
    FactoryApi* next( FactoryApi& currentFactory ) noexcept;

public:
    /// See Fxt::Point::FactoryDatabaseApi
    Api* createPointfromJSON( JsonObject&                        pointObject,
                              Fxt::Type::Error&                  pointErrorCode,
                              Cpl::Memory::ContiguousAllocator&  generalAllocator,
                              Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                              Fxt::Point::DatabaseApi&           dbForPoints,
                              const char*                        pointIdKeyName = "id",
                              bool                               createSetter   = true ) noexcept;

public:
    /// See Fxt::Point::FactoryDatabaseApi
    void insert_( FactoryApi& cardFactoryToAdd ) noexcept;

    /// See Fxt::Point::FactoryDatabaseApi
    void remove_( FactoryApi& cardFactoryToRemove ) noexcept;

private:
    /// Prevent access to the copy constructor -->Databases can not be copied!
    FactoryDatabase( const FactoryDatabaseApi& m );

    /// Prevent access to the assignment operator -->Databases can not be copied!
    const FactoryDatabase& operator=( const FactoryDatabaseApi& m );


protected:
    /// List of supported Point Factories
    Cpl::Container::SList<FactoryApi>   m_factories;
};



};      // end namespaces
};
#endif  // end header latch
