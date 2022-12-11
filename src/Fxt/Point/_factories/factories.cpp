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


#include "Fxt/Point/Factory.h"
#include "Fxt/Point/FactoryDb.h"
#include "Fxt/Point/Bool.h"
#include "Fxt/Point/Int8.h"
#include "Fxt/Point/Int16.h"
#include "Fxt/Point/Int32.h"
#include "Fxt/Point/Int64.h"
#include "Fxt/Point/Uint8.h"
#include "Fxt/Point/Uint16.h"
#include "Fxt/Point/Uint32.h"
#include "Fxt/Point/Uint64.h"
#include "Fxt/Point/Float.h"
#include "Fxt/Point/Double.h"
#include "Fxt/Point/String.h"

///
using namespace Fxt::Point;

FactoryDatabase  Fxt::Point::g_pointFactoryDb;

static Factory<Bool>            boolFactory_( g_pointFactoryDb ); 
static Factory<Int8>            int8Factory_( g_pointFactoryDb );
static Factory<Int16>           int16Factory_( g_pointFactoryDb );
static Factory<Int32>           int32Factory_( g_pointFactoryDb );
static Factory<Int64>           int64Factory_( g_pointFactoryDb );
static Factory<Double>          doubleFactory_( g_pointFactoryDb );
static Factory<Float>           floatFactory_( g_pointFactoryDb );
static Factory<Uint8>           uint8Factory_( g_pointFactoryDb );
static Factory<Uint16>          uint16Factory_( g_pointFactoryDb );
static Factory<Uint32>          uint32Factory_( g_pointFactoryDb );
static Factory<Uint64>          uint64Factory_( g_pointFactoryDb );
static Factory1DArray<String>   stringFactory_( g_pointFactoryDb );
