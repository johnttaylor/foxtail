#ifndef Fxt_Card_Common_h_
#define Fxt_Card_Common_h_
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


#include "Fxt/Card/Api.h"
#include "Cpl/Json/Arduino.h"
#include "Fxt/Point/Bank.h"
#include "Cpl/System/Mutex.h"

///
namespace Fxt {
///
namespace Card {


/** This partially concrete class provide common infrastructure for a Card
 */
class Common_ : public Api
{
public:
    /// Constructor
    Common_( const char*            cardName,
             uint16_t               cardLocalId,
             Fxt::Point::BankApi&   internalInputsBank,
             Fxt::Point::BankApi&   registerInputsBank,
             Fxt::Point::BankApi&   virtualInputsBank,
             Fxt::Point::BankApi&   internalOutputsBank,
             Fxt::Point::BankApi&   registerOutputsBank,
             Fxt::Point::BankApi&   virtualOutputsBank );

    /// Destructor
    ~Common_();

public:
    /// See Fxt::Card::Api
    bool start() noexcept;

    /// See Fxt::Card::Api
    bool stop() noexcept;

    /// See Fxt::Card::Api
    bool isStarted() const noexcept;

    /// See Fxt::Card::Api
    uint16_t getLocalId() const noexcept;

    /// See Fxt::Card::Api
    const char* getName() const noexcept;

    /// See Fxt::Card::Api
    bool scanInputs() noexcept;

    /// See Fxt::Card::Api
    bool flushOutputs() noexcept;

protected:
    /// Updates the IO Registers from the Internal Card Inputs
    virtual bool updateInputRegisters() noexcept;

    /// Updates the Internal Card Outputs from the IO Registers
    virtual bool readOutputRegisters() noexcept;

protected:
    /// Mutex for protecting the data when transferring data to/from IO Registers
    Cpl::System::Mutex          m_registerLock;

    /// Mutex for protecting the data when transferring data to/from the card's internal points
    Cpl::System::Mutex          m_internalLock;

    /// Point Bank for the Internal Inputs
    Fxt::Point::BankApi&        m_internalInputsBank;

    /// Point Bank for the IO Register Inputs
    Fxt::Point::BankApi&        m_registerInputsBank;

    /// Point Bank for the Virtual Point Inputs
    Fxt::Point::BankApi&        m_virtualInputsBank;

    /// Point Bank for the Internal outputs
    Fxt::Point::BankApi&        m_internalOutputsBank;
    
    /// Point Bank for the IO Register Outputs
    Fxt::Point::BankApi&        m_registerOutputsBank;
    
    /// Point Bank for the Virtual Point Outputs
    Fxt::Point::BankApi&        m_virtualOutputsBank;

    /// The card's runtime name
    const char*                 m_cardName;

    /// The card's 'User facing local ID'
    uint16_t                    m_localId;

    /// My started state
    bool                        m_started;
};



};      // end namespaces
};
};
};
#endif  // end header latch
