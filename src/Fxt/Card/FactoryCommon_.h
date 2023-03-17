#ifndef Fxt_Card_FactoryCommon_h_
#define Fxt_Card_FactoryCommon_h_
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


#include "Fxt/Card/FactoryApi.h"
#include "Fxt/Card/FactoryDatabaseApi.h"
#include "Fxt/Card/Error.h"
#include "Fxt/Point/DatabaseApi.h"
#include "Cpl/Memory/ContiguousAllocator.h"
#include "Cpl/Itc/PostApi.h"

///
namespace Fxt {
///
namespace Card {


/** This partially concrete class provide common infrastructure for a Card
    Factory.
 */
class FactoryCommon_ : public Fxt::Card::FactoryApi
{
public:
    /// Constructor
    FactoryCommon_( FactoryDatabaseApi&  factoryDatabase,
                    Cpl::Itc::PostApi*   cardMbox = nullptr,
                    void*                extraArgs = nullptr );

    /// Destructor
    ~FactoryCommon_();

public:
    /// See Fxt::Card::FactoryApi
    void destroy( Api& cardToDestory ) noexcept;

protected:
    /** Helper method to that allocates memory for the Card and parses
        some parse basic/common fields for a card.  Returns the
        Err_T::NO_ERROR if successful; else error code is returned.
        Field(s) are returned via the function arguments.
     */
    Fxt::Type::Error allocateAndParse( JsonVariant&                       obj,
                                       Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                       size_t                             cardSizeInBytes,
                                       void*&                             memoryForCard ) noexcept;
protected:
    /** Optional pointer to the Card's mailbox (aka Thread).  The mailbox/thread
        is required when the Card needs to execute other than when scanInputs() 
        and flushOutputs() are called.
     */
    Cpl::Itc::PostApi*  m_cardMbox;

    /** Optional pointer for additional card constructor arguments
     */
    void*               m_extraArgs;
};


/** This template class implements a typesafe Card factory
 */
template <class CARDTYPE>
class Factory : public FactoryCommon_
{
public:
    /// Constructor
    Factory( FactoryDatabaseApi&  factoryDatabase, Cpl::Itc::PostApi* cardMbox=nullptr ) : FactoryCommon_( factoryDatabase, cardMbox )
    {
    }

public:
    /// See Fxt::Card::FactoryApi
    const char* getGuid() const noexcept { return CARDTYPE::GUID_STRING; }

    /// See Fxt::Card::FactoryApi
    Api* create( JsonVariant&                       cardObject,
                 Fxt::Type::Error&                  cardErrorCode,
                 Cpl::Memory::ContiguousAllocator&  generalAllocator,
                 Cpl::Memory::ContiguousAllocator&  cardStatefulDataAllocator,
                 Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                 Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                 Fxt::Point::DatabaseApi&           dbForPoints ) noexcept
    {
        //  Get basic info about the card
        void* memCardInstance;
        cardErrorCode = allocateAndParse( cardObject, generalAllocator, sizeof( CARDTYPE ), memCardInstance );
        if ( cardErrorCode == Fxt::Type::Error::SUCCESS()  )
        {
            // Create the card
            CARDTYPE* card = new(memCardInstance) CARDTYPE( generalAllocator,
                                                            cardStatefulDataAllocator,
                                                            haStatefulDataAllocator,
                                                            pointFactoryDb,
                                                            dbForPoints,
                                                            cardObject,
                                                            m_cardMbox,
                                                            m_extraArgs );

            cardErrorCode = card->getErrorCode();
            return card;
        }

        return nullptr;
    }
};

};      // end namespaces
};
#endif  // end header latch
