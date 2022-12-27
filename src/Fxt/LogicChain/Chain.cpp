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


#include "Chain.h"
#include "Cpl/System/Assert.h"
#include <new>


///
using namespace Fxt::LogicChain;

//////////////////////////////////////////////////
Chain::Chain( Cpl::Memory::ContiguousAllocator&   generalAllocator,
              uint16_t                            numComponents )
    : m_components( nullptr )
    , m_error( Fxt::Type::Error::SUCCESS() )
    , m_numComponents( numComponents )
    , m_nextIdx( 0 )
{
    // Allocate my array of Component pointers
    m_components = (Fxt::Component::Api**) generalAllocator.allocate( sizeof( Api* ) * numComponents );
    if ( m_components == nullptr )
    {
        m_numComponents = 0;
        m_error         = fullErr( Err_T::NO_MEMORY_COMPONENT_LIST );
    }
    else
    {
        // Zero the array so we can tell if there are missing components
        memset( m_components, 0, sizeof( Api* ) * numComponents );
    }
}

Chain::~Chain()
{
    // Call the destructors on all of the components
    for ( uint16_t i=0; i < m_numComponents; i++ )
    {
        if ( m_components[i] )
        {
            m_components[i]->~Api();
        }
    }
}

//////////////////////////////////////////////////
Fxt::Type::Error Chain::start( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Do nothing if already started
    if ( !m_started )
    {
        if ( m_error == Fxt::Type::Error::SUCCESS() )
        {
            for ( uint16_t i=0; i < m_numComponents; i++ )
            {
                if ( m_components[i] == nullptr )
                {
                    m_error = fullErr( Err_T::MISSING_COMPONENTS );
                    break;
                }
                if ( m_components[i]->start( currentElapsedTimeUsec ) != Fxt::Type::Error::SUCCESS() )
                {
                    m_error = fullErr( Err_T::FAILED_START );
                    break;
                }
            }
        }

        m_started = true;
    }

    return m_error;
}

void Chain::stop() noexcept
{
    // Only stop if I have been started
    if ( m_started )
    {
        // Always stop the components - even if there is an internal error
        for ( uint16_t i=0; i < m_numComponents; i++ )
        {
            m_components[i]->stop();
        }
    }
}

/// See Fxt::LogicChain::Api
bool Chain::isStarted() const noexcept
{
    return m_started;
}

Fxt::Type::Error Chain::execute( int64_t currentTickUsec ) noexcept
{
    // Only execute if there is no error AND the LC was actually started
    if ( m_error == Fxt::Type::Error::SUCCESS() && m_started )
    {
        for ( uint16_t i=0; i < m_numComponents; i++ )
        {
            if ( m_components[i]->execute( currentTickUsec ) != Fxt::Type::Error::SUCCESS() )
            {
                m_error = fullErr( Err_T::COMPONENT_FAILURE );
                break;
            }
        }
    }

    return m_error;
}


Fxt::Type::Error Chain::getErrorCode() const noexcept
{
    return m_error;
}


//////////////////////////////////////////////////
Fxt::Type::Error Chain::add( Fxt::Component::Api& componentToAdd ) noexcept
{
    if ( m_error == Fxt::Type::Error::SUCCESS() )
    {
        if ( m_nextIdx >= m_numComponents )
        {
            m_error = fullErr( Err_T::TOO_MANY_COMPONENTS );
        }
        else
        {
            m_components[m_nextIdx] = &componentToAdd;
            m_nextIdx++;
        }
    }

    return m_error;
}

Api* Api::createLogicChainfromJSON( JsonVariant                         logicChainObject,
                                    Fxt::Component::FactoryDatabaseApi& componentFactory,
                                    Fxt::Point::BankApi&                statePointBank,
                                    Cpl::Memory::ContiguousAllocator&   generalAllocator,
                                    Cpl::Memory::ContiguousAllocator&   statefulDataAllocator,
                                    Fxt::Point::FactoryDatabaseApi&     pointFactoryDb,
                                    Fxt::Point::DatabaseApi&            dbForPoints,
                                    Fxt::Type::Error&                   logicChainErrorode ) noexcept
{
    // Minimal syntax checking of the JSON input
    if ( logicChainObject["components"].is<JsonArray>() == false )
    {
        logicChainErrorode = fullErr( Err_T::PARSE_COMPONENT_ARRAY );
        return nullptr;
    }

    JsonArray components  = logicChainObject["components"];
    uint16_t  numComponents = components.size();
    if ( numComponents == 0 )
    {
        logicChainErrorode = fullErr( Err_T::NO_COMPONENTS );
        return nullptr;
    }

    // Create Logic Chain instance
    void* memLogicChain = generalAllocator.allocate( sizeof( Chain ) );
    if ( memLogicChain == nullptr )
    {
        logicChainErrorode = fullErr( Err_T::NO_MEMORY_LOGIC_CHAIN );
        return nullptr;
    }
    Api* logicChain = new(memLogicChain) Chain( generalAllocator, numComponents );

    //  Create Components
    for ( uint16_t i=0; i < numComponents; i++ )
    {
        Fxt::Type::Error     errorCode     = Fxt::Type::Error::SUCCESS();
        JsonVariant          componentJson = components[i];
        Fxt::Component::Api* component     = componentFactory.createComponentfromJSON( componentJson,
                                                                                       statePointBank,
                                                                                       generalAllocator,
                                                                                       statefulDataAllocator,
                                                                                       pointFactoryDb,
                                                                                       dbForPoints,
                                                                                       errorCode );
        if ( component == nullptr )
        {
            logicChainErrorode = fullErr( Err_T::FAILED_CREATE_COMPONENT );
            return nullptr;
        }
        logicChainErrorode = logicChain->add( *component );
        if ( logicChainErrorode != Fxt::Type::Error::SUCCESS() )
        {
            return nullptr;
        }
    }

    // If I get here -->everything worked
    return logicChain;
}

Fxt::Type::Error Chain::resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept
{
    if ( m_error == Fxt::Type::Error::SUCCESS() )
    {
        for ( uint16_t i=0; i < m_numComponents; i++ )
        {
            if ( m_components[i] == nullptr )
            {
                m_error = fullErr( Err_T::MISSING_COMPONENTS );
                break;
            }
            if ( m_components[i]->resolveReferences( pointDb ) != Fxt::Type::Error::SUCCESS() )
            {
                m_error = fullErr( Err_T::FAILED_POINT_RESOLVE );
                break;
            }
        }
    }

    return m_error;
}
