#ifndef Fxt_Component_FactoryCommon_h_
#define Fxt_Component_FactoryCommon_h_
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


#include "Fxt/Component/FactoryApi.h"
#include "Fxt/Component/Error.h"
#include "Fxt/Component/FactoryDatabaseApi.h"
#include "Cpl/Memory/ContiguousAllocator.h"

///
namespace Fxt {
///
namespace Component {


/** This partially concrete class provide common infrastructure for a Card
    Factory.
 */
class FactoryCommon_ : public Fxt::Component::FactoryApi
{
public:
    /// Constructor
    FactoryCommon_( Fxt::Component::FactoryDatabaseApi& factoryDb );

    /// Destructor
    ~FactoryCommon_();

public:
    /// See Fxt::Card::FactoryApi
    void destroy( Api& componentToDestory ) noexcept;

protected:
    /** Helper method to that allocates memory for the Component and parses
        some parse basic/common fields for a component.  Returns the
        FXT_COMPONENT_ERR_NO_ERROR if successful; else error code is returned.
        Field(s) are returned via the function arguments.
     */
    Fxt::Type::Error allocateAndParse( JsonVariant&                       obj,
                                       Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                       size_t                             compenentSizeInBytes,
                                       void*&                             memoryForComponent ) noexcept;

};

/** This template class implements a typesafe Component factory
 */
template <class COMPONENTTYPE>
class Factory : public FactoryCommon_
{
public:
    /// Constructor
    Factory( FactoryDatabaseApi&  factoryDatabase ) : FactoryCommon_( factoryDatabase )
    {
    }

public:
    /// See Fxt::Component::FactoryApi
    const char* getGuid() const noexcept { return COMPONENTTYPE::GUID_STRING; }

    /// See Fxt::Component::FactoryApi
    Api* create( Fxt::Point::BankApi&               statePointBank,
                 JsonVariant&                       componentObject,
                 Fxt::Type::Error&                  componentErrorCode,
                 Cpl::Memory::ContiguousAllocator&  generalAllocator,
                 Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                 Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                 Fxt::Point::DatabaseApi&           dbForPoints ) noexcept
    {
        //  Get basic info about the card
        void* memComponentInstance;
        componentErrorCode = allocateAndParse( componentObject, generalAllocator, sizeof( COMPONENTTYPE ), memComponentInstance );
        if ( componentErrorCode == Fxt::Type::Error::SUCCESS() )
        {
            // Create the Component
            COMPONENTTYPE* component = new(memComponentInstance) COMPONENTTYPE( componentObject,
                                                                                generalAllocator,
                                                                                statefulDataAllocator,
                                                                                statePointBank,
                                                                                pointFactoryDb,
                                                                                dbForPoints );

            componentErrorCode = component->getErrorCode();
            return component;
        }

        return nullptr;
    }
};

};      // end namespaces
};
#endif  // end header latch
