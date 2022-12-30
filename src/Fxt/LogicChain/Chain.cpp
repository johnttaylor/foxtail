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
              uint16_t                            numComponents,
              uint16_t                            numAutoPoints )
    : m_components( nullptr )
    , m_autoPoints( nullptr )
    , m_error( Fxt::Type::Error::SUCCESS() )
    , m_numComponents( numComponents )
    , m_numAutoPoints( numAutoPoints )
    , m_nextComponentIdx( 0 )
    , m_nextAutoPtsIdx( 0 )
    , m_started( false )
{
    // Allocate my array of Component pointers
    m_components = (Fxt::Component::Api**) generalAllocator.allocate( sizeof( Fxt::Component::Api* ) * numComponents );
    if ( m_components == nullptr )
    {
        m_numComponents = 0;
        m_error         = fullErr( Err_T::NO_MEMORY_COMPONENT_LIST );
    }
    else
    {
        // Zero the array so we can tell if there are missing components
        memset( m_components, 0, sizeof( Fxt::Component::Api* ) * numComponents );
    }

    // Allocate my array of Auto Point pointers
    m_autoPoints = (Fxt::Point::Api**) generalAllocator.allocate( sizeof( Fxt::Point::Api* ) * numAutoPoints );
    if ( m_autoPoints == nullptr )
    {
        m_numAutoPoints = 0;
        m_error         = fullErr( Err_T::NO_MEMORY_AUTO_POINT_LIST );
    }
    else
    {
        // Zero the array so we can tell if there are missing auto points
        memset( m_autoPoints, 0, sizeof( Fxt::Point::Api* ) * m_numAutoPoints );
    }
}

Chain::~Chain()
{
    // Ensure stop is called first
    stop();

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
Fxt::Type::Error Chain::resolveReferences( Fxt::Point::DatabaseApi & pointDb )  noexcept
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

Fxt::Type::Error Chain::start( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Do nothing if already started
    if ( !m_started && m_error == Fxt::Type::Error::SUCCESS() )
    {
        // Star the individual components (and additional error checking)
        for ( uint16_t i=0; i < m_numComponents; i++ )
        {
            // NOTE: The resolveReferences() method has already validated that
            //       array of components is fully populated with non-null pointers
            if ( m_components[i]->start( currentElapsedTimeUsec ) != Fxt::Type::Error::SUCCESS() )
            {
                m_error = fullErr( Err_T::FAILED_START );
                return m_error;
            }
        }

        // More error checking for Auto Points
        for ( uint16_t i=0; i < m_numAutoPoints; i++ )
        {
            if ( m_autoPoints[i] == nullptr )
            {
                m_error = fullErr( Err_T::MISSING_AUTO_POINTS );
                return m_error;
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
            if ( m_components[i] )
            {
                m_components[i]->stop();
            }
        }

        m_started = false;
    }
}

bool Chain::isStarted() const noexcept
{
    return m_started;
}

Fxt::Type::Error Chain::getErrorCode() const noexcept
{
    return m_error;
}

Fxt::Type::Error Chain::execute( int64_t currentTickUsec ) noexcept
{
    // Only execute if there is no error AND the LC was actually started
    if ( m_error == Fxt::Type::Error::SUCCESS() && m_started )
    {
        // Initialize the Auto points at the start of execution cycle
        for ( uint16_t i=0; i < m_numAutoPoints; i++ )
        {
            m_autoPoints[i]->updateFromSetter();
        }

        // Execute the Components
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


//////////////////////////////////////////////////
Fxt::Type::Error Chain::add( Fxt::Component::Api& componentToAdd ) noexcept
{
    if ( m_error == Fxt::Type::Error::SUCCESS() )
    {
        if ( m_nextComponentIdx >= m_numComponents )
        {
            m_error = fullErr( Err_T::TOO_MANY_COMPONENTS );
        }
        else
        {
            m_components[m_nextComponentIdx] = &componentToAdd;
            m_nextComponentIdx++;
        }
    }

    return m_error;
}

Fxt::Type::Error Chain::add( Fxt::Point::Api& autoPointToAdd ) noexcept
{
    if ( m_error == Fxt::Type::Error::SUCCESS() )
    {
        if ( m_nextAutoPtsIdx >= m_numAutoPoints )
        {
            m_error = fullErr( Err_T::TOO_MANY_AUTO_POINTS );
        }
        else
        {
            m_autoPoints[m_nextAutoPtsIdx] = &autoPointToAdd;
            m_nextAutoPtsIdx++;
        }
    }

    return m_error;
}

//////////////////////////////////////////////////
Api* Api::createLogicChainfromJSON( JsonVariant                         logicChainObject,
                                    Fxt::Component::FactoryDatabaseApi& componentFactory,
                                    Cpl::Memory::ContiguousAllocator&   generalAllocator,
                                    Cpl::Memory::ContiguousAllocator&   haStatefulDataAllocator,
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

    JsonArray components    = logicChainObject["components"];
    size_t    numComponents = components.size();
    if ( numComponents == 0 )
    {
        logicChainErrorode = fullErr( Err_T::NO_COMPONENTS );
        return nullptr;
    }

    // Get the number of auto points
    size_t    numAutoPts = 0;
    JsonArray autoPts;
    if ( logicChainObject["autoPts"].is<JsonArray>() )
    {
        autoPts     = logicChainObject["autoPts"];
        numAutoPts  = autoPts.size();
    }

    // Create Logic Chain instance
    void* memLogicChain = generalAllocator.allocate( sizeof( Chain ) );
    if ( memLogicChain == nullptr )
    {
        logicChainErrorode = fullErr( Err_T::NO_MEMORY_LOGIC_CHAIN );
        return nullptr;
    }
    Api* logicChain = new(memLogicChain) Chain( generalAllocator, (uint16_t) numComponents, (uint16_t) numAutoPts );

    // Create Components
    for ( uint16_t i=0; i < numComponents; i++ )
    {
        Fxt::Type::Error     errorCode     = Fxt::Type::Error::SUCCESS();
        JsonVariant          componentJson = components[i];
        Fxt::Component::Api* component     = componentFactory.createComponentfromJSON( componentJson,
                                                                                       generalAllocator,
                                                                                       haStatefulDataAllocator,
                                                                                       pointFactoryDb,
                                                                                       dbForPoints,
                                                                                       errorCode );
        if ( component == nullptr )
        {
            logicChainErrorode = fullErr( Err_T::FAILED_CREATE_COMPONENT );
            return nullptr;
        }
        if ( errorCode != Fxt::Type::Error::SUCCESS() )
        {
            logicChainErrorode = fullErr( Err_T::COMPONENT_CREATE_ERROR );
            return nullptr;
        }
        logicChainErrorode = logicChain->add( *component );
        if ( logicChainErrorode != Fxt::Type::Error::SUCCESS() )
        {
            return nullptr;
        }
    }

    // Create Connector Points
    if ( logicChainObject["connectionPts"].is<JsonArray>() )
    {
        JsonArray connectionPts = logicChainObject["connectionPts"];
        size_t    numPoints     = connectionPts.size();
        for ( size_t i=0; i < numPoints; i++ )
        {
            Fxt::Type::Error pointError;
            JsonObject       pointJson = connectionPts[i];
            Fxt::Point::Api* pt = pointFactoryDb.createPointfromJSON( pointJson,
                                                                      pointError,
                                                                      generalAllocator,
                                                                      generalAllocator,     // Note: Connector points are NOT part of the HA data
                                                                      dbForPoints,
                                                                      "id",
                                                                      false ); // Do NOT create setters (connector points can NOT have an initial value)
            if ( pt == nullptr )
            {
                logicChainErrorode = fullErr( Err_T::FAILED_CREATE_POINTS );
                return nullptr;
            }
            if ( pointError != Fxt::Type::Error::SUCCESS() )
            {
                logicChainErrorode = fullErr( Err_T::POINT_CREATE_ERROR );
                return nullptr;
            }
        }
    }

    // Create Auto Points
    if ( numAutoPts > 0 )
    {
        for ( size_t i=0; i < numAutoPts; i++ )
        {
            Fxt::Type::Error pointError;
            JsonObject       pointJson = autoPts[i];
            Fxt::Point::Api* pt = pointFactoryDb.createPointfromJSON( pointJson,
                                                                      pointError,
                                                                      generalAllocator,
                                                                      generalAllocator,     // Note: Auto points are NOT part of the HA data
                                                                      dbForPoints,
                                                                      "id",
                                                                      true );
            if ( pt == nullptr )
            {
                logicChainErrorode = fullErr( Err_T::FAILED_CREATE_AUTO_POINTS );
                return nullptr;
            }
            if ( pointError != Fxt::Type::Error::SUCCESS() )
            {
                logicChainErrorode = fullErr( Err_T::AUTO_POINT_CREATE_ERROR );
                return nullptr;
            }
            if ( pt->hasSetter() == false )
            {
                logicChainErrorode = fullErr( Err_T::NO_INITIAL_VAL_AUTO_POINT );
                return nullptr;
            }
            logicChainErrorode = logicChain->add( *pt );
            if ( logicChainErrorode != Fxt::Type::Error::SUCCESS() )
            {
                return nullptr;
            }
        }
    }

    // If I get here -->everything worked
    return logicChain;
}

