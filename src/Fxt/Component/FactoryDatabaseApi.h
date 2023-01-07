#ifndef Fxt_Component_FactoryDatabaseApi_h_
#define Fxt_Component_FactoryDatabaseApi_h_
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

#include "Fxt/Component/Api.h"
#include "Fxt/Component/FactoryApi.h"
#include "Cpl/Container/SList.h"


///
namespace Fxt {
///
namespace Component {


/** This abstract class defines the interface for the Component Factory Database.
    The database contains the list of Factory instances for all supported
    Components

    Note: There is no guaranteed order to the Component Factory list.

    The semantics of the Database interface is NOT thread safe.
 */
class FactoryDatabaseApi: public Cpl::Container::SList<FactoryApi>
{
public:
    /** This method attempts to parse the provided JSON Object that represents
        a Component and create binary representation of the defined component.
        If there is an error (e.g. component not supported, missing key/value
        pairs, etc.) the method returns nullptr; else a pointer to the newly
        created component object is returned.  When an error occurs, details
        about the specific component error is returned via 'componentErrorode'
        argument.

        The method also return's the Component's execution order via the argument
        list.

        See the Fxt::Component::FactoryApi interface for the required JSON Object
        fields.

        This method should ONLY be called after all Factory instances have been
        registered with the Factory Database.
     */
    virtual Api* createComponentfromJSON( JsonVariant&                       componentObject,
                                          Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                          Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                                          Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                                          Fxt::Point::DatabaseApi&           dbForPoints,
                                          Fxt::Type::Error&                  componentErrorCode ) noexcept = 0;

public:
    /** This method looks-up the Component Factory by its GUID.  If the
        Component Factory GUID cannot be found (i.e. card type not supported by
        the platform), THEN the method returns 0.

        The 'guidCardTypeId' is null terminated string contain a 8-4-4-4-12
        formated GUID.
     */
    virtual FactoryApi* lookup( const char* guidComponentTypeId ) noexcept = 0;


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
