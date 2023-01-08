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


#include "Chassis.h"
#include "Error.h"
#include "Cpl/System/Assert.h"
#include "Cpl/Container/DList.h"
#include <new>


///
using namespace Fxt::Chassis;

//////////////////////////////////////////////////
Chassis::Chassis( ServerApi&                         chassisServer,
                  Cpl::Memory::ContiguousAllocator&  generalAllocator,
                  uint64_t                           fer,
                  uint16_t                           numScanners,
                  uint16_t                           numExecutionSets,
                  uint16_t                           numSharedPts )
    : m_server( chassisServer )
    , m_executionSets( nullptr )
    , m_scanners( nullptr )
    , m_sharedPts( nullptr )
    , m_inputPeriods( nullptr )
    , m_executionPeriods( nullptr )
    , m_outputPeriods( nullptr )
    , m_error( Fxt::Type::Error::SUCCESS() )
    , m_fer( fer )
    , m_numExecutionSets( numExecutionSets )
    , m_numScanners( numScanners )
    , m_numSharedPts( numSharedPts )
    , m_nextExecutionSetIdx( 0 )
    , m_nextScannerIdx( 0 )
    , m_nextSharedPtIdx( 0 )
    , m_started( false )
{
    // Allocate my array of scanner pointers
    m_scanners = (ScannerApi**) generalAllocator.allocate( sizeof( ScannerApi* ) * numScanners );
    if ( m_scanners == nullptr )
    {
        m_numScanners = 0;
        m_error       = fullErr( Err_T::NO_MEMORY_SCANNER_LIST );
    }
    else
    {
        // Zero the array so we can tell if there are scanners
        memset( m_scanners, 0, sizeof( ScannerApi* ) * numScanners );
    }

    // Allocate my array of ExecutionSet pointers
    m_executionSets = (ExecutionSetApi**) generalAllocator.allocate( sizeof( ExecutionSetApi* ) * numExecutionSets );
    if ( m_executionSets == nullptr )
    {
        m_numExecutionSets = 0;
        m_error            = fullErr( Err_T::NO_MEMORY_EXESET_LIST );
    }
    else
    {
        // Zero the array so we can tell if there are Execution Sets
        memset( m_executionSets, 0, sizeof( ExecutionSetApi* ) * numExecutionSets );
    }

    // Allocate my array of SharedPoints pointers
    m_sharedPts = (Fxt::Point::Api**) generalAllocator.allocate( sizeof( Fxt::Point::Api* ) * numSharedPts );
    if ( m_sharedPts == nullptr )
    {
        m_numSharedPts = 0;
        m_error        = fullErr( Err_T::NO_MEMORY_SHARED_PTS_LIST );
    }
    else
    {
        // Zero the array so we can tell if there are Shared Pts
        memset( m_sharedPts, 0, sizeof( Fxt::Point::Api* ) * numSharedPts );
    }

    // Allocate Period arrays
    m_inputPeriods = (Fxt::System::PeriodApi**) generalAllocator.allocate( sizeof( Fxt::System::PeriodApi* ) * (numScanners + 1) );
    if ( m_inputPeriods == nullptr )
    {
        m_error = fullErr( Err_T::NO_MEMORY_PERIOD_LISTS );
    }
    else
    {
        // Zero the array (and it also sets the end-of-array marker)
        memset( m_inputPeriods, 0, sizeof( Fxt::System::PeriodApi* ) * (numScanners + 1) );
    }
    m_outputPeriods = (Fxt::System::PeriodApi**) generalAllocator.allocate( sizeof( Fxt::System::PeriodApi* ) * (numScanners + 1) );
    if ( m_outputPeriods == nullptr )
    {
        m_error = fullErr( Err_T::NO_MEMORY_PERIOD_LISTS );
    }
    else
    {
        // Zero the array (and it also sets the end-of-array marker)
        memset( m_outputPeriods, 0, sizeof( Fxt::System::PeriodApi* ) * (numScanners + 1) );
    }
    m_executionPeriods = (Fxt::System::PeriodApi**) generalAllocator.allocate( sizeof( Fxt::System::PeriodApi* ) * (numExecutionSets + 1) );
    if ( m_executionPeriods == nullptr )
    {
        m_error = fullErr( Err_T::NO_MEMORY_PERIOD_LISTS );
    }
    else
    {
        // Zero the array (and it also sets the end-of-array marker)
        memset( m_executionPeriods, 0, sizeof( Fxt::System::PeriodApi* ) * (numExecutionSets + 1) );
    }
}

Chassis::~Chassis()
{
    // Ensure stop is called first
    stop();

    // Call the destructors on all of the Scanners
    for ( uint16_t i=0; i < m_numScanners; i++ )
    {
        if ( m_scanners[i] )
        {
            m_scanners[i]->~ScannerApi();
        }
    }

    // Call the destructors on all of the ExecutionSet
    for ( uint16_t i=0; i < m_numExecutionSets; i++ )
    {
        if ( m_executionSets[i] )
        {
            m_executionSets[i]->~ExecutionSetApi();
        }
    }
}

//////////////////////////////////////////////////
Fxt::Type::Error Chassis::resolveReferences( Fxt::Point::DatabaseApi & pointDb )  noexcept
{
    if ( m_error == Fxt::Type::Error::SUCCESS() )
    {
        for ( uint16_t i=0; i < m_numExecutionSets; i++ )
        {
            if ( m_executionSets[i] == nullptr )
            {
                m_error = fullErr( Err_T::MISSING_EXECUTION_SETS );
                break;
            }
            if ( m_executionSets[i]->resolveReferences( pointDb ) != Fxt::Type::Error::SUCCESS() )
            {
                m_error = fullErr( Err_T::FAILED_POINT_RESOLVE );
                break;
            }
        }
    }

    return m_error;
}

bool Chassis::start( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Do nothing if already started
    if ( !m_started && m_error == Fxt::Type::Error::SUCCESS() )
    {
        // Start the individual Scanners (and additional error checking)
        for ( uint16_t i=0; i < m_numScanners; i++ )
        {
            // Check that all cards got created
            if ( m_scanners[i] == nullptr )
            {
                m_error = fullErr( Err_T::MISSING_SCANNERS );
                return false;
            }

            if ( m_scanners[i]->start( currentElapsedTimeUsec ) == false )
            {
                m_error = fullErr( Err_T::SCANNER_FAILED_START );
                return false;
            }
        }

        // Start the individual ExecutionSets 
        for ( uint16_t i=0; i < m_numExecutionSets; i++ )
        {
            // NOTE: The list executionSets is verified for non-null pointers in resolveReferences()
            if ( m_executionSets[i]->start( currentElapsedTimeUsec ) != Fxt::Type::Error::SUCCESS() )
            {
                m_error = fullErr( Err_T::EXECUTION_SET_FAILED_START );
                return false;
            }
        }

        // Initialize the Shared Points
        for ( uint16_t i=0; i < m_numSharedPts; i++ )
        {
            // Check that all Shared Points got created
            if ( m_sharedPts[i] == nullptr )
            {
                m_error = fullErr( Err_T::MISSING_SHARED_PTS );
                return false;
            }

            m_sharedPts[i]->updateFromSetter();
        }

        // Start the Chassis server
        ChassisPeriods_T periodsInfo ={ m_inputPeriods, m_executionPeriods, m_outputPeriods };
        m_server.open( &periodsInfo );
        m_started = true;
        return true;
    }

    return false;
}

void Chassis::stop() noexcept
{
    // Only stop if I have been started
    if ( m_started )
    {
        // Stop the Chassis server
        m_server.close();

        // Stop the Scanners
        for ( uint16_t i=0; i < m_numScanners; i++ )
        {
            // Check for valid pointer (when stopping when there was a create error)
            if ( m_scanners[i] )
            {
                m_scanners[i]->stop();
            }
        }

        // Stop the ExecutionSets
        for ( uint16_t i=0; i < m_numExecutionSets; i++ )
        {
            // Check for valid pointer (when stopping when there was a create error)
            if ( m_executionSets[i] )
            {
                m_executionSets[i]->stop();
            }
        }

        m_started = false;
    }
}

bool Chassis::isStarted() const noexcept
{
    return m_started;
}

Fxt::Type::Error Chassis::getErrorCode() const noexcept
{
    return m_error;
}

uint64_t Chassis::getFER() const noexcept
{
    return m_fer;
}

uint16_t Chassis::getNumScanners() const noexcept
{
    return m_error == Fxt::Type::Error::SUCCESS() ? m_numScanners : 0;
}

Fxt::Chassis::ScannerApi* Chassis::getScanner( uint16_t scannerIndex ) noexcept
{
    if ( scannerIndex >= m_numScanners || m_error != Fxt::Type::Error::SUCCESS() )
    {
        return nullptr;
    }
    return m_scanners[scannerIndex];
}

uint16_t Chassis::getNumExecutionSets() const noexcept
{
    return m_error == Fxt::Type::Error::SUCCESS() ? m_numExecutionSets : 0;
}

Fxt::Chassis::ExecutionSetApi* Chassis::getExecutionSet( uint16_t executionSetIndex ) noexcept
{
    if ( executionSetIndex >= m_numExecutionSets || m_error != Fxt::Type::Error::SUCCESS() )
    {
        return nullptr;
    }
    return m_executionSets[executionSetIndex];
}

Fxt::Card::Api* Api::getCard( Api& chassis, uint8_t cardSlotNumber ) noexcept
{
    for ( uint16_t scannerIdx = 0; scannerIdx < chassis.getNumScanners(); scannerIdx++ )
    {
        ScannerApi* scanner = chassis.getScanner( scannerIdx );
        if ( scanner == nullptr )
        {
            return nullptr;
        }

        for ( uint16_t cardIdx = 0; cardIdx < scanner->getNumCards(); cardIdx++ )
        {
            Fxt::Card::Api* card = scanner->getCard( cardIdx );
            if ( card == nullptr )
            {
                return nullptr;
            }
            if ( card->getSlotNumber() == cardSlotNumber )
            {
                return card;
            }
        }
    }

    return nullptr;
}


//////////////////////////////////////////////////
Fxt::Type::Error Chassis::add( ScannerApi& scannerToAdd ) noexcept
{
    if ( m_error == Fxt::Type::Error::SUCCESS() )
    {
        if ( m_nextScannerIdx >= m_numScanners )
        {
            m_error = fullErr( Err_T::TOO_MANY_SCANNERS );
        }
        else
        {
            m_scanners[m_nextScannerIdx] = &scannerToAdd;
            m_nextScannerIdx++;
        }
    }

    return m_error;
}

Fxt::Type::Error Chassis::add( ExecutionSetApi& exeSetToAdd ) noexcept
{
    if ( m_error == Fxt::Type::Error::SUCCESS() )
    {
        if ( m_nextExecutionSetIdx >= m_numExecutionSets )
        {
            m_error = fullErr( Err_T::TOO_MANY_SCANNERS );
        }
        else
        {
            m_executionSets[m_nextExecutionSetIdx] = &exeSetToAdd;
            m_nextExecutionSetIdx++;
        }
    }

    return m_error;
}

Fxt::Type::Error Chassis::add( Fxt::Point::Api& sharedPtToAdd ) noexcept
{
    if ( m_error == Fxt::Type::Error::SUCCESS() )
    {
        if ( m_nextSharedPtIdx >= m_numSharedPts )
        {
            m_error = fullErr( Err_T::TOO_MANY_SHARED_PTS );
        }
        else
        {
            m_sharedPts[m_nextSharedPtIdx] = &sharedPtToAdd;
            m_nextSharedPtIdx++;
        }
    }

    return m_error;
}

Fxt::Type::Error Chassis::buildSchedule() noexcept
{
    // Sort Scanners by their's SRM (lowest multiplier first)
    Cpl::Container::DList<ScannerApi> sortedScannerList;
    for ( uint16_t i=0; i < m_numScanners; i++ )
    {
        ScannerApi* elemToAdd = m_scanners[i];
        if ( elemToAdd == nullptr )
        {
            return fullErr( Err_T::FAILED_SCANNER_SCHEDULE_BUILD );
        }

        // Insert into the list by lowest SRM order
        // Note: The list is constructed such that if the Scanners where 
        //       ordered (lowest to highest) then the sort time is O(N)
        bool        added   = false;
        ScannerApi* curElem = sortedScannerList.last();
        while ( curElem )
        {
            if ( elemToAdd->getScanRateMultiplier() >= curElem->getScanRateMultiplier() )
            {
                sortedScannerList.insertAfter( *curElem, *elemToAdd );
                added = true;
                break;
            }
            curElem = sortedScannerList.previous( *curElem );
        }

        // If the element was NOT added -->add it now to the start of the list
        if ( !added )
        {
            sortedScannerList.putFirst( *elemToAdd );
        }
    }

    // Convert the sorted scanner list to a Array of Periods AND set the period interval times
    ScannerApi* curScannerElem = sortedScannerList.first();
    for ( uint16_t i=0; i < m_numScanners; i++ )
    {
        if ( curScannerElem == nullptr )
        {
            return fullErr( Err_T::FAILED_SCANNER_SCHEDULE_BUILD );
        }

        m_inputPeriods[i]              = &(curScannerElem->getInputPeriod());
        m_inputPeriods[i]->m_duration  = curScannerElem->getScanRateMultiplier() * m_fer;
        m_outputPeriods[i]             = &(curScannerElem->getOutputPeriod());
        m_outputPeriods[i]->m_duration = curScannerElem->getScanRateMultiplier() * m_fer;
        curScannerElem                 = sortedScannerList.next( *curScannerElem );
    }

    // Sort ExecutionSets by their's ERM (lowest multiplier first)
    Cpl::Container::DList<ExecutionSetApi> sortedExecutionSetList;
    for ( uint16_t i=0; i < m_numExecutionSets; i++ )
    {
        ExecutionSetApi* elemToAdd = m_executionSets[i];
        if ( elemToAdd == nullptr )
        {
            return fullErr( Err_T::FAILED_EXESET_SCHEDULE_BUILD );
        }

        // Insert into the list by lowest ERM order
        // Note: The list is constructed such that if the Execution Sets where 
        //       ordered (lowest to highest) then the sort time is O(N)
        bool             added   = false;
        ExecutionSetApi* curElem = sortedExecutionSetList.last();
        while ( curElem )
        {
            if ( elemToAdd->getExecutionRateMultiplier() >= curElem->getExecutionRateMultiplier() )
            {
                sortedExecutionSetList.insertAfter( *curElem, *elemToAdd );
                added = true;
                break;
            }
            curElem = sortedExecutionSetList.previous( *curElem );
        }

        // If the element was NOT added -->add it now to the start of the list
        if ( !added )
        {
            sortedExecutionSetList.putFirst( *elemToAdd );
        }
    }

    // Convert the sorted Execution Set list to a Array of Periods AND set the period interval times
    ExecutionSetApi* curExeSetElem = sortedExecutionSetList.first();
    for ( uint16_t i=0; i < m_numExecutionSets; i++ )
    {
        if ( curExeSetElem == nullptr )
        {
            return fullErr( Err_T::FAILED_EXESET_SCHEDULE_BUILD );
        }

        m_executionPeriods[i]             = curExeSetElem;
        m_executionPeriods[i]->m_duration = curExeSetElem->getExecutionRateMultiplier() * m_fer;
        curExeSetElem                     = sortedExecutionSetList.next( *curExeSetElem );
    }

    return Fxt::Type::Error::SUCCESS();
}

//////////////////////////////////////////////////
Api* Api::createChassisfromJSON( JsonVariant                         chassisJsonObject,
                                 ServerApi&                          chassisServer,
                                 Fxt::Component::FactoryDatabaseApi& componentFactory,
                                 Fxt::Card::FactoryDatabaseApi&      cardFactoryDb,
                                 Cpl::Memory::ContiguousAllocator&   generalAllocator,
                                 Cpl::Memory::ContiguousAllocator&   cardStatefulDataAllocator,
                                 Cpl::Memory::ContiguousAllocator&   haStatefulDataAllocator,
                                 Fxt::Point::FactoryDatabaseApi&     pointFactoryDb,
                                 Fxt::Point::DatabaseApi&            dbForPoints,
                                 Fxt::Type::Error&                   chassisErrorode ) noexcept
{
    // Minimal syntax checking of the JSON input
    if ( chassisJsonObject["scanners"].is<JsonArray>() == false )
    {
        chassisErrorode = fullErr( Err_T::PARSE_SCANNER_ARRAY );
        return nullptr;
    }
    if ( chassisJsonObject["executionSets"].is<JsonArray>() == false )
    {
        chassisErrorode = fullErr( Err_T::PARSE_EXECUTION_SET_ARRAY );
        return nullptr;
    }

    // Get Scanner Array
    JsonArray scanners    = chassisJsonObject["scanners"];
    size_t    numScanners = scanners.size();
    if ( numScanners == 0 )
    {
        chassisErrorode = fullErr( Err_T::PARSE_SCANNER_ARRAY );
        return nullptr;
    }

    // Get ExecutionSet Array
    JsonArray executionSets    = chassisJsonObject["executionSets"];
    size_t    numExecutionSets = executionSets.size();
    if ( numExecutionSets == 0 )
    {
        chassisErrorode = fullErr( Err_T::PARSE_EXECUTION_SET_ARRAY );
        return nullptr;
    }

    // Get Shared Points Array
    JsonArray sharedPtsObj = chassisJsonObject["sharedPts"];
    size_t    numSharedPts = sharedPtsObj.size();


    // Parse Fundamental Execution Rate
    size_t fer = chassisJsonObject["fer"] | ((size_t) (-1));
    if ( fer == ((size_t) (-1)) )
    {
        chassisErrorode = fullErr( Err_T::SCANNER_MISSING_SRM );
        return nullptr;
    }

    // Create Chassis instance
    void* memChassis = generalAllocator.allocate( sizeof( Chassis ) );
    if ( memChassis == nullptr )
    {
        chassisErrorode = fullErr( Err_T::NO_MEMORY_CHASSIS );
        return nullptr;
    }
    Api* chassis = new(memChassis) Chassis( chassisServer, generalAllocator, fer, (uint16_t) numScanners, (uint16_t) numExecutionSets, (uint16_t) numSharedPts );

    // Create Scanners
    for ( uint16_t i=0; i < numScanners; i++ )
    {
        Fxt::Type::Error  errorCode  = Fxt::Type::Error::SUCCESS();
        JsonObject        scannerObj = scanners[i];
        ScannerApi*       scanner     = ScannerApi::createScannerfromJSON( scannerObj,
                                                                           generalAllocator,
                                                                           cardStatefulDataAllocator,
                                                                           haStatefulDataAllocator,
                                                                           cardFactoryDb,
                                                                           pointFactoryDb,
                                                                           dbForPoints,
                                                                           errorCode );
        if ( scanner == nullptr )
        {
            chassisErrorode = fullErr( Err_T::FAILED_CREATE_SCANNER );
            chassis->~Api();
            return nullptr;
        }
        if ( errorCode != Fxt::Type::Error::SUCCESS() )
        {
            chassisErrorode = fullErr( Err_T::SCANNER_CREATE_ERROR );
            chassis->~Api();
            return nullptr;
        }
        chassisErrorode = chassis->add( *scanner );
        if ( chassisErrorode != Fxt::Type::Error::SUCCESS() )
        {
            chassis->~Api();
            return nullptr;
        }
    }

    // Create ExecutionSets
    for ( uint16_t i=0; i < numExecutionSets; i++ )
    {
        Fxt::Type::Error  errorCode  = Fxt::Type::Error::SUCCESS();
        JsonObject        exeSetObj  = executionSets[i];
        ExecutionSetApi*  exeSet     = ExecutionSetApi::createExecutionSetfromJSON( exeSetObj,
                                                                                    componentFactory,
                                                                                    generalAllocator,
                                                                                    haStatefulDataAllocator,
                                                                                    pointFactoryDb,
                                                                                    dbForPoints,
                                                                                    errorCode );
        if ( exeSet == nullptr )
        {
            chassisErrorode = fullErr( Err_T::FAILED_CREATE_EXESET );
            chassis->~Api();
            return nullptr;
        }
        if ( errorCode != Fxt::Type::Error::SUCCESS() )
        {
            chassisErrorode = fullErr( Err_T::EXESET_CREATE_ERROR );
            chassis->~Api();
            return nullptr;
        }
        chassisErrorode = chassis->add( *exeSet );
        if ( chassisErrorode != Fxt::Type::Error::SUCCESS() )
        {
            chassis->~Api();
            return nullptr;
        }
    }

    // Create Shared Points
    for ( size_t i=0; i < sharedPtsObj.size(); i++ )
    {
        Fxt::Type::Error pointError;
        JsonObject       pointJson = sharedPtsObj[i];
        Fxt::Point::Api* pt = pointFactoryDb.createPointfromJSON( pointJson,
                                                                  pointError,
                                                                  generalAllocator,
                                                                  haStatefulDataAllocator,     // Note: Share points ARE part of the HA data
                                                                  dbForPoints,
                                                                  "id",
                                                                  true );
        if ( pt == nullptr )
        {
            chassisErrorode = fullErr( Err_T::FAILED_CREATE_SHARED_POINTS );
            chassis->~Api();
            return nullptr;
        }
        chassisErrorode = chassis->add( *pt );
        if ( chassisErrorode != Fxt::Type::Error::SUCCESS() )
        {
            chassis->~Api();
            return nullptr;
        }
    }

    // Build the Chassis's execution schedule
    chassisErrorode = chassis->buildSchedule();
    if ( chassisErrorode != Fxt::Type::Error::SUCCESS() )
    {
        chassis->~Api();
        return nullptr;
    }

    // If I get here -->everything worked
    return chassis;
}

