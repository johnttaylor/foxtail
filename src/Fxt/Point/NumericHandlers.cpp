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


#include "NumericHandlers.h"
#include "Fxt/Point/Int8.h"
#include "Fxt/Point/Uint8.h"
#include "Fxt/Point/Int16.h"
#include "Fxt/Point/Uint16.h"
#include "Fxt/Point/Int32.h"
#include "Fxt/Point/Uint32.h"
#include "Fxt/Point/Int64.h"
#include "Fxt/Point/Uint64.h"
#include "Fxt/Point/Double.h"
#include "Fxt/Point/Float.h"

///
using namespace Fxt::Point;


///////////////////////////////////////////////////////////////////////////////
static void writeInt8( Fxt::Point::Api* genericPt, uint64_t value );
static void writeUint8( Fxt::Point::Api* genericPt, uint64_t value );
static void writeInt16( Fxt::Point::Api* genericPt, uint64_t value );
static void writeUint16( Fxt::Point::Api* genericPt, uint64_t value );
static void writeInt32( Fxt::Point::Api* genericPt, uint64_t value );
static void writeUint32( Fxt::Point::Api* genericPt, uint64_t value );
static void writeInt64( Fxt::Point::Api* genericPt, uint64_t value );
static void writeUint64( Fxt::Point::Api* genericPt, uint64_t value );
static void writeFloat( Fxt::Point::Api* genericPt, double value );
static void writeDouble( Fxt::Point::Api* genericPt, double value );

static bool readInt8( Fxt::Point::Api* genericPt, uint64_t& dstValue );
static bool readUint8( Fxt::Point::Api* genericPt, uint64_t& dstValue );
static bool readInt16( Fxt::Point::Api* genericPt, uint64_t& dstValue );
static bool readUint16( Fxt::Point::Api* genericPt, uint64_t& dstValue );
static bool readInt32( Fxt::Point::Api* genericPt, uint64_t& dstValue );
static bool readUint32( Fxt::Point::Api* genericPt, uint64_t& dstValue );
static bool readInt64( Fxt::Point::Api* genericPt, uint64_t& dstValue );
static bool readUint64( Fxt::Point::Api* genericPt, uint64_t& dstValue );
static bool readFloat( Fxt::Point::Api* genericPt, double& dstValue );
static bool readDouble( Fxt::Point::Api* genericPt, double& dstValue );


static const NumericHandlers::IntegerAttributes_T attrInt8_   ={ writeInt8, readInt8, sizeof( int8_t ) * 8 , true };
static const NumericHandlers::IntegerAttributes_T attrUint8_  ={ writeUint8, readUint8, sizeof( uint8_t ) * 8 , false };
static const NumericHandlers::IntegerAttributes_T attrInt16_  ={ writeInt16, readInt16, sizeof( int16_t ) * 8 , true };
static const NumericHandlers::IntegerAttributes_T attrUint16_ ={ writeUint16, readUint16, sizeof( uint16_t ) * 8 , false };
static const NumericHandlers::IntegerAttributes_T attrInt32_  ={ writeInt32, readInt32, sizeof( int32_t ) * 8 , true };
static const NumericHandlers::IntegerAttributes_T attrUint32_ ={ writeUint32, readUint32, sizeof( uint32_t ) * 8 , false };
static const NumericHandlers::IntegerAttributes_T attrInt64_  ={ writeInt64, readInt64, sizeof( int64_t ) * 8 , true };
static const NumericHandlers::IntegerAttributes_T attrUint64_ ={ writeUint64, readUint64, sizeof( uint64_t ) * 8, false };
static const NumericHandlers::FloatAttributes_T   attrFloat_  ={ writeFloat, readFloat, sizeof( float ) * 8 };
static const NumericHandlers::FloatAttributes_T   attrDouble_ ={ writeDouble, readDouble, sizeof( double ) * 8 };


///////////////////////////////////////////////////////////////////////////////
const NumericHandlers::IntegerAttributes_T* NumericHandlers::getIntegerPointAttributes( const char* typeGuid )
{
    const IntegerAttributes_T* result = nullptr;

    if ( strcmp( typeGuid, Fxt::Point::Uint64::GUID_STRING ) == 0 )
    {
        result = &attrUint64_;
    }
    else if ( strcmp( typeGuid, Fxt::Point::Int64::GUID_STRING ) == 0 )
    {
        result = &attrInt64_;
    }
    else if ( strcmp( typeGuid, Fxt::Point::Uint32::GUID_STRING ) == 0 )
    {
        result = &attrUint32_;
    }
    else if ( strcmp( typeGuid, Fxt::Point::Int32::GUID_STRING ) == 0 )
    {
        result = &attrInt32_;
    }
    else if ( strcmp( typeGuid, Fxt::Point::Uint16::GUID_STRING ) == 0 )
    {
        result = &attrUint16_;
    }
    else if ( strcmp( typeGuid, Fxt::Point::Int16::GUID_STRING ) == 0 )
    {
        result = &attrInt16_;
    }
    else if ( strcmp( typeGuid, Fxt::Point::Uint8::GUID_STRING ) == 0 )
    {
        result = &attrUint8_;
    }
    else if ( strcmp( typeGuid, Fxt::Point::Int8::GUID_STRING ) == 0 )
    {
        result = &attrInt8_;
    }

    return result;
}

const NumericHandlers::FloatAttributes_T* NumericHandlers::getFloatPointAttributes( const char* typeGuid )
{
    const FloatAttributes_T* result = nullptr;

    if ( strcmp( typeGuid, Fxt::Point::Float::GUID_STRING ) == 0 )
    {
        result = &attrFloat_;
    }
    else if ( strcmp( typeGuid, Fxt::Point::Double::GUID_STRING ) == 0 )
    {
        result = &attrDouble_;
    }

    return result;
}


///////////////////////////////////////////////////////////////////////////////
bool readUint8( Fxt::Point::Api* genericPt, uint64_t& dstValue )
{
    Fxt::Point::Uint8* pt = (Fxt::Point::Uint8*) genericPt;
    uint8_t value;
    bool valid = pt->read( value );
    dstValue    = (uint64_t) value;
    return valid;
}

bool readInt8( Fxt::Point::Api* genericPt, uint64_t& dstValue )
{
    Fxt::Point::Int8* pt = (Fxt::Point::Int8*) genericPt;
    int8_t value;
    bool valid = pt->read( value );
    dstValue    = (uint64_t) value;
    return valid;
}

bool readUint16( Fxt::Point::Api* genericPt, uint64_t& dstValue )
{
    Fxt::Point::Uint16* pt = (Fxt::Point::Uint16*) genericPt;
    uint16_t value;
    bool valid = pt->read( value );
    dstValue    = (uint64_t) value;
    return valid;

}
bool readInt16( Fxt::Point::Api* genericPt, uint64_t& dstValue )
{
    Fxt::Point::Int16* pt = (Fxt::Point::Int16*) genericPt;
    int16_t value;
    bool valid = pt->read( value );
    dstValue    = (uint64_t) value;
    return valid;
}

bool readUint32( Fxt::Point::Api* genericPt, uint64_t& dstValue )
{
    Fxt::Point::Uint32* pt = (Fxt::Point::Uint32*) genericPt;
    uint32_t value;
    bool valid = pt->read( value );
    dstValue    = (uint64_t) value;
    return valid;
}
bool readInt32( Fxt::Point::Api* genericPt, uint64_t& dstValue )
{
    Fxt::Point::Int32* pt = (Fxt::Point::Int32*) genericPt;
    int32_t value;
    bool valid = pt->read( value );
    dstValue    = (uint64_t) value;
    return valid;
}

bool readUint64( Fxt::Point::Api* genericPt, uint64_t& dstValue )
{
    Fxt::Point::Uint64* pt = (Fxt::Point::Uint64*) genericPt;
    bool valid = pt->read( dstValue );
    return valid;

}
bool readInt64( Fxt::Point::Api* genericPt, uint64_t& dstValue )
{
    Fxt::Point::Int64* pt = (Fxt::Point::Int64*) genericPt;
    int64_t value;
    bool valid = pt->read( value );
    dstValue    = (uint64_t) value;
    return valid;

}

bool readFloat( Fxt::Point::Api* genericPt, double& dstValue )
{
    Fxt::Point::Float* pt = (Fxt::Point::Float*) genericPt;
    float value;
    bool valid = pt->read( value );
    dstValue   = value;
    return valid;
}
bool readDouble( Fxt::Point::Api* genericPt, double& dstValue )
{
    Fxt::Point::Double* pt = (Fxt::Point::Double*) genericPt;
    bool valid = pt->read( dstValue );
    return valid;
}

///////////////////////////////////////////////////////////////////////////////
void writeUint8( Fxt::Point::Api* genericPt, uint64_t value )
{
    Fxt::Point::Uint8* pt = (Fxt::Point::Uint8*) genericPt;
    pt->write( (uint8_t) value );
}
void writeInt8( Fxt::Point::Api* genericPt, uint64_t value )
{
    Fxt::Point::Int8* pt = (Fxt::Point::Int8*) genericPt;
    pt->write( (int8_t) value );
}

void writeUint16( Fxt::Point::Api* genericPt, uint64_t value )
{
    Fxt::Point::Uint16* pt = (Fxt::Point::Uint16*) genericPt;
    pt->write( (uint16_t) value );
}
void writeInt16( Fxt::Point::Api* genericPt, uint64_t value )
{
    Fxt::Point::Int16* pt = (Fxt::Point::Int16*) genericPt;
    pt->write( (int16_t) value );
}

void writeUint32( Fxt::Point::Api* genericPt, uint64_t value )
{
    Fxt::Point::Uint32* pt = (Fxt::Point::Uint32*) genericPt;
    pt->write( (uint32_t) value );
}
void writeInt32( Fxt::Point::Api* genericPt, uint64_t value )
{
    Fxt::Point::Int32* pt = (Fxt::Point::Int32*) genericPt;
    pt->write( (int32_t) value );
}

void writeUint64( Fxt::Point::Api* genericPt, uint64_t value )
{
    Fxt::Point::Uint64* pt = (Fxt::Point::Uint64*) genericPt;
    pt->write( value );
}
void writeInt64( Fxt::Point::Api* genericPt, uint64_t value )
{
    Fxt::Point::Int64* pt = (Fxt::Point::Int64*) genericPt;
    pt->write( (int64_t) value );
}

void writeFloat( Fxt::Point::Api* genericPt, double value )
{
    Fxt::Point::Float* pt = (Fxt::Point::Float*) genericPt;
    pt->write( (float) value );
}
void writeDouble( Fxt::Point::Api* genericPt, double value )
{
    Fxt::Point::Double* pt = (Fxt::Point::Double*) genericPt;
    pt->write( value );
}
