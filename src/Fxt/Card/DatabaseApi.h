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


///
namespace Fxt {
///
namespace Card {


/** This class defines the basic operations that can be performed on a Card
    Database.  A Card Database is a collection of instantiated Cards.

    Unless explicitly commented - the Database Interface can ONLY be called
    from the "Chassis Thread"
 */
class DatabaseApi
{
public:
    /** This method performs a generic looks-up of a Card instance by
        its numeric Identifier and returns a pointer to the instance.  If the
        Card identifier cannot be found a nullptr is returned.

        This method is ONLY okay to call from ANY thread AFTER the Card database
        has been populated.

        This method is not type-safe because it returns point to the Card base
        class (which does not have the read/write operations).  The application
        is required to properly down case the pointer to appropriate child class.
    */
    virtual Fxt::Card::Api* lookupById( uint32_t cardIdToFind ) const noexcept = 0;

    /** This method returns the maximum number of Cards that the database
        can store.
    */
    virtual size_t getMaxNumPoints() const noexcept = 0;


public:
    /** This method is use clear/empties/resets the Card Database.  This method
        is responsible for calling the destructor on all existing cards
        prior to resetting.

        Note: Freeing of the Card memory is the responsibility application since
              the memory comes from the application's allocators
     */
    virtual void clearCards() noexcept = 0;


public:
    /** This adds a Card instance to the database.  False is returned when
        there is insufficient memory to add the Card; else true is returned.

        The application is responsible ensuring that all "Card IDs" are unique.
        If the new instance's "card-id" already exists in the database false
        is returned; else true is returned.

        This method is can ONLY be called from a "Chassis Thread"
     */
    virtual bool add( Api& cardInstanceToAdd ) noexcept = 0;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~DatabaseApi() {}
};

};      // end namespaces
};
#endif  // end header latch
