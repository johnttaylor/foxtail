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
#include "Common_.h"
#include "Error.h"
#include "FactoryApi.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Api.h"
#include <new>

#ifndef OPTION_FXT_NODE_WAIT_THREAD_MAX_LOOP_ITERATIONS
#define OPTION_FXT_NODE_WAIT_THREAD_MAX_LOOP_ITERATIONS 100
#endif
#ifndef OPTION_FXT_NODE_WAIT_THREAD_WAIT_TIME_MS
#define OPTION_FXT_NODE_WAIT_THREAD_WAIT_TIME_MS        1
#endif

// Note: The LeanHeap expect memory to aligned to size_t -->hence the extra macros to help with the allocation/sizing
#define BYTES_AS_SIZET(n)       (((n)+sizeof(size_t)-1)/ sizeof(size_t))
#define SIZET_TO_BYTES(m)       (m*sizeof(size_t))

///
using namespace Fxt::Node;


//////////////////////////////////////////////////
Common_::Common_( uint8_t                  numChassis,
                  Fxt::Point::DatabaseApi& pointDb,
                  size_t                   sizeGeneralHeap,
                  size_t                   sizeCardStatefulHeap,
                  size_t                   sizeHaStatefulHeap )
    : m_generalAllocator( new size_t[BYTES_AS_SIZET( sizeGeneralHeap )], SIZET_TO_BYTES( BYTES_AS_SIZET( sizeGeneralHeap ) ) )
    , m_cardStatefulAllocator( new size_t[BYTES_AS_SIZET( sizeCardStatefulHeap )], SIZET_TO_BYTES( BYTES_AS_SIZET( sizeCardStatefulHeap ) ) )
    , m_haStatefulAllocator( new size_t[BYTES_AS_SIZET( sizeHaStatefulHeap )], SIZET_TO_BYTES( BYTES_AS_SIZET( sizeHaStatefulHeap ) ) )
    , m_pointDb( pointDb )
    , m_chassis( nullptr )
    , m_error( Fxt::Type::Error::SUCCESS() )
    , m_numChassis( numChassis )
    , m_nextChassisIdx( 0 )
    , m_started( false )
    , m_pointReferencesResolved( false )
{
    // Clear the Point database
    m_pointDb.clearPoints();

    // Check if heap allocations worked
    size_t dummy;
    if ( m_generalAllocator.getMemoryStart( dummy ) == nullptr ||
         (sizeCardStatefulHeap > 0 && m_cardStatefulAllocator.getMemoryStart( dummy ) == nullptr) ||
         m_haStatefulAllocator.getMemoryStart( dummy ) == nullptr )
    {
        m_error      = fullErr( Err_T::FAILED_HEAP_ALLOCATIONS );
        m_error.logIt();
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
            m_error.logIt();
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
    // The application is REQUIRED to stop the node before deleting it.
    if ( isStarted() )
    {
        Cpl::System::FatalError::logf( "Fxt::Node instance is being deleted when the node is NOT in the stopped state" );
    }


    // Call the destructors on all of the Chassis
    for ( uint8_t i=0; i < m_numChassis; i++ )
    {
        // Destroy the Chassis's thread instance
        if ( m_chassis[i].thread )
        {
            destroyChassisThread( *m_chassis[i].thread );
        }

        // Destroy the Chassis instance
        if ( m_chassis[i].chassis )
        {
            m_chassis[i].chassis->~Api();
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
        // Resolve Point references (but only once)
        if ( !m_pointReferencesResolved )
        {
            m_pointReferencesResolved = true;

            for ( uint8_t i=0; i < m_numChassis; i++ )
            {
                // Check that all cards got created
                if ( m_chassis[i].chassis == nullptr )
                {
                    m_error = fullErr( Err_T::MISSING_CHASSIS );
                    m_error.logIt();
                    return false;
                }

                // Resolve point references
                m_error = m_chassis[i].chassis->resolveReferences( m_pointDb );
                if ( m_error != Fxt::Type::Error::SUCCESS() )
                {
                    return false;
                }
            }
        }

        // Start the individual chassis (and additional error checking)
        for ( uint8_t i=0; i < m_numChassis; i++ )
        {
            // NOTE: Execution of the chassis will begin here
            if ( m_chassis[i].chassis->start( currentElapsedTimeUsec ) == false )
            {
                m_error = fullErr( Err_T::CHASSIS_FAILED_START );
                m_error.logIt();
                return false;
            }
        }

        m_started = true;
        return true;
    }

    return false;
}

void Common_::stop() noexcept
{
    // Only stop if I have been started
    if ( m_started )
    {
        for ( uint8_t i=0; i < m_numChassis; i++ )
        {
            // Stop chassis instances
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
    if ( m_started )
    {
        return true;
    }

    // When stopped, report my started state as a function of my contained
    // chassis.  This is needed because a chassis can contain cards that have 
    // asynchronous start/stop implementations (and the stop scenario must be 
    // handled differently, i.e need to wait to till the cards have actually 
    // stopped).
    bool started = false;
    for ( uint16_t i=0; i < m_numChassis; i++ )
    {
        if ( m_chassis[i].chassis )
        {
            started |= m_chassis[i].chassis->isStarted();
        }
    }

    return started;
}

Fxt::Type::Error Common_::getErrorCode() const noexcept
{
    return m_error;
}

void Common_::destroyChassisThread( Cpl::System::Thread & chassisThreadToDelete ) noexcept
{
    Cpl::System::Thread::destroy( chassisThreadToDelete );
}

bool Common_::waitForThreadToRun( Cpl::System::Runnable & runnable )
{
    for ( uint8_t i=0; i < OPTION_FXT_NODE_WAIT_THREAD_MAX_LOOP_ITERATIONS; i++ )
    {
        Cpl::System::Api::sleep( OPTION_FXT_NODE_WAIT_THREAD_WAIT_TIME_MS );
        if ( runnable.isRunning() )
        {
            return true;
        }
    }
    return false;
}

uint16_t Common_::getNumChassis() const noexcept
{
    return m_error == Fxt::Type::Error::SUCCESS()? m_numChassis: 0;
}

/// See Fxt::Node::Api
Fxt::Chassis::Api* Common_::getChassis( uint16_t chassisIndex ) noexcept
{
    if ( chassisIndex >= m_numChassis || m_error != Fxt::Type::Error::SUCCESS() )
    {
        return nullptr;
    }
    return m_chassis[chassisIndex].chassis;
}




//////////////////////////////////////////////////
Fxt::Type::Error Common_::add( Fxt::Chassis::Api&       chassisToAdd,
                               Cpl::System::Thread&     chassisThreadToAdd ) noexcept
{
    if ( m_error == Fxt::Type::Error::SUCCESS() )
    {
        if ( m_nextChassisIdx >= m_numChassis )
        {
            m_error = fullErr( Err_T::TOO_MANY_CHASSIS );
            m_error.logIt();
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


