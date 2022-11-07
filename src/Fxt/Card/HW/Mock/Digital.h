#ifndef Fxt_Card_Hw_Mock_Digital_h_
#define Fxt_Card_Hw_Mock_Digital_h_
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


#include "Fxt/Card/Common_.h"
#include "Cpl/Json/Arduino.h"
#include "Fxt/Point/Bool.h"
#include "Fxt/Point/BankApi.h"
#include "Cpl/Memory/ContiguousAllocator.h"
#include "Cpl/System/Mutex.h"

///
namespace Fxt {
///
namespace Card {
///
namespace HW {
///
namespace Mock {


/** This concrete class implements a mocked/emulated IO card that
    supports N digital inputs, and M digital outputs.

    In general the implementation is NOT thread safe, however the get/set methods
    ARE thread safe to allow the application/test/console to drive/access the
    inputs/outputs.

    The class dynamically creates numerous thingys and objects when it is created.
    This memory is allocated via a 'Contiguous Allocator'.  This means the this
    class will NOT free the allocated memory in its destructor, it WILL however
    call the destructor on any objects it directly created using the allocator.
    The semantics with the Application is that the Application is RESPONSIBLE for
    freeing/recycling the memory in the Contiguous Allocator when Card(s) are
    deleted.
 */
class Digital : public Fxt::Card::Common_
{
public:
    /// Type ID for the card
    static constexpr const char* GUID_STRING = "59d33888-62c7-45b2-a4d4-9dbc55914ed3";

    /// Type name for the card
    static constexpr const char* TYPE_NAME   = "Fxt::Card::HW::Mock::Digital";

public:

public:
    /// Maximum number of signals/channels/points per Input and Output
    static constexpr unsigned MAX_CHANNELS = 32;


public:
    /// Constructor
    Digital( JsonVariant&                                          cardObject,
             PointAllocators_T&                                    pointAllocators,
             Fxt::Point::DatabaseApi&                              pointDatabase,
             Cpl::Memory::ContiguousAllocator&                     allocator );

    /// Destructor
    ~Digital();

public:
    /// See Fxt::Card::Api
    bool start() noexcept;

    /// See Fxt::Card::Api
    bool stop() noexcept;

    /// See Fxt::Card::Api
    const char* getGuid() const noexcept;

    /// See Fxt::Card::Api
    const char* getTypeName() const noexcept;

public:
    /// Provide the Application the ability to set the inputs. This method is thread safe
    void setInputs( uint32_t bitMaskToOR );

    /// Provide the Application the ability to clear the inputs. This method is thread safe
    void clearInputs( uint32_t bitMaskToAND );

    /// Provide the Application the ability to toggle the inputs. This method is thread safe
    void toggleInputs( uint32_t bitMaskToXOR );

    /// Provide the Application the ability to read the inputs. Returns false if one or more of the outputs is INVALID; else true is returned
    bool getOutputs( uint32_t& valueMask );

    /// Set input by bit position (zero based bit position)
    inline void setInputBit( uint8_t bitPosition ) { setInputs( ((uint32_t) 1) << bitPosition ); }

    /// Clear input by bit position (zero based bit position)
    inline void clearInputBit( uint8_t bitPosition ) { clearInputs( ~(((uint32_t) 1) << bitPosition) ); }

    /// Toggle input by bit position (zero based bit position)
    inline void toggleInputBit( uint8_t bitPosition ) { toggleInputs( ((uint32_t) 1) << bitPosition ); }


protected:
    /// Contains the channel to internal point instance mapping
    struct ChannelInfo_T
    {
        Fxt::Point::Bool*   point;          //!< Internal point associated with the channel
        uint8_t             channel;        //!< Channel number of the signal
    };

protected:
    /// Helper method to parse the card's JSON config
    virtual void parseConfiguration( JsonVariant& obj ) noexcept;

    /// Helper method to create Point descriptors
    virtual bool createDescriptors( ChannelInfo_T* channels, JsonArray& json, size_t numDescriptors, uint32_t errCode ) noexcept;

    /// Helper method to create the point instances
    virtual void createPoints( Fxt::Point::DatabaseApi&          pointDb, 
                               PointAllocators_T&                pointAllocators,
                               Cpl::Memory::ContiguousAllocator& generalAllocator ) noexcept;

    /// Helper method that returns a Point handle for the specified channel.  Returns nullptr if not match found
    Fxt::Point::Bool* getPointByChannel( ChannelInfo_T * channels, uint16_t channelIndex ) noexcept;

    /// Helper method that updates individual Bool Points based on the current input mask value
    void maskToPoints() noexcept;

    /// Helper method that updates the current output mask value from individual Bool Points
    bool pointsToMask() noexcept;

protected:
    /// Mutex to provide thread safety for the application driving/reading the mocked IO
    Cpl::System::Mutex                  m_lock;

    /// List of 'visible' Input Descriptors (allocate space for max IO plus a list-terminator)
    Fxt::Point::Descriptor*             m_externalInDescriptors[MAX_CHANNELS + 1];

    /// List of IO Register Input Descriptors (allocate space for max IO plus a list-terminator)
    Fxt::Point::Descriptor*             m_ioRegInDescriptors[MAX_CHANNELS+1];

    /// List of Internal Input Descriptors (allocate space for max IO plus a list-terminator)
    Fxt::Point::Descriptor*             m_intennalInDescriptors[MAX_CHANNELS + 1];

    /// List of 'visible' Output Descriptors (allocate space for max IO plus a list-terminator)
    Fxt::Point::Descriptor*             m_externalOutDescriptors[MAX_CHANNELS + 1];

    /// List of IO Register Output Descriptors (allocate space for max IO plus a list-terminator)
    Fxt::Point::Descriptor*             m_ioRegOutDescriptors[MAX_CHANNELS + 1];

    /// List of Internal Output Descriptors (allocate space for max IO plus a list-terminator)
    Fxt::Point::Descriptor*             m_intennalOutDescriptors[MAX_CHANNELS + 1];

    /// Point IDs of the Internal Register Input Points
    uint32_t                            m_inputChannels[MAX_CHANNELS];

    /// Point IDs of the Internal Register Output Points
    uint32_t                            m_outputChannels[MAX_CHANNELS];

    /// Initial (when start() is called) input values
    uint32_t                            m_initInputVals;

    /// Current input values
    uint32_t                            m_inputVals;

    /// Current output values
    uint32_t                            m_outputVals;

    /// Number of Inputs
    uint8_t                             m_numInputs;

    /// Number of Outputs
    uint8_t                             m_numOutputs;
};



};      // end namespaces
};
};
};
#endif  // end header latch
