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

#include "colony_config.h"
#include "Fxt/Card/DatabaseApi.h"
#include "Cpl/Json/Arduino.h"
#include "Cpl/System/Mutex.h"




///
namespace Fxt {
///
namespace Card {


/** This concrete template class implements a simple Card Database.

    Template Args:
        N   - The maximum number of Card that can be stored in the Database
  */
template<int N>
class Database : public DatabaseApi
{
public:
    /// Constructor.  Use this constructor when creating the instance AFTER main() executes
    Database() noexcept;

    /// Constructor.  Use this constructor when creating a static instance, i.e. BEFORE main() executes
    Database( const char* dummyArgUsedToCreateStaticConstructorSignature ) noexcept;

public:
    /// See Fxt::Card::DatabaseApi
    Fxt::Card::Api* lookupById( uint32_t cardIdToFind ) const noexcept;

    /// See Fxt::Card::DatabaseApi
    size_t getMaxNumPoints() const noexcept;

    /// See Fxt::Card::DatabaseApi
    bool add( Api& cardInstanceToAdd ) noexcept;

    /// See Fxt::Card::DatabaseApi
    void clearCards() noexcept;


private:
    /// Prevent access to the copy constructor -->Point Databases can not be copied!
    Database( const DatabaseApi& m );

    /// Prevent access to the assignment operator -->Point Databases can not be copied!
    const Database& operator=( const DatabaseApi& m );

protected:
    /// Memory for Point table.  Note: A Point ID is its index into m_cards.
    Fxt::Card::Api*             m_cards[N];
};

/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////


template <int N>
Database<N>::Database( void ) noexcept
{
    memset( m_cards, 0, sizeof( m_cards ) );
}

template <int N>
Database<N>::Database( const char* dummyArgUsedToCreateStaticConstructorSignature ) noexcept
{
    // Nothing needed since I am statically allocated and memory is all zeros at this point
}

template <int N>
Fxt::Card::Api* Database<N>::lookupById( uint32_t cardIdToFind ) const noexcept
{
    if ( cardIdToFind >= N )
    {
        return nullptr;
    }
    return m_cards[cardIdToFind];
}

template <int N>
size_t Database<N>::getMaxNumPoints() const noexcept
{
    return N;
}

template <int N>
bool Database<N>::add( Api& cardToAdd ) noexcept
{
    // Prevent duplicate and out-of-range IDs
    uint32_t id = cardToAdd.getId();
    if ( id >= N || m_cards[id] != 0 )
    {
        return false;

    }
    m_cards[id] = &cardToAdd;
    return true;
}

template <int N>
void Database<N>::clearCards() noexcept
{
    // Walk all possible points
    for ( int i=0; i < N; i++ )
    {
        // Call the Point's destructor and then remove the point from the DB
        if ( m_cards[i] != nullptr )
        {
            m_cards[i]->~Api();
            m_cards[i] = nullptr;
        }
    }
}




};      // end namespaces
};
#endif  // end header latch
