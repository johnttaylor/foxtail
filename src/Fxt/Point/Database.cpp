/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Database.h"

///
using namespace Fxt::Point;

// global lock
static Cpl::System::Mutex globalMutex_;
StaticJsonDocument<OPTION_FXT_POINT_DATABASE_MAX_CAPACITY_JSON> Database::g_doc_;

//////////////////////////////////////////////
Database::Database() noexcept
    : m_map()
{
    createLock();
}

//////////////////////////////////////////////
bool Database::createLock()
{
    m_lock = new(std::nothrow) Cpl::System::Mutex();
    if ( m_lock == 0 )
    {
        return false;
    }
    return true;
}

Fxt::Point::Api* Database::lookupById( const Identifier_T pointIdToFind, bool fatalOnNotFound = true ) const noexcept
{
}

//ModelPoint* Database::getFirstByName() noexcept
//{
//    lock_();
//    ModelPoint* result = m_map.first();
//    unlock_();
//    return result;
//}
//
//ModelPoint* Database::getNextByName( ModelPoint& currentModelPoint ) noexcept
//{
//    lock_();
//    ModelPoint* result = m_map.next( currentModelPoint );
//    unlock_();
//    return result;
//}
//
//void Database::insert_( ModelPoint& mpToAdd ) noexcept
//{
//    lock_();
//    m_map.insert( mpToAdd );
//    unlock_();
//}


void Database::globalLock_() noexcept
{
    globalMutex_.lock();
}

void Database::globalUnlock_() noexcept
{
    globalMutex_.unlock();
}

bool Database::fromJSON( const char* src, Cpl::Text::String* errorMsg ) noexcept
{
    // Get access to the Global JSON document
    Database::globalLock_();

    // Parse the JSON payload...
    DeserializationError err = deserializeJson( Database::g_doc_, src );
    if ( err )
    {
        if ( errorMsg )
        {
            *errorMsg = err.c_str();
        }
        return false;
    }

    //    { id=nnnn, valid=nn, locked:true|false, val:<value> }

    // Valid JSON... Parse the Point identifier
    uint32_t numericId = Database::g_doc_["id"] | ((uint32_t)(-1));
    if ( numericId == ((uint32_t)(-1)) )
    {
        if ( errorMsg )
        {
            *errorMsg = "No valid 'id' key in the JSON input.";
        }
        return false;
    }

    // Look-up the Point name
    Api* pt = lookupById( numericId, false );
    if ( pt == nullptr )
    {
        if ( errorMsg )
        {
            errorMsg->format( "Point ID (%u) NOT found.", numericId );
        }
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
        return false;
    }

    // Release the Global JSON document
    Database::globalUnlock_();
    return true;
}
