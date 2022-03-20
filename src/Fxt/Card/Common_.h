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
#include "Fxt/Card/Banks.h"
#include "Cpl/Json/Arduino.h"
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

public:
    /// Constructor
    Common_( Cpl::Memory::ContiguousAllocator& allocator );

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
    uint32_t getLocalId() const noexcept;

    /// See Fxt::Card::Api
    const char* getName() const noexcept;

    /// See Fxt::Card::Api
    bool scanInputs() noexcept;

    /// See Fxt::Card::Api
    bool flushOutputs() noexcept;

    /// See Fxt::Card::Api
    uint32_t getErrorCode() const noexcept;

    /// See Fxt::Card::Api
    uint16_t getSlot() const noexcept;

protected:
    /// Updates the IO Registers from the Internal Card Inputs
    virtual bool updateInputRegisters() noexcept;

    /// Updates the Internal Card Outputs from the IO Registers
    virtual bool readOutputRegisters() noexcept;

    /// Parses/extracts the Card's slot number and name from the JSON config
    virtual bool parseCommon( JsonVariant& jsonConfigObject, const char* expectedGuid ) noexcept;

protected:
    /// Allocator for all thing dynamic - except for Points
    Cpl::Memory::ContiguousAllocator&   m_allocator;

    /// Mutex for protecting the data when transferring data to/from the card's internal points
    Cpl::System::Mutex                  m_internalLock;

    /// Mutex for protecting the data when transferring data to/from IO Registers
    Cpl::System::Mutex                  m_registerLock;

    /// Point Banks
    Banks_T                             m_banks;

    /// Error state. A value of 0 indicates NO error
    uint32_t                            m_error;

    /// The card's runtime name
    char*                               m_cardName;

    /// The card's 'User facing local ID'
    uint32_t                            m_localId;

    /// The card's slot number/identifier
    uint16_t                            m_slotNumber;

    /// My started state
    bool                                m_started;
};



};      // end namespaces
};
#endif  // end header latch
