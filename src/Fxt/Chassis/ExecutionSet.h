#ifndef Fxt_Chassis_ExecutionSet_h_
#define Fxt_Chassis_ExecutionSet_h_
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


#include "Fxt/Chassis/ExecutionSetApi.h"
#include "Fxt/LogicChain/Api.h"

///
namespace Fxt {
///
namespace Chassis {


/** This concrete class implements the ExecutionSet interface
 */
class ExecutionSet : public ExecutionSetApi
{
public:
    /// Constructor
    ExecutionSet( Cpl::Memory::ContiguousAllocator&   generalAllocator,
                  uint16_t                            numLogicChains,
                  size_t                              exeRateMultipler );

    /// Destructor
    ~ExecutionSet();

public:
    /// See Fxt::Chassis::ExecutionSetApi
    Fxt::Type::Error resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept;

    /// See Fxt::Chassis::ExecutionSetApi
    Fxt::Type::Error start( uint64_t currentElapsedTimeUsec ) noexcept;

    /// See Fxt::Chassis::ExecutionSetApi
    void stop() noexcept;

    /// See Fxt::Chassis::ExecutionSetApi
    bool isStarted() const noexcept;

    /// See Fxt::Chassis::ExecutionSetApi
    Fxt::Type::Error getErrorCode() const noexcept;

    /// See Fxt::Chassis::ExecutionSetApi
    Fxt::Type::Error add( Fxt::LogicChain::Api& logicChainToAdd ) noexcept;

    /// Set Fxt::Chassis::ExecutionSetApi
    size_t getExecutionRateMultiplier() const noexcept;

    /// See Fxt::System::PeriodApi
    bool execute( uint64_t currentTick, uint64_t currentInterval ) noexcept;


protected:
    /// Array/List of components in the logic chain
    Fxt::LogicChain::Api**               m_logicChains;

    /// Error state. A value of 0 indicates NO error
    Fxt::Type::Error                    m_error;

    /// The ExecutionSet's Execution Rate Multiplier (ERM)
    size_t                              m_erm;

    /// Number of Logic Chains
    uint16_t                            m_numLogicChains;

    /// Array index for the next Logic Chain add operation
    uint16_t                            m_nextLogicChainIdx;

    /// My started state
    bool                                m_started;
};



};      // end namespaces
};
#endif  // end header latch
