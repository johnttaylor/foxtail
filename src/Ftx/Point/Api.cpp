/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Api.h"

START()
POINT( Int32, {extern info"} )
using namespace Fxt::Point;

#define ID_XYZ      0
#define ID_ABC      1
Api xyz(ID_XYZ);
Api abc(ID_ABC);

POINT( MyXyz, Int32 )

// Header file
constexpr size_t MyXyz      32 

// C++ file
initialize()
{
    instances_int32[3] = new Int32(MyXyz);
    instances_int32[4] = new Int32(MyAbc);
}

const char* const names_[2] = { "MyXyz", "Abc" };

List of all instance - number 0 to N, sorted by number
Lists by types

'define' header file
-- > #include for point type
-- > Description
-- > Macro Type, Symbolic ID


'Allocate Header file'
#include "point/auto.h"
#include 'define header files'

FXT_POINT_NAMESPACE( Fxt::pts )

/* FXT_END_APPLICATION_EDITS */

namespace macros
marker
    namespace start
    Expanded IDs
    namespace end


-->MACRO    PO