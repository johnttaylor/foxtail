#ifndef Fxt_Point_PopulateFactoryDb_h_
#define Fxt_Point_PopulateFactoryDb_h_
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

    This is header file is used to populate a Point Factory Database with
    all known (to this namespace) Points.  To use the application must
    define the symbol FXT_MY_APP_POINT_FACTORY_DB with an instance of
    FactoryDatabaseApi.  This file should ONLY be include ONCE in SINGLE file.
*/


#include "Fxt/Point/Int8.h"
#include "Fxt/Point/Int16.h"
#include "Fxt/Point/Int32.h"
#include "Fxt/Point/Int64.h"
#include "Fxt/Point/Uint8.h"
#include "Fxt/Point/Uint16.h"
#include "Fxt/Point/Uint32.h"
#include "Fxt/Point/Uint64.h"
#include "Fxt/Point/Bool.h"
#include "Fxt/Point/Float.h"
#include "Fxt/Point/Double.h"
#include "Fxt/Point/String.h"

#include "Fxt/Point/Factory.h"

static Fxt::Point::Factory<Fxt::Point::Int8>            int8Factory_( FXT_MY_APP_POINT_FACTORY_DB );
static Fxt::Point::Factory<Fxt::Point::Int16>           int16Factory_( FXT_MY_APP_POINT_FACTORY_DB );
static Fxt::Point::Factory<Fxt::Point::Int32>           int32Factory_( FXT_MY_APP_POINT_FACTORY_DB );
static Fxt::Point::Factory<Fxt::Point::Int64>           int64Factory_( FXT_MY_APP_POINT_FACTORY_DB );

static Fxt::Point::Factory<Fxt::Point::Uint8>           uint8Factory_( FXT_MY_APP_POINT_FACTORY_DB );
static Fxt::Point::Factory<Fxt::Point::Uint16>          uint16Factory_( FXT_MY_APP_POINT_FACTORY_DB );
static Fxt::Point::Factory<Fxt::Point::Uint32>          uint32Factory_( FXT_MY_APP_POINT_FACTORY_DB );
static Fxt::Point::Factory<Fxt::Point::Uint64>          uint64Factory_( FXT_MY_APP_POINT_FACTORY_DB );

static Fxt::Point::Factory<Fxt::Point::Bool>            boolFactory_( FXT_MY_APP_POINT_FACTORY_DB );

static Fxt::Point::Factory<Fxt::Point::Float>           floatFactory_( FXT_MY_APP_POINT_FACTORY_DB );
static Fxt::Point::Factory<Fxt::Point::Double>          doubleFactory_( FXT_MY_APP_POINT_FACTORY_DB );

static Fxt::Point::Factory1DArray<Fxt::Point::String>   stringFactory_( FXT_MY_APP_POINT_FACTORY_DB );

#endif  // end header latch
