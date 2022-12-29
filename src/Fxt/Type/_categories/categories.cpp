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
/** @file 

    This file provides the default construction of the different FoxTail
    Error categories
*/


#include "Fxt/Card/Error.h"
#include "Fxt/Point/Error.h"
#include "Fxt/Component/Error.h"
#include "Fxt/Component/Digital/Error.h"
#include "Fxt/LogicChain/Error.h"


// Instantiate the root.  Note: There are no 'local' error values - only children
Fxt::Type::ErrorCategoryRoot root_;
Fxt::Type::ErrorBase& Fxt::Type::Error::getErrorCategoriesRoot() noexcept
{
    return root_;
}

// Instantiate Error categories (and their children)
Fxt::Point::ErrCategory                     Fxt::Point::ErrCategory::g_theOne;

Fxt::Card::ErrCategory                      Fxt::Card::ErrCategory::g_theOne;

Fxt::Component::ErrCategory                 Fxt::Component::ErrCategory::g_theOne;
Fxt::Component::Digital::ErrCategory        Fxt::Component::Digital::ErrCategory::g_theOne;

Fxt::LogicChain::ErrCategory                Fxt::LogicChain::ErrCategory::g_theOne;
