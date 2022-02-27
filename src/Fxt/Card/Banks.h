#ifndef Fxt_Card_Banks_h_
#define Fxt_Card_Banks_h_
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


#include "Fxt/Point/BankApi.h"

///
namespace Fxt {
///
namespace Card {


/// Convenience structure to reduce 'noise' in function arguments
struct Banks_T
{
    Fxt::Point::BankApi*   internalInputs;      //!< Bank that is used manage/create the card's internal Input Points
    Fxt::Point::BankApi*   registerInputs;      //!< Bank that is used manage/create the card's internal Input Registers
    Fxt::Point::BankApi*   virtualInputs;       //!< Bank that is used manage/create the cards' application visible Input Points
    Fxt::Point::BankApi*   internalOutputs;     //!< Bank that is used manage/create the card's internal Output Points
    Fxt::Point::BankApi*   registerOutputs;     //!< Bank that is used manage/create the card's internal Output Registers
    Fxt::Point::BankApi*   virtualOutputs;      //!< Bank that is used manage/create the cards' application visible Output Points

    /// Copy constructor
    Banks_T( Banks_T& src )
    {
        internalInputs  = src.internalInputs;
        registerInputs  = src.registerInputs;
        virtualInputs   = src.virtualInputs;
        internalOutputs = src.internalOutputs;
        registerOutputs = src.registerOutputs;
        virtualOutputs  = src.virtualOutputs;
    }
};

/// Convenience structure to reduce 'noise' in function arguments
struct PointAllocators_T
{
    Cpl::Memory::ContiguousAllocator*  internalInputs;      //!< Allocator for the card's internal Input Points
    Cpl::Memory::ContiguousAllocator*  registerInputs;      //!< Allocator for the card's internal Input Registers
    Cpl::Memory::ContiguousAllocator*  virtualInputs;       //!< Allocator for the cards' application visible Input Points
    Cpl::Memory::ContiguousAllocator*  internalOutputs;     //!< Allocator for the card's internal Output Points
    Cpl::Memory::ContiguousAllocator*  registerOutputs;     //!< Allocator for the card's internal Output Registers
    Cpl::Memory::ContiguousAllocator*  virtualOutputs;      //!< Allocator for the cards' application visible Output Points

    /// Copy constructor
    PointAllocators_T( PointAllocators_T& src )
    {
        internalInputs  = src.internalInputs;
        registerInputs  = src.registerInputs;
        virtualInputs   = src.virtualInputs;
        internalOutputs = src.internalOutputs;
        registerOutputs = src.registerOutputs;
        virtualOutputs  = src.virtualOutputs;
    }
};


};      // end namespaces
};
#endif  // end header latch
