#ifndef Fxt_Card_FactoryDatabaseApi_h_
#define Fxt_Card_FactoryDatabaseApi_h_
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

#include "Fxt/Card/Api.h"
#include "Fxt/Card/FactoryApi.h"


///
namespace Fxt {
///
namespace Card {


/** This abstract class defines the interface for the IO Card Factory Database.  
    The database contains the list of Factory instances for all support IO
    card types.

    Note: There is no guaranteed order IO Card Factory list.

    The semantics of the Database interface is NOT thread safe.
 */
class FactoryDatabaseApi
{
public:
    /** This method attempts to parse the provided JSON Object that represents
        a CARD and create binary representation of the defined card.  If there
        is an error (e.g. card not supported, missing key/value pairs, etc.) the
        method returns nullptr; else a pointer to the newly created card object
        is returned.  When an error occurs, details about the specific card error
        is returned via 'cardErrorode' argument.
        
        See the Fxt::Card::FactoryApi interface for the required JSON Object 
        fields.

        This method should ONLY be called after all Factory instances have been
        registered with the Factory Database.
     */
    virtual Api* createCardfromJSON( DatabaseApi&                       cardDb, 
                                     JsonVariant                        cardObj, 
                                     Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                     Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                                     Fxt::Point::DatabaseApi&           dbForPoints, 
                                     Api::Err_T&                        cardErrorCode ) noexcept = 0;

public:
    /** This method looks-up the IO card Factory by its GUID.  If the
        IO Card Factory GUID cannot be found (i.e. card type not supported by
        the platform), THEN the method returns 0.

        The 'guidCardTypeId' is null terminated string contain a 8-4-4-4-12 
        formated GUID.
     */
    virtual FactoryApi* lookup( const char* guidCardTypeId ) noexcept = 0;

    /** This method returns a pointer to the first IO Card Factory in the 
        Database. If there are no IO Card Factories in the Database, THEN the 
        method returns 0.
     */
    virtual FactoryApi* first() noexcept = 0;

    /** This method returns the next IO Card Factory in the Database. If 
        'currentFactory' is the last IO Card Factory in the Database the method 
        returns 0.
     */
    virtual FactoryApi* next( FactoryApi& currentFactory ) noexcept = 0;


public:
    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Fxt::Card namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        This method inserts a IO Card Factory instance t into the Database.
     */
    virtual void insert_( FactoryApi& cardFactoryToAdd ) noexcept = 0;

    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Fxt::Card namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        This method removes a IO Card factory from the Database.
     */
    virtual void remove_( FactoryApi& cardFactoryToRemove ) noexcept = 0;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~FactoryDatabaseApi() {}
};


};      // end namespaces
};
#endif  // end header latch
