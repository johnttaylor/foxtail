#ifndef Fxt_Point_Database_h_
#define Fxt_Point_Database_h_
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
#include "Fxt/Point/DatabaseApi.h"
#include "Cpl/Json/Arduino.h"
#include "Cpl/Container/Dictionary.h"
#include "Cpl/System/Mutex.h"


/** This symbol defines the size, in bytes, of a single/global JSON document
    buffer that is used for the toJSON() and fromJSON() operations. Only one
    instance of this buffer is allocated.
*/
#ifndef OPTION_FXT_POINT_DATABASE_MAX_CAPACITY_JSON
#define OPTION_FXT_POINT_DATABASE_MAX_CAPACITY_JSON          (1024*2)
#endif

/** This symbol defines the size, in bytes, of temporary storage allocated for
    use by the fromJSON_() method (e.g. create a temporary array instance)
 */
#ifndef OPTION_FXT_POINT_DATABASE_TEMP_STORAGE_SIZE
#define OPTION_FXT_POINT_DATABASE_TEMP_STORAGE_SIZE         (1024*2)
#endif


 ///
namespace Fxt {
///
namespace Point {


/** This concrete template class implements a simple Point Database.

    Template Args:
        N   - The maximum number of Points that can be stored in the Database
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
    /// See Fxt::Point::DatabaseApi
    Fxt::Point::Api* lookupById( uint32_t pointIdToFind ) const noexcept;

    /// See Fxt::Point::DatabaseApi
    size_t getMaxNumPoints() const noexcept;

    /// See Fxt::Point::DatabaseApi
    Fxt::Point::Api* next( Fxt::Point::Api& currentPoint ) const noexcept;

    /// See Fxt::Point::DatabaseApi
    bool toJSON( uint32_t         pointId,
                 char*            dst,
                 size_t           dstSize,
                 bool&            truncated,
                 bool             verbose = true,
                 bool             pretty  = true ) noexcept;

    /// See Fxt::Point::DatabaseApi
    bool fromJSON( const char* src, Cpl::Text::String* errorMsg=0 ) noexcept;

    /// See Fxt::Point::DatabaseApi
    bool add( Api& pointInstanceToAdd ) noexcept;

    /// See Fxt::Point::DatabaseApi
    void clearPoints() noexcept;

public:
    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Cpl::Point namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        This method provides a single global lock for ALL Point Database
        instances. The method is used to protect global Point Database (e.g.
        the global parse buffer).

        This method locks the global Point Database lock. For every call to
        globalLock_() there must be corresponding call to globalUnlock_();
    */
    static void globalLock_() noexcept;

    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Cpl::Point namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        This method unlocks the global Point Database lock
    */
    static void globalUnlock_() noexcept;

    /** This variable has 'PACKAGE Scope' in that is should only be called by
        other classes in the Cpl::Point namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        Global/single instance of a JSON document. Model Point's need to have
        acquired the global lock before using this buffer
     */
    static StaticJsonDocument<OPTION_FXT_POINT_DATABASE_MAX_CAPACITY_JSON> g_doc_;

    /** This variable has 'PACKAGE Scope' in that is should only be called by
        other classes in the Cpl::Point namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        Global temporary buffer. Model Point's need to have acquired the global
        lock before using this buffer
     */
    static uint8_t   g_tempBuffer_[OPTION_FXT_POINT_DATABASE_TEMP_STORAGE_SIZE];

private:
    /// Prevent access to the copy constructor -->Point Databases can not be copied!
    Database( const Database& m );

    /// Prevent access to the assignment operator -->Point Databases can not be copied!
    const Database& operator=( const Database& m );

protected:
    /// Memory for Point table.  Note: A Point ID is its index into m_points.
    Fxt::Point::Api*            m_points[N];

    /// Mutex for the global lock
    static Cpl::System::Mutex   g_globalMutex;
};

/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////

template <int N> Cpl::System::Mutex                                              Database<N>::g_globalMutex;
template <int N> StaticJsonDocument<OPTION_FXT_POINT_DATABASE_MAX_CAPACITY_JSON> Database<N>::g_doc_;
template <int N> uint8_t                                                         Database<N>::g_tempBuffer_[OPTION_FXT_POINT_DATABASE_TEMP_STORAGE_SIZE];


template <int N>
Database<N>::Database( void ) noexcept
{
    memset( m_points, 0, sizeof( m_points ) );
}

template <int N>
Database<N>::Database( const char* dummyArgUsedToCreateStaticConstructorSignature ) noexcept
{
    // Nothing needed since I am statically allocated and memory is all zeros at this point
}

template <int N>
Fxt::Point::Api* Database<N>::lookupById( uint32_t pointIdToFind ) const noexcept
{
    if ( pointIdToFind >= N )
    {
        return nullptr;
    }
    return m_points[pointIdToFind];
}

template <int N>
size_t Database<N>::getMaxNumPoints() const noexcept
{
    return N;
}

template <int N>
bool Database<N>::add( Api& pointToAdd ) noexcept
{
    // Prevent duplicate and out-of-range IDs
    uint32_t id = pointToAdd.getId();
    if ( id >= N || m_points[id] != 0 )
    {
        return false;

    }
    m_points[id] = &pointToAdd;
    return true;
}

template <int N>
void Database<N>::clearPoints() noexcept
{
    // Walk all possible points
    for ( int i=0; i < N; i++ )
    {
        // Call the Point's destructor and then remove the point from the DB
        if ( m_points[i] != nullptr )
        {
            m_points[i]->~Api();
            m_points[i] = nullptr;
        }
    }
}


template <int N>
void Database<N>::globalLock_() noexcept
{
    g_globalMutex.lock();
}

template <int N>
void Database<N>::globalUnlock_() noexcept
{
    g_globalMutex.unlock();
}

template <int N>
bool Database<N>::toJSON( uint32_t         pointId,
                          char*            dst,
                          size_t           dstSize,
                          bool&            truncated,
                          bool             verbose,
                          bool             pretty ) noexcept
{
    // Get the point instance
    Api* point = lookupById( pointId );
    if ( point == nullptr )
    {
        return false;
    }

    // Get the metadata
    bool isValid;
    bool isLocked;
    point->getMetadata( isValid, isLocked );

    // Get access to the Global JSON document
    Database<N>::globalLock_();
    Database<N>::g_doc_.clear();  // Make sure the JSON document is starting "empty"

    // Construct the JSON
    Database<N>::g_doc_["id"]    = point->getId();
    Database<N>::g_doc_["valid"] = isValid;
    if ( verbose )
    {
        Database<N>::g_doc_["type"]   = point->getType();
        Database<N>::g_doc_["locked"] = isLocked;
    }

    // Have the Point instance fill in the 'val' details
    bool result = true;
    if ( isValid && !point->toJSON_( Database<N>::g_doc_, verbose ) )
    {
        result = false;
    }

    // Generate the actual output string 
    if ( result )
    {
        size_t jsonLen;
        size_t outputLen;
        if ( !pretty )
        {
            jsonLen   = measureJson( Database<N>::g_doc_ );
            outputLen = serializeJson( Database<N>::g_doc_, dst, dstSize );
        }
        else
        {
            jsonLen   = measureJsonPretty( Database<N>::g_doc_ );
            outputLen = serializeJsonPretty( Database<N>::g_doc_, dst, dstSize );
        }
        truncated = outputLen == jsonLen ? false : true;
    }

    // Release the Global JSON document
    Database<N>::globalUnlock_();
    return result;
}

template <int N>
bool Database<N>::fromJSON( const char* src, Cpl::Text::String* errorMsg ) noexcept
{
    // Get access to the Global JSON document
    Database<N>::globalLock_();

    // Parse the JSON payload...
    DeserializationError err = deserializeJson( Database<N>::g_doc_, src );
    if ( err )
    {
        if ( errorMsg )
        {
            *errorMsg = err.c_str();
        }
        Database::globalUnlock_();
        return false;
    }

    // Valid JSON... Parse the Point identifier
    if ( Database<N>::g_doc_["id"].isNull() )
    {
        if ( errorMsg )
        {
            *errorMsg = "No valid 'id' key in the JSON input.";
        }
        Database<N>::globalUnlock_();
        return false;
    }
    uint32_t numericId = Database<N>::g_doc_["id"];

    // Look-up the Point name
    Api* pt = lookupById( numericId );
    if ( pt == nullptr )
    {
        if ( errorMsg )
        {
            errorMsg->format( "Point ID (%u) NOT found.", numericId );
        }
        Database<N>::globalUnlock_();
        return false;
    }

    // Attempt to parse the key/value pairs of interest
    JsonVariant validKey = Database::g_doc_["valid"];
    JsonVariant locked   = Database::g_doc_["locked"];
    JsonVariant valElem  = Database::g_doc_["val"];
    Api::LockRequest_T lockAction = Api::eNO_REQUEST;
    if ( locked.isNull() == false )
    {
        lockAction = locked.as<bool>() ? Api::eLOCK : Api::eUNLOCK;
    }

    // Request to invalidate the MP
    if ( validKey.isNull() == false && validKey.as<bool>() == false )
    {
        pt->setInvalid( lockAction );
    }

    // Write a valid value to the MP
    else if ( valElem.isNull() == false )
    {
        if ( pt->fromJSON_( valElem, lockAction, errorMsg ) == false )
        {
            Database::globalUnlock_();
            return false;
        }
    }

    // Just lock/unlock the MP
    else if ( locked.isNull() == false )
    {
        pt->setLockState( lockAction );
    }

    // Bad Syntax
    else
    {
        if ( errorMsg )
        {
            *errorMsg = "JSON syntax is not valid or invalid payload semantics";
        }
        Database<N>::globalUnlock_();
        return false;
    }

    // Release the Global JSON document
    Database::globalUnlock_();
    return true;
}


};      // end namespaces
};
#endif  // end header latch
