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
    Common_( uint16_t                           maxInputChannels,
             uint16_t                           maxOutputChannels,
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
    bool scanInputs( uint64_t currentElapsedTimeUsec ) noexcept;

    /// See Fxt::Card::Api
    bool flushOutputs( uint64_t currentElapsedTimeUsec ) noexcept;

    /// See Fxt::Card::Api
    Fxt::Type::Error getErrorCode() const noexcept;

protected:
    /** Helper method that does an initial parsing of the input and output
        points.  Creates the Virtual and IO Register Points and populates
        the m_inputIoRegisterPoints, m_outputIoRegisterPoints array.  The
        point references are stored either by 'channelNum - 1' or by
        JSON array index (i.e. order of the JSON elements)
     */
    bool parseInputOutputPoints( Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                 Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                                 Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                                 Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                                 Fxt::Point::DatabaseApi&           dbForPoints,
                                 JsonVariant&                       cardObject,
                                 bool                               storeByChannelNum,
                                 unsigned                           minInputs,
                                 unsigned                           minOutputs ) noexcept;

    /** Helper method to create the points for a single channel.  Returns a nullptr
        (and sets m_error) when there is an error.
     */
    Fxt::Point::Api* createPointForChannel( Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                                            Fxt::Point::Bank&                  pointBank,
                                            bool                               isIoRegPt,
                                            JsonObject&                        channelObject,
                                            Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                            Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                                            Fxt::Point::DatabaseApi&           dbForPoints ) noexcept;


    /** Helper method that initializes the Virtual and IO Register points
        when starting the card.  This method ensure that BOTH the Virtual
        and IO Register Point have the same initial values.  It does NOT
        update the card's physical HW (that is the responsibility of the
        concrete card class).

        The method initializes both Input and Output sets of points.

        Return false if an error occurred
     */
    bool setInitialPointValues() noexcept;

    /// Returns the channel for an point element.  Returns 0 if invalid channel# and sets m_error
    uint16_t getChannelNumber( JsonObject& channelObject, uint16_t minChannelNum, uint16_t maxChannelNum ) noexcept;

protected:
    /// Bank for the Card's Input IO Register Points
    Fxt::Point::Bank                    m_ioRegisterInputs;

    /// Bank for the Card's Input Virtual Points
    Fxt::Point::Bank                    m_virtualInputs;

    /// Bank for the Card's Output IO Register Points
    Fxt::Point::Bank                    m_ioRegisterOutputs;

    /// Bank for the Card's OUtput Virtual Points
    Fxt::Point::Bank                    m_virtualOutputs;

    /// Array of INPUT IoRegister Point Pointers.  
    Fxt::Point::Api**                   m_inputIoRegisterPoints;

    /// Array of OUTPUT IoRegister Point Pointers.  
    Fxt::Point::Api**                   m_outputIoRegisterPoints;

    /// Error state. A value of 0 indicates NO error
    Fxt::Type::Error                    m_error;

    /// Number of input channels
    unsigned                            m_numInputs;

    /// Number of output channels
    unsigned                            m_numOutputs;

    /// Maximum Number allowed input channels
    unsigned                            m_maxInputs;

    /// Maximum Number allowed output channels
    unsigned                            m_maxOutputs;
    
    /// My slot number
    uint8_t                             m_slotNum;

    /// My started state
    bool                                m_started;
};



};      // end namespaces
};
#endif  // end header latch
