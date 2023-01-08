#ifndef Fxt_Chassis_Chassis_h_
#define Fxt_Chassis_Chassis_h_
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


#include "Fxt/Chassis/Api.h"
#include "Fxt/Chassis/Server.h"

///
namespace Fxt {
///
namespace Chassis {


/** This concrete class implements the ScannerAPI interface
 */
class Chassis : public Api
{
public:
    /// Constructor
    Chassis( ServerApi&                         chassisServer,
             Cpl::Memory::ContiguousAllocator&  generalAllocator, 
             uint64_t                           fer,
             uint16_t                           numScanners,
             uint16_t                           numExecutionSets,
             uint16_t                           numSharedPts );
    
    /// Destructor
    ~Chassis();

public:
    /// See Fxt::Chassis::Api
    Fxt::Type::Error resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept;
    
    /// See Fxt::Chassis::Api
    bool start( uint64_t currentElapsedTimeUsec ) noexcept;
    
    /// See Fxt::Chassis::Api
    void stop() noexcept;
    
    /// See Fxt::Chassis::Api
    bool isStarted() const noexcept;

    /// See Fxt::Chassis::Api
    Fxt::Type::Error add( ScannerApi& componentToAdd ) noexcept;

    /// See Fxt::Chassis::Api
    Fxt::Type::Error add( ExecutionSetApi& autoPointToAdd ) noexcept;

    /// See Fxt::Chassis::Api
    Fxt::Type::Error add( Fxt::Point::Api& sharedPtToAdd ) noexcept;
    
    /// See Fxt::Chassis::Api
    Fxt::Type::Error buildSchedule() noexcept;

    /// See Fxt::Chassis::Api
    Fxt::Type::Error getErrorCode() const noexcept;

    /// See Fxt::Chassis::Api
    uint64_t getFER() const noexcept;

    /// See Fxt::Chassis::Api
    uint16_t getNumScanners() const noexcept;

    /// See Fxt::Chassis::Api
    Fxt::Chassis::ScannerApi* getScanner( uint16_t scannerIndex ) noexcept;

    /// See Fxt::Chassis::Api
    uint16_t getNumExecutionSets() const noexcept;

    /// See Fxt::Chassis::Api
    Fxt::Chassis::ExecutionSetApi* getExecutionSet( uint16_t executionSetIndex ) noexcept;

protected:
    /// Reference/Handle to the Chassis server (aka the runnable-object/thread that executes the chassis)
    ServerApi&                          m_server;

    /// Array/List of Execution Sets
    ExecutionSetApi**                   m_executionSets;

    /// Array/List of Scanners
    ScannerApi**                        m_scanners;

    /// Array/List of Shared points
    Fxt::Point::Api**                   m_sharedPts;

    /// Array/List of Periods for scanning inputs
    Fxt::System::PeriodApi**            m_inputPeriods;

    /// Array/List of Periods for executing logic chains
    Fxt::System::PeriodApi**            m_executionPeriods;

    /// Array/List of Periods for flushing outputs
    Fxt::System::PeriodApi**            m_outputPeriods;

    /// Error state. A value of 0 indicates NO error
    Fxt::Type::Error                    m_error;

    /// The Chassis's fundamental execution rate in microseconds, i.e. the root timing 'tick' for all of the Chassis execution
    uint64_t                            m_fer;

    /// Number of ExecutionSets
    uint16_t                            m_numExecutionSets;

    /// Number of Card Scanners
    uint16_t                            m_numScanners;

    /// Number of Shared Points
    uint16_t                            m_numSharedPts;

    /// Array index for the next Execution add operation
    uint16_t                            m_nextExecutionSetIdx;

    /// Array index for the next card Scanner add operation
    uint16_t                            m_nextScannerIdx;

    /// Array index for the next Shared Pt add operation
    uint16_t                            m_nextSharedPtIdx;

    /// My started state
    bool                                m_started;
};



};      // end namespaces
};
#endif  // end header latch
