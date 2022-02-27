#ifndef Fxt_Card_Database_h_
#define Fxt_Card_Database_h_
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


#include "Fxt/Card/DatabaseApi.h"
#include "Cpl/Container/SList.h"

///
namespace Fxt {
///
namespace Card {


/** This concrete class implements the DatabaseApi

    NOTE: This class is NOT thread safe
 */
class Database : public DatabaseApi
{
public:
    /// Constructor
    Database() noexcept;

    /** This is a special constructor for when the Database is
        statically declared (i.e. it is initialized as part of C++ startup 
        BEFORE main() is executed.  C/C++ does NOT guarantee the order of static 
        initializers.  Since the Database contains a list of IO Card factory 
        that are typically statically created/initialized - we need to do 
        something to ensure my internal list is properly initialized for this 
        scenario - hence this constructor.
     */
    Database( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept;


public:
    /// See Fxt::Card::DatabaseApi
    Api* lookupCard( uint32_t cardLocalId ) noexcept;

    /// See Fxt::Card::DatabaseApi
    Api* getFirstCard() noexcept;

    /// See Fxt::Card::DatabaseApi
    Api* getNextCard( Api& currentCard ) noexcept;

    /// See Fxt::Card::DatabaseApi
    FactoryApi* lookupFactory( const char* guidCardTypeId ) noexcept;

    /// See Fxt::Card::DatabaseApi
    FactoryApi* getFirstFactory() noexcept;

    /// See Fxt::Card::DatabaseApi
    FactoryApi* getNextFactory( FactoryApi& currentFactory ) noexcept;


public:
    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Fxt::Card namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        This method inserts a IO Card instance t into the Model Database.
   */
    void insert_( Api& cardToAdd ) noexcept;

    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Fxt::Card namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        This method removes a IO Card instance t from the Model Database.
   */
    void remove_( Api& cardToRemove ) noexcept;


public:
    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Fxt::Card namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        This method inserts a IO Card Factory instance t into the Model Database.
   */
    void insert_( FactoryApi& cardFactoryToAdd ) noexcept;


private:
    /// Prevent access to the copy constructor -->Databases can not be copied!
    Database( const Database& m );

    /// Prevent access to the assignment operator -->Databases can not be copied!
    const Database& operator=( const Database& m );


protected:
    /// List of active IO cards
    Cpl::Container::SList<Api>          m_cards;

    /// List of supported IO card Factories
    Cpl::Container::SList<FactoryApi>   m_factories;
};



};      // end namespaces
};
#endif  // end header latch
