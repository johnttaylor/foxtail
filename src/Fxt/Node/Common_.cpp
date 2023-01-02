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


#include "Common_.h"
#include "Error.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Api.h"
#include <new>

#define BYTES_AS_SIZET(n)       ((n+sizeof(size_t)-1)/ sizeof(size_t))

///
using namespace Fxt::Node;

//////////////////////////////////////////////////
Common_::Common_( uint8_t numChassis,
                  size_t  sizeGeneralHeap,
                  size_t  sizeCardStatefulHeap,
                  size_t  sizeHaStatefulHeap )
    : m_generalAllocator( new size_t[BYTES_AS_SIZET( sizeGeneralHeap )], BYTES_AS_SIZET( sizeGeneralHeap ) )
    , m_cardStatefulAllocator( new size_t[BYTES_AS_SIZET( sizeCardStatefulHeap )], BYTES_AS_SIZET( sizeCardStatefulHeap ) )
    , m_haStatefulAllocator( new size_t[BYTES_AS_SIZET( sizeHaStatefulHeap )], BYTES_AS_SIZET( sizeHaStatefulHeap ) )
    , m_chassis( nullptr )
    , m_error( Fxt::Type::Error::SUCCESS() )
    , m_numChassis( numChassis )
    , m_nextChassisIdx( 0 )
    , m_started( false )
{
    // Check if heap allocations worked
    size_t dummy;
    if ( m_generalAllocator.getMemoryStart( dummy ) == nullptr ||
         (sizeCardStatefulHeap > 0 && m_cardStatefulAllocator.getMemoryStart( dummy ) == nullptr) ||
         m_haStatefulAllocator.getMemoryStart( dummy ) == nullptr )
    {
        m_error      = fullErr( Err_T::FAILED_HEAP_ALLOCATIONS );
        m_numChassis = 0;
    }

    // Heaps are valid -->continue construction...
    else
    {
        // Allocate my array of Chassis
        m_chassis = (Chassis_T*) m_generalAllocator.allocate( sizeof( Chassis_T ) * numChassis );
        if ( m_chassis == nullptr )
        {
            m_numChassis = 0;
            m_error      = fullErr( Err_T::NO_MEMORY_CHASSIS_LIST );
        }
        else
        {
            // Zero the array so we can tell if there are chassis
            memset( m_chassis, 0, sizeof( Chassis_T ) * numChassis );
        }
    }
}

Common_::~Common_()
{
    // Ensure stop is called first
    stop();

    // Call the destructors on all of the Chassis
    for ( uint8_t i=0; i < m_numChassis; i++ )
    {
        // Destroy the Chassis instance
        if ( m_chassis[i].chassis )
        {
            m_chassis[i].chassis->~Api();
        }

        // Destroy the Chassis's thread (and runnable object) instances
        if ( m_chassis[i].thread )
        {
            destroyChassisThread( *m_chassis[i].thread );
        }
    }

    // Free my local heaps
    size_t dummy;
    delete[] m_generalAllocator.getMemoryStart( dummy );
    delete[] m_cardStatefulAllocator.getMemoryStart( dummy );
    delete[] m_haStatefulAllocator.getMemoryStart( dummy );
}

//////////////////////////////////////////////////
Cpl::Memory::ContiguousAllocator& Common_::getGeneralAlloactor() noexcept
{
    return m_generalAllocator;
}

Cpl::Memory::ContiguousAllocator& Common_::getCardStatefulAlloactor() noexcept
{
    return m_cardStatefulAllocator;
}

Cpl::Memory::ContiguousAllocator& Common_::getHaStatefulAlloactor() noexcept
{
    return m_haStatefulAllocator;
}

//////////////////////////////////////////////////
bool Common_::start( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Do nothing if already started
    if ( !m_started && m_error == Fxt::Type::Error::SUCCESS() )
    {
        // Start the individual Scanners (and additional error checking)
        for ( uint8_t i=0; i < m_numChassis; i++ )
        {
            // Check that all cards got created
            if ( m_chassis[i].chassis == nullptr )
            {
                m_error = fullErr( Err_T::MISSING_CHASSIS );
                return false;
            }

            if ( m_chassis[i].chassis->start( currentElapsedTimeUsec ) == false )
            {
                m_error = fullErr( Err_T::CHASSIS_FAILED_START );
                return false;
            }
        }
    }

    return true;
}

void Common_::stop() noexcept
{
    // Only stop if I have been started
    if ( m_started )
    {
        // Stop the Chassis
        for ( uint8_t i=0; i < m_numChassis; i++ )
        {
            // Check for valid pointer (when stopping when there was a create error)
            if ( m_chassis[i].chassis )
            {
                m_chassis[i].chassis->stop();
            }
        }

        m_started = false;
    }
}

bool Common_::isStarted() const noexcept
{
    return m_started;
}

Fxt::Type::Error Common_::getErrorCode() const noexcept
{
    return m_error;
}

void Common_::destroyChassisThread( Cpl::System::Thread& chassisThreadToDelete ) noexcept
{
    Cpl::System::Thread::destroy( chassisThreadToDelete );
}

//////////////////////////////////////////////////
Fxt::Type::Error Common_::add( Fxt::Chassis::Api&     chassisToAdd,
                               Cpl::System::Thread&   chassisThreadToAdd ) noexcept
{
    if ( m_error == Fxt::Type::Error::SUCCESS() )
    {
        if ( m_nextChassisIdx >= m_numChassis )
        {
            m_error = fullErr( Err_T::TOO_MANY_CHASSIS );
        }
        else
        {
            m_chassis[m_nextChassisIdx].chassis = &chassisToAdd;
            m_chassis[m_nextChassisIdx].thread  = &chassisThreadToAdd;
            m_nextChassisIdx++;
        }
    }

    return m_error;
}



//////////////////////////////////////////////////
Api* Api::createNodefromJSON( FactoryApi&              nodeFactory,
                              JsonVariant              nodeJsonObject,
                              Fxt::Point::DatabaseApi& dbForPoints,
                              Fxt::Type::Error&        nodeErrorCode ) noexcept
{
    // Get the chassis Array
    JsonArray chassisArray = nodeJsonObject["chassis"];
    size_t    numChassis   = chassisArray.size();
    if ( numChassis == 0 )
    {
        nodeErrorCode = fullErr( Err_T::PARSE_CHASSIS_ARRAY );
        return nullptr;
    }
    if ( numChassis > nodeFactory.getMaxAllowedChassis() )
    {
        nodeErrorCode = fullErr( Err_T::MAX_CHASSIS_EXCEEDED );
        return nullptr;
    }

    // Validate the Node type
    const char* typeGuid = nodeJsonObject["type"];
    if ( typeGuid == nullptr || strcmp( typeGuid, nodeFactory.getGuid() ) != 0 )
    {
        nodeErrorCode = fullErr( Err_T::NOT_ME );
        return nullptr;
    }

    // Create Node instance
    Api* node = nodeFactory.create( nodeJsonObject, dbForPoints, nodeErrorCode );
    if ( node == nullptr )
    {
        nodeErrorCode = fullErr( Err_T::NO_MEMORY_NODE );
        return nullptr;
    }
    if ( node->getErrorCode() != Fxt::Type::Error::SUCCESS() )
    {
        nodeErrorCode = fullErr( Err_T::NODE_CREATE_ERROR );
        nodeFactory.destroy( *node );
        return nullptr;
    }

    // Create Chassis
    for ( uint8_t i=0; i < numChassis; i++ )
    {
        Fxt::Type::Error   errorCode  = Fxt::Type::Error::SUCCESS();

        // Create the Chassis thread (and runnable obj) instance
        Fxt::Chassis::ServerApi* serverApi;
        Cpl::System::Thread*     chassisThread = node->createChassisThread( serverApi );
        if ( chassisThread == nullptr || serverApi == nullptr )
        {
            nodeErrorCode = fullErr( Err_T::FAILED_CREATE_CHASSIS_SERVER );
            nodeFactory.destroy( *node );
            return nullptr;
        }

        // Create the Chassis
        Fxt::Type::Error   errorCode;
        JsonObject         chassisObj = chassisArray[i];
        Fxt::Chassis::Api* chassisPtr = Fxt::Chassis::Api::createChassisfromJSON( chassisObj,
                                                                                  *serverApi,
                                                                                  nodeFactory.getComponentFactoryDb(),
                                                                                  nodeFactory.getCardFactoryDb(),
                                                                                  node->getGeneralAlloactor(),
                                                                                  node->getCardStatefulAlloactor(),
                                                                                  node->getHaStatefulAlloactor(),
                                                                                  nodeFactory.getPointFactoryDb(),
                                                                                  dbForPoints,
                                                                                  errorCode );
        if ( chassisPtr == nullptr )
        {
            nodeErrorCode = fullErr( Err_T::FAILED_CREATE_CHASSIS );
            nodeFactory.destroy( *node );
            return nullptr;
        }
        if ( errorCode != Fxt::Type::Error::SUCCESS() )
        {
            nodeErrorCode = fullErr( Err_T::CHASSIS_CREATE_ERROR );
            nodeFactory.destroy( *node );
            return nullptr;
        }
        nodeErrorCode = node->add( *chassisPtr, *chassisThread );
        if ( nodeErrorCode != Fxt::Type::Error::SUCCESS() )
        {
            nodeFactory.destroy( *node );
            return nullptr;
        }
    }


    // If I get here -->everything worked
    return node;
}
