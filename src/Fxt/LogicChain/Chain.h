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


#include "Fxt/LogicChain/Api.h"
#include "Fxt/LogicChain/Error.h"
#include "Fxt/Point/Bank.h"
#include "Fxt/Point/DatabaseApi.h"
#include "Fxt/Point/Descriptor.h"
#include "Cpl/Json/Arduino.h"
#include "Cpl/Memory/ContiguousAllocator.h"

///
namespace Fxt {
///
namespace LogicChain {


/** This concrete class implements the LogicChain interface
 */
class Chain : public Api
{
public:

public:
    /// Constructor
    Chain( DatabaseApi&                       cardDb,
             Cpl::Memory::ContiguousAllocator&  generalAllocator,
             Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
             Fxt::Point::DatabaseApi&           dbForPoints,
             uint16_t                           cardId );

    /// Destructor
    ~Common_();

public:
    /// See Fxt::LogicChain::Api
    Fxt::Type::Error start( uint64_t currentElapsedTimeUsec ) noexcept;

    /// See Fxt::LogicChain::Api
    void stop() noexcept;

    /// See Fxt::LogicChain::Api
    bool isStarted() const noexcept;

    /// See Fxt::LogicChain::Api
    Fxt::Type::Error execute( int64_t currentTickUsec ) noexcept;



protected:
    /// Helper method to create descriptor instances
    bool createDescriptors( Fxt::Point::Descriptor::CreateFunc_T createFunc,
                            Fxt::Point::Descriptor*              vpointDesc[],
                            Fxt::Point::Descriptor*              ioRegDesc[],
                            uint16_t                             channelIds[],
                            JsonArray&                           json,
                            size_t                               numDescriptors ) noexcept;
    
    /** Helper method to set an initial value from a 'setter'.  If the 
        the descriptor is null or the descriptor has no 'setter' then nothing 
        is done
     */
    void setInitialValue( Fxt::Point::Descriptor* descriptor ) noexcept;
    
protected:
    /// Handle to the Point data base
    Fxt::Point::DatabaseApi&            m_dbForPoints; 

    /// General purpose allocator
    Cpl::Memory::ContiguousAllocator&   m_generalAllocator;

    /// Allocator for a Point's stateful data
    Cpl::Memory::ContiguousAllocator&   m_statefulDataAllocator;
    
    /// Bank for the Card's Input IO Register Points
    Fxt::Point::Bank                    m_registerInputs;

    /// Bank for the Card's Input Virtual Points
    Fxt::Point::Bank                    m_virtualInputs;          

    /// Bank for the Card's Output IO Register Points
    Fxt::Point::Bank                    m_registerOutputs;        
    
    /// Bank for the Card's OUtput Virtual Points
    Fxt::Point::Bank                    m_virtualOutputs;         

    /// Error state. A value of 0 indicates NO error
    Fxt::Type::Error                    m_error;

    /// The card's ID
    uint16_t                            m_id;

    /// My started state
    bool                                m_started;
};



};      // end namespaces
};
#endif  // end header latch
