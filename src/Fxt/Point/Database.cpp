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

#include "Database.h"
#include "Cpl/System/FatalError.h"
#include <new>

///
using namespace Fxt::Point;

// global lock
static Cpl::System::Mutex globalMutex_;
StaticJsonDocument<OPTION_FXT_POINT_DATABASE_MAX_CAPACITY_JSON> Database::g_doc_;
uint8_t                                                         Database::g_tempBuffer_[OPTION_FXT_POINT_DATABASE_TEMP_STORAGE_SIZE];

//////////////////////////////////////////////
Database::Database( size_t maxNumPoints ) noexcept
    :m_maxNumPoints( maxNumPoints )
{
    m_points = new(std::nothrow) Info_T[maxNumPoints];
}

Database::~Database()
{
    delete[] m_points;
}

//////////////////////////////////////////////
Fxt::Point::Api* Database::lookupById( const Identifier_T pointIdToFind, bool fatalOnNotFound ) const noexcept
{
    // Look-up point
    if ( m_points && pointIdToFind < m_maxNumPoints )
    {
        return m_points[pointIdToFind].pointInstance;
    }

    // No Point found -->perform a hard failure when requested
    if ( fatalOnNotFound )
    {
        Cpl::System::FatalError::logf( "Point lookup failed. Pt ID=%lu, dbPtr=%p", +pointIdToFind, this );
    }
    return nullptr;
}

const char* Database::getSymbolicName( const Identifier_T pointIdToFind ) const noexcept
{
    if ( m_points && pointIdToFind < m_maxNumPoints )
    {
        return m_points[pointIdToFind].symbolicName;
    }
    return nullptr;
}

bool Database::add( const Identifier_T numericId, Info_T& pointInfo ) noexcept
{
    if ( m_points && numericId < m_maxNumPoints )
    {
        m_points[numericId] = pointInfo;
        return true;
    }
    return false;
}

bool Database::toJSON( const Identifier_T srcPoint,
                       char*              dst,
                       size_t             dstSize,
                       bool&              truncated,
                       bool               verbose ) noexcept
{
    // Look-up the point
    Api* pt = lookupById( srcPoint );
    if ( pt == nullptr )
    {
        return false;
    }

    // Get the metadata
    bool isValid;
    bool isLocked;
    pt->getMetadata( isValid, isLocked );

    // Get access to the Global JSON document
    Database::globalLock_();
    Database::g_doc_.clear();  // Make sure the JSON document is starting "empty"

    // Construct the JSON
    Database::g_doc_["id"]    = +srcPoint;
    Database::g_doc_["valid"] = isValid;
    if ( verbose )
    {
        Database::g_doc_["type"]   = pt->getTypeAsText();
        Database::g_doc_["locked"] = isLocked;
    }

    // Have the Point instance fill in the 'val' details
    bool result = true;
    if ( isValid )
    {
        if ( pt->toJSON_( g_doc_, verbose ) )
        {
            // Generate the actual output string 
            size_t num = serializeJson( Database::g_doc_, dst, dstSize );
            truncated  = num == dstSize ? true : false;
        }
        else
        {
            result = false;
        }
    }

    // Release the Global JSON document
    Database::globalUnlock_();
    return result;
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
        Database::globalUnlock_();
        return false;
    }

    // Valid JSON... Parse the Point identifier
    uint32_t numericId = Database::g_doc_["id"] | ((uint32_t)(-1));
    if ( numericId == ((uint32_t)(-1)) )
    {
        if ( errorMsg )
        {
            *errorMsg = "No valid 'id' key in the JSON input.";
        }
        Database::globalUnlock_();
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
        Database::globalUnlock_();
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
        Database::globalUnlock_();
        return false;
    }

    // Release the Global JSON document
    Database::globalUnlock_();
    return true;
}

void Database::globalLock_() noexcept
{
    globalMutex_.lock();
}

void Database::globalUnlock_() noexcept
{
    globalMutex_.unlock();
}


