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
#include "Fxt/Card/Error.h"
#include "Fxt/Point/Bank.h"
#include "Fxt/Point/DatabaseApi.h"
#include "Fxt/Point/FactoryDatabaseApi.h"
#include "Cpl/Json/Arduino.h"
#include "Cpl/Memory/ContiguousAllocator.h"

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
    Common_( uint16_t                           totalNumChannels,
             Cpl::Memory::ContiguousAllocator&  generalAllocator,
             JsonVariant&                       cardObject );

    /// Destructor
    ~Common_();

public:
    /// See Fxt::Card::Api
    bool start( uint64_t currentElapsedTimeUsec ) noexcept;

    /// See Fxt::Card::Api
    void stop() noexcept;

    /// See Fxt::Card::Api
    uint8_t getSlotNumber() const noexcept;

    /// See Fxt::Card::Api
    bool isStarted() const noexcept;

    /// See Fxt::Card::Api
    bool scanInputs() noexcept;

    /// See Fxt::Card::Api
    bool flushOutputs() noexcept;

    /// See Fxt::Card::Api
    Fxt::Type::Error getErrorCode() const noexcept;

protected:
    /** Helper method to create the points for a single channel.  Returns a pointer
        to the created 'IO Register' Point if all points were successfully created;
        else nullptr is returned.
     */
    Fxt::Point::Api* createPointForChannel( Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                                            Fxt::Point::Bank&                  pointBank,
                                            bool                               isIoRegPt,
                                            JsonObject&                        channelObject,
                                            Fxt::Type::Error&                  cardErrorCode,
                                            uint16_t                           maxChannels,
                                            uint16_t                           channelIndexOffset,
                                            uint16_t&                          channelNum,
                                            Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                            Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                                            Fxt::Point::DatabaseApi&           dbForPoints ) noexcept;

protected:
    /// Bank for the Card's Input IO Register Points
    Fxt::Point::Bank                    m_ioRegisterInputs;

    /// Bank for the Card's Input Virtual Points
    Fxt::Point::Bank                    m_virtualInputs;

    /// Bank for the Card's Output IO Register Points
    Fxt::Point::Bank                    m_ioRegisterOutputs;

    /// Bank for the Card's OUtput Virtual Points
    Fxt::Point::Bank                    m_virtualOutputs;

    /// Array of IoRegister Point Pointers.  Ordered by Channel Index
    Fxt::Point::Api**                   m_ioRegisterPoints;

    /// Error state. A value of 0 indicates NO error
    Fxt::Type::Error                    m_error;

    /// My slot number
    uint8_t                             m_slotNum;

    /// My started state
    bool                                m_started;
};



};      // end namespaces
};
#endif  // end header latch
