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


#include "ExecutionSet.h"
#include "Error.h"
#include "Cpl/System/Assert.h"
#include <new>


///
using namespace Fxt::Chassis;

//////////////////////////////////////////////////
ExecutionSet::ExecutionSet( Cpl::Memory::ContiguousAllocator&   generalAllocator,
                            uint16_t                            numLogicChains,
                            size_t                              exeRateMultipler )
    : m_logicChains( nullptr )
    , m_error( Fxt::Type::Error::SUCCESS() )
    , m_erm( exeRateMultipler )
    , m_numLogicChains( numLogicChains )
    , m_nextLogicChainIdx( 0 )
{
    // Allocate my array of Component pointers
    m_logicChains = (Fxt::LogicChain::Api**) generalAllocator.allocate( sizeof( Fxt::LogicChain::Api* ) * numLogicChains );
    if ( m_logicChains == nullptr )
    {
        m_numLogicChains = 0;
        m_error         = fullErr( Err_T::NO_MEMORY_LOGIC_CHAIN_LIST );
    }
    else
    {
        // Zero the array so we can tell if there are missing components
        memset( m_logicChains, 0, sizeof( Fxt::LogicChain::Api* ) * numLogicChains );
    }
}

ExecutionSet::~ExecutionSet()
{
    // Ensure stop is called first
    stop();

    // Call the destructors on all of the Logic Chains
    for ( uint16_t i=0; i < m_numLogicChains; i++ )
    {
        if ( m_logicChains[i] )
        {
            m_logicChains[i]->~Api();
        }
    }
}

//////////////////////////////////////////////////
Fxt::Type::Error ExecutionSet::resolveReferences( Fxt::Point::DatabaseApi & pointDb )  noexcept
{
    if ( m_error == Fxt::Type::Error::SUCCESS() )
    {
        for ( uint16_t i=0; i < m_numLogicChains; i++ )
        {
            if ( m_logicChains[i] == nullptr )
            {
                m_error = fullErr( Err_T::MISSING_LOGIC_CHAINS );
                break;
            }
            if ( m_logicChains[i]->resolveReferences( pointDb ) != Fxt::Type::Error::SUCCESS() )
            {
                m_error = fullErr( Err_T::FAILED_POINT_RESOLVE );
                break;
            }
        }
    }

    return m_error;
}

Fxt::Type::Error ExecutionSet::start( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Do nothing if already started
    if ( !m_started && m_error == Fxt::Type::Error::SUCCESS() )
    {
        // Star the individual components (and additional error checking)
        for ( uint16_t i=0; i < m_numLogicChains; i++ )
        {
            // NOTE: The resolveReferences() method has already validated that
            //       array of components is fully populated with non-null pointers
            if ( m_logicChains[i]->start( currentElapsedTimeUsec ) != Fxt::Type::Error::SUCCESS() )
            {
                m_error = fullErr( Err_T::LOGIC_CHAIN_FAILED_START );
                return m_error;
            }
        }

        m_started = true;
    }

    return m_error;
}

void ExecutionSet::stop() noexcept
{
    // Only stop if I have been started
    if ( m_started )
    {
        // Always stop the components - even if there is an internal error
        for ( uint16_t i=0; i < m_numLogicChains; i++ )
        {
            if ( m_logicChains[i] )
            {
                m_logicChains[i]->stop();
            }
        }

        m_started = false;
    }
}

bool ExecutionSet::isStarted() const noexcept
{
    return m_started;
}

Fxt::Type::Error ExecutionSet::getErrorCode() const noexcept
{
    return m_error;
}

size_t ExecutionSet::getExecutionRateMultiplier() const noexcept
{
    return m_erm;
}


Fxt::Type::Error ExecutionSet::execute( int64_t currentTickUsec ) noexcept
{
    // Only execute if there is no error AND the Execution Set was actually started
    if ( m_error == Fxt::Type::Error::SUCCESS() && m_started )
    {
        // Execute the Logic Chain
        for ( uint16_t i=0; i < m_numLogicChains; i++ )
        {
            if ( m_logicChains[i]->execute( currentTickUsec ) != Fxt::Type::Error::SUCCESS() )
            {
                m_error = fullErr( Err_T::LOGIC_CHAIN_FAILURE );
                break;
            }
        }
    }

    return m_error;
}


//////////////////////////////////////////////////
Fxt::Type::Error ExecutionSet::add( Fxt::LogicChain::Api& logicChainToAdd ) noexcept
{
    if ( m_error == Fxt::Type::Error::SUCCESS() )
    {
        if ( m_nextLogicChainIdx >= m_numLogicChains )
        {
            m_error = fullErr( Err_T::TOO_MANY_LOGIC_CHAINS );
        }
        else
        {
            m_logicChains[m_nextLogicChainIdx] = &logicChainToAdd;
            m_nextLogicChainIdx++;
        }
    }

    return m_error;
}


//////////////////////////////////////////////////
Api* Api::createLogicChainfromJSON( JsonVariant                         executionSetObject,
                                    Fxt::Component::FactoryDatabaseApi& componentFactory,
                                    Cpl::Memory::ContiguousAllocator&   generalAllocator,
                                    Cpl::Memory::ContiguousAllocator&   haStatefulDataAllocator,
                                    Fxt::Point::FactoryDatabaseApi&     pointFactoryDb,
                                    Fxt::Point::DatabaseApi&            dbForPoints,
                                    Fxt::Type::Error&                   executionSetErrorode ) noexcept
{
    // Minimal syntax checking of the JSON input
    if ( executionSetObject["logicChains"].is<JsonArray>() == false )
    {
        executionSetErrorode = fullErr( Err_T::PARSE_LOGIC_CHAIN_ARRAY );
        return nullptr;
    }

    JsonArray logicChains    = executionSetObject["logicChains"];
    size_t    numLogicChains = logicChains.size();
    if ( numLogicChains == 0 )
    {
        executionSetErrorode = fullErr( Err_T::NO_LOGIC_CHAINS );
        return nullptr;
    }

    // Parse Execution Rate Multiplier
    size_t erm = executionSetObject["exeRateMultiplier"] | ((size_t) (-1));
    if ( erm == ((size_t) (-1)) )
    {
        executionSetErrorode = fullErr( Err_T::EXECUTION_SET_MISSING_ERM );
        return nullptr;
    }

    // Create ExecutionSet instance
    void* memExecutionSet = generalAllocator.allocate( sizeof( ExecutionSet ) );
    if ( memExecutionSet == nullptr )
    {
        executionSetErrorode = fullErr( Err_T::NO_MEMORY_EXECUTION_SET );
        return nullptr;
    }
    ExecutionSetApi* executionSet = new(memExecutionSet) ExecutionSet( generalAllocator, (uint16_t) numLogicChains, erm );

    // Create Logic Chains
    for ( uint16_t i=0; i < numLogicChains; i++ )
    {
        Fxt::Type::Error      errorCode      = Fxt::Type::Error::SUCCESS();
        JsonVariant           logicChainJson = logicChains[i];
        Fxt::LogicChain::Api* logicChain     = Fxt::LogicChain::Api::createLogicChainfromJSON( logicChainJson,
                                                                                               componentFactory,
                                                                                               generalAllocator,
                                                                                               haStatefulDataAllocator,
                                                                                               pointFactoryDb,
                                                                                               dbForPoints,
                                                                                               errorCode );
        if ( logicChain == nullptr )
        {
            executionSetErrorode = fullErr( Err_T::FAILED_CREATE_LOGIC_CHAIN );
            return nullptr;
        }
        if ( errorCode != Fxt::Type::Error::SUCCESS() )
        {
            executionSetErrorode = fullErr( Err_T::LOGIC_CHAIN_CREATE_ERROR );
            return nullptr;
        }
        executionSetErrorode = executionSet->add( *logicChain );
        if ( executionSetErrorode != Fxt::Type::Error::SUCCESS() )
        {
            return nullptr;
        }
    }

    // If I get here -->everything worked
    return executionSet;
}

