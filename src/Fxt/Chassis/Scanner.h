#ifndef Fxt_LogicChain_Chain_h_
#define Fxt_LogicChain_Chain_h_
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


#include "Fxt/Chassis/ScannerApi.h"
#include "Fxt/Card/Api.h"

///
namespace Fxt {
///
namespace Chassis {


/** This concrete class implements the ScannerAPI interface
 */
class Scanner : public ScannerApi
{
protected:
    /// Period for Scanning inputs
    class Inputs : public Fxt::System::PeriodApi
    {
    public:
        /// Constructor
        Inputs( Scanner& context): m_scanner(context) {}

    public:
        /// Execute function
        bool execute( uint64_t currentTick, uint64_t currentInterval ) noexcept
        {
            return m_scanner.scanInputs( currentInterval );
        }

    protected:
        /// The scanner instance that we are scanning for
        Scanner& m_scanner;
    };

    /// Period for flushing outputs
    class Outputs : public Fxt::System::PeriodApi
    {
    public:
        /// Constructor
        Outputs( Scanner& context ) : m_scanner( context ) {}

    public:
        /// Execute function
        bool execute( uint64_t currentTick, uint64_t currentInterval ) noexcept
        {
            return m_scanner.flushOutputs( currentInterval );
        }

    protected:
        /// The scanner instance that we are scanning for
        Scanner& m_scanner;
    };


public:
    /// Constructor
    Scanner( Cpl::Memory::ContiguousAllocator&   generalAllocator,
             uint16_t                            numCards,
             size_t                              scanRateMultipler );

    /// Destructor
    ~Scanner();

public:
    /// Set Fxt::Chassis::ScannerApi
    bool start( uint64_t currentElapsedTimeUsec ) noexcept;

    /// Set Fxt::Chassis::ScannerApi
    void stop() noexcept;

    /// Set Fxt::Chassis::ScannerApi
    bool isStarted() const noexcept;

    /// Set Fxt::Chassis::ScannerApi
    Fxt::System::PeriodApi& getInputPeriod() noexcept;

    /// Set Fxt::Chassis::ScannerApi
    Fxt::System::PeriodApi& getOutputPeriod() noexcept;

    /// Set Fxt::Chassis::ScannerApi
    Fxt::Type::Error getErrorCode() const noexcept;

    /// Set Fxt::Chassis::ScannerApi
    Fxt::Type::Error add( Fxt::Card::Api& cardToAdd ) noexcept;

    /// Set Fxt::Chassis::ScannerApi
    size_t getScanRateMultiplier() const noexcept;

    /// Set Fxt::Chassis::ScannerApi
    uint16_t getNumCards() const noexcept;
    
    /// Set Fxt::Chassis::ScannerApi
    Fxt::Card::Api* getCard( uint16_t cardIndex ) noexcept;

protected:
    /** This method invokes the scanInputs() method on all contained
        IO cards.

        The method return false if unrecoverable error occurred; else true
        is returned.
     */
    bool scanInputs( uint64_t currentElapsedTimeUsec ) noexcept;

    /** This method invokes the flushOutputs() method on all contained
        IO Cards.

        The method return false if unrecoverable error occurred; else true
        is returned.
     */
    bool flushOutputs( uint64_t currentElapsedTimeUsec ) noexcept;

protected:
    /// Input Period instance
    Inputs              m_inputPeriod;

    /// Output Period instance
    Outputs             m_outputPeriod;

    /// Array/List of IO Cards
    Fxt::Card::Api**    m_cards;

    /// Error state. A value of 0 indicates NO error
    Fxt::Type::Error    m_error;

    /// The Scanner's Scan Rate Multiplier (SRM)
    size_t              m_srm;

    /// Number of Cards
    uint16_t            m_numCards;

    /// Array index for the next Card add operation
    uint16_t            m_nextCardIdx;

    /// My started state
    bool                m_started;
};



};      // end namespaces
};
#endif  // end header latch
