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


#include "Error.h"
#include "Cpl/System/Assert.h"
#include "Fxt/Logging/Api.h"

#define MAX_ERR_MSG 128

///
using namespace Fxt::Type;


/////////////////////////////////////////////////////
Error::Error( const ErrorBase& leafErrorCategory, uint8_t localCategoryErrorNumber )
    : errVal( 0 )
{
    CPL_SYSTEM_ASSERT( localCategoryErrorNumber < 128 );

    // Build error code
    if ( localCategoryErrorNumber > 0 )
    {
        uint8_t    levelShift = leafErrorCategory.getLevelIndex() * 8;
        errVal               |= ((uint32_t) localCategoryErrorNumber) << levelShift;
        uint8_t    catId      = leafErrorCategory.getCategoryIdentifier();
        ErrorBase* parent     = leafErrorCategory.getParent();

        while ( parent )
        {
            levelShift -= 8;
            errVal     |= ((uint32_t) catId) << levelShift;
            catId       = parent->getCategoryIdentifier();
            parent      = parent->getParent();
        }
    }
}


/////////////////////////////////////////////////////

static void categoryToText( uint32_t fullErrValue, uint8_t levelShift, Cpl::Text::String& buffer, ErrorBase& category ) noexcept
{
    uint8_t     levelErrValue = (fullErrValue >> levelShift) & 0xFF;
    const char* text;

    // Check for Zero/Success
    if ( levelErrValue == 0 )
    {
        text = "SUCCESS";
    }

    // Current level is enum value, i.e. leaf value/level
    else if ( levelErrValue < 128 )
    {
        text = category.getLocalText( levelErrValue );
        if ( text == nullptr )
        {
            text = Error::UNKNOWN_TEXT;
        }
    }

    // Level value is a child category
    else
    {
        ErrorBase* child = category.getFirstChild();
        while ( child )
        {
            if ( child->getCategoryIdentifier() == levelErrValue )
            {
                // Update the text
                if ( levelShift != 0 )
                {
                    buffer += ':';
                }
                buffer += child->getCategoryName();

                categoryToText( fullErrValue, levelShift + 8, buffer, *child );
                return;
            }

            child = category.getNextChild( *child );
        }

        // Unknown category
        text = Error::UNKNOWN_TEXT;
    }

    // Update the text
    if ( levelShift != 0 )
    {
        buffer += ':';
    }
    buffer += text;
}


const char* Error::toText( Error errCode, Cpl::Text::String& buffer ) noexcept
{
    buffer.clear();
    categoryToText( errCode.errVal, 0, buffer, Error::getErrorCategoriesRoot() );
    return buffer.getString();
}

void Error::logIt( const char* optionalErrMsg ) const noexcept
{
    Cpl::Text::FString<MAX_TEXT_LEN + 2 + 8 + 1> buf;
    Fxt::Logging::logf( Fxt::Logging::ErrCodeMsg::ERRVALUE, "%s (%08lX). %s", toText(buf), errVal, optionalErrMsg? optionalErrMsg: "" );
}


void Error::logItFormatted( const char* msgFormat, ... ) const noexcept
{
    va_list ap;
    va_start( ap, msgFormat );
    Cpl::Text::FString<MAX_ERR_MSG> buf;
    buf.vformat( msgFormat, ap );
    logIt( buf.getString() );
    va_end( ap );
}

///////////////////////////////////////////
ErrorBase::ErrorBase( ErrorBase&   parentCategory,
                      uint8_t      levelIndex,
                      const char*  nameForErrorCategory )
    : m_children()
    , m_parent( &parentCategory )
    , m_name( nameForErrorCategory )
    , m_numChildren( 0 )
    , m_level( levelIndex )
{
    // Self register with my Parent and get/generate my category ID
    parentCategory.m_children.put( *this );
    parentCategory.m_numChildren++;
    m_categoryId = parentCategory.m_numChildren + 127;
    m_level      = parentCategory.m_level + 1;
}

ErrorBase::ErrorBase( ErrorBase&   parentCategory,
                      uint8_t      levelIndex,
                      const char*  nameForErrorCategory,
                      const char*  ignoreThisParameter_usedToCreateAUniqueConstructor )
    : m_children( ignoreThisParameter_usedToCreateAUniqueConstructor )
    , m_parent( &parentCategory )
    , m_name( nameForErrorCategory )
    , m_level( levelIndex )
{
    // Self register with my Parent and get/generate my category ID
    parentCategory.m_children.put( *this );
    parentCategory.m_numChildren++;
    m_categoryId = parentCategory.m_numChildren + 127;
}

ErrorBase::ErrorBase()
    : m_children()
    , m_parent( nullptr )
    , m_name( nullptr )
    , m_numChildren( 0 )
    , m_level( 0 )
{
}

ErrorBase::ErrorBase( const char* ignoreThisParameter_usedToCreateAUniqueConstructor )
    : m_children( ignoreThisParameter_usedToCreateAUniqueConstructor )
    , m_parent( nullptr )
    , m_name( nullptr )
{
}

ErrorBase* ErrorBase::getFirstChild() const noexcept
{
    return m_children.first();
}

ErrorBase* ErrorBase::getNextChild( ErrorBase& currentChild ) const noexcept
{
    return m_children.next( currentChild );
}

///////////////////////////////////////////
ErrorCategoryRoot::ErrorCategoryRoot()
    : ErrorBase()
{
}

ErrorCategoryRoot::ErrorCategoryRoot( const char* ignoreThisParameter_usedToCreateAUniqueConstructor )
    : ErrorBase( ignoreThisParameter_usedToCreateAUniqueConstructor )
{
}

const char* ErrorCategoryRoot::getLocalText( uint8_t localCategoryErrorNumber ) const noexcept
{
    return nullptr;
}
