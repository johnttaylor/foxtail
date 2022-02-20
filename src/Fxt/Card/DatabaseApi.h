#ifndef Fxt_Card_DatabaseApi_h_
#define Fxt_Card_DatabaseApi_h_
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


/** This abstract class defines the interface for the IO Card Database.  The
    database contains the list of all currently created IO Cards.  It also
    contains the list of all support IO Card type (aka. factory instances)
    for the HW platform.

    Note: There is no guaranteed order to the IO Card and IO Card Factory
          lists.

    The semantics of the Database interface is NOT thread safe.
 */
class DatabaseApi
{
public:
    /** This method looks-up the IO card by its 'User facing local ID'.  If the 
        IO Card ID cannot be found, THEN the method returns 0.
     */
    virtual Api* lookupCard( uint16_t cardLocalId ) noexcept = 0;

    /** This method returns a pointer to the first IO Card in the Database. If 
        there are no IO Cards in the Database, THEN the method returns 0.
     */
    virtual Api* getFirstCard() noexcept = 0;

    /** This method returns the next IO Card in the Database. If 'currentCard' 
        is the last IO Card in the Database the method returns 0.
     */
    virtual Api* getNextCard( Api& currentCard ) noexcept = 0;


public:
    /** This method looks-up the IO card Factory by its GUID.  If the
        IO Card Factory GUID cannot be found (i.e. card type not supported by
        the platform), THEN the method returns 0.
     */
    virtual FactoryApi* lookupFactory( Cpl::Type::Guid_T cardTypeId ) noexcept = 0;

    /** This method returns a pointer to the first IO Card Factory in the 
        Database. If there are no IO Card Factories in the Database, THEN the 
        method returns 0.
     */
    virtual FactoryApi* getFirstFactory() noexcept = 0;

    /** This method returns the next IO Card Factory in the Database. If 
        'currentFactory' is the last IO Card Factory in the Database the method 
        returns 0.
     */
    virtual Api* getNextFactory( FactoryApi& currentFactory ) noexcept = 0;


public:
    /// Virtual destructor to make the compiler happy
    virtual ~DatabaseApi() {}
};


};      // end namespaces
};
#endif  // end header latch
