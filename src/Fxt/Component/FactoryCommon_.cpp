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


#include "FactoryCommon_.h"


///
using namespace Fxt::Component;

FactoryCommon_::FactoryCommon_( Fxt::Component::FactoryDatabaseApi& factoryDb )
{
    // Auto register the card factory
    factoryDb.insert_( *this );
}

FactoryCommon_::~FactoryCommon_()
{
}


void FactoryCommon_::destroy( Api& componentToDestory ) noexcept
{
    // Call the card's destructor. Note: The Application is still responsible for 'freeing/reseting' the card's memory allocator
    componentToDestory.~Api();
}


Api::Err_T FactoryCommon_::allocateAndParse( JsonVariant&                       obj,
                                             Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                             size_t                             compenentSizeInBytes,
                                             void*&                             memoryForComponent,
                                             uint16_t&                          exeOrder ) noexcept
{
    // Ensure that a Execution order has been assigned
    if ( obj["exeOrder"].is<unsigned>() == false )
    {
        return FXT_COMPONENT_ERR_MISSING_INVALID_EXE_ORDER;
    }
    exeOrder = obj["exeOrder"];

    // Allocate memory for the component
    memoryForComponent = generalAllocator.allocate( compenentSizeInBytes );
    if ( memoryForComponent == nullptr )
    {
        return FXT_COMPONENT_ERR_OUT_OF_MEMORY;
    }

    return FXT_COMPONENT_ERR_NO_ERROR;
}