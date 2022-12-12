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
class FactoryDatabaseApi
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
                                      const char*                        pointIdKeyName = "id" ) noexcept = 0;

public:
    /** This method looks-up the Point Factory by its GUID.  If the
        Point Factory GUID cannot be found (i.e. card type not supported by
        the platform), THEN the method returns 0.

        The 'guidCardTypeId' is null terminated string contain a 8-4-4-4-12
        formated GUID.
     */
    virtual FactoryApi* lookup( const char* guidPointTypeId ) noexcept = 0;

    /** This method returns a pointer to the first Point Factory in the
        Database. If there are no Point Factories in the Database, THEN the
        method returns 0.
     */
    virtual FactoryApi* first() noexcept = 0;

    /** This method returns the next Point Factory in the Database. If
        'currentFactory' is the last Point Factory in the Database the method
        returns 0.
     */
    virtual FactoryApi* next( FactoryApi& currentFactory ) noexcept = 0;


public:
    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Fxt::Card namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        NOTE: There is NO checks/protection against adding two factories with
              the same card GUID

        This method inserts a Point Factory instance into the Database.
     */
    virtual void insert_( FactoryApi& pointFactoryToAdd ) noexcept = 0;

    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Fxt::Card namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        This method removes a Point factory from the Database.
     */
    virtual void remove_( FactoryApi& pointFactoryToRemove ) noexcept = 0;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~FactoryDatabaseApi() {}
};


};      // end namespaces
};
#endif  // end header latch
