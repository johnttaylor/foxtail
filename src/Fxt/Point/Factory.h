#ifndef Fxt_Point_Factory_h_
#define Fxt_Point_Factory_h_
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

#include "Fxt/Point/FactoryCommon_.h"
#include "Fxt/Point/Error.h"
#include "Fxt/Point/FactoryDatabaseApi.h"


///
namespace Fxt {
///
namespace Point {


/** This template concrete class implements a Point factory for a Point
    data type that is a non-complex type (i.e. a non array type)
 */
template <class POINTTYPE>
class Factory : public FactoryCommon_
{
public:
    /// Constructor
    Factory( FactoryDatabaseApi& factoryDb ) : FactoryCommon_( factoryDb ) {}

public:
    /// Same as Fxt::Point::FactoryApi - excepts returns the point as a concrete sub-type
    POINTTYPE* createTypeSafe( JsonObject&                        pointObject,
                               Fxt::Type::Error&                  pointErrorCode,
                               Cpl::Memory::ContiguousAllocator&  generalAllocator,
                               Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                               Fxt::Point::DatabaseApi&           dbForPoints,
                               const char*                        pointIdKeyName = "id",
                               bool                               createSetter   = true ) noexcept
    {
        return (POINTTYPE*) create( pointObject, pointErrorCode, generalAllocator, statefulDataAllocator, dbForPoints, pointIdKeyName, createSetter );
    }

    /// See Fxt::Point::FactoryApi
    const char* getGuid() const noexcept { return POINTTYPE::GUID_STRING; }

    /// See Fxt::Point::FactoryApi
    Api* createRawPoint( JsonObject&                        pointObject,
                         Fxt::Type::Error&                  pointErrorCode,
                         Cpl::Memory::ContiguousAllocator&  generalAllocator,
                         Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                         Fxt::Point::DatabaseApi&           dbForPoints,
                         JsonObject&                        typeConfigObject,
                         const char*                        pointIdKeyName,
                         Api*                               setter = nullptr ) noexcept
    {
        // Get Point ID
        if ( pointObject[pointIdKeyName].is<unsigned long>() == false )
        {
            pointErrorCode = fullErr( Err_T::MISSING_ID );
            pointErrorCode.logIt();
            return nullptr;
        }
        uint32_t pointId = pointObject[pointIdKeyName].as<unsigned long>();

        // Create Point
        void* memPtr = generalAllocator.allocate( sizeof( POINTTYPE ) );
        if ( memPtr )
        {
            Api* newPoint = new(memPtr) POINTTYPE( dbForPoints, pointId, statefulDataAllocator, setter );
            if ( newPoint->getId() != Api::INVALID_ID )
            {
                // When I get here -->everything worked
                newPoint->updateFromSetter();
                return newPoint;
            }
            pointErrorCode = fullErr( Err_T::FAILED_DB_INSERT );
            pointErrorCode.logIt();
        }
        return nullptr;
    }
};


/** This template concrete class implements a Point factory for a Point
    data type that is (or contains a single) 1D array.  Expected Syntax
    is:

    {
        "id":                   <ID. Note: the ID key can vary, e.g. 'id', 'ioRegId', etc.>,
        "type":                 "<Points's Type GUID: 8-4-4-4-12 format>",
        "typeCfg: {
            "numElems":         <number of array elements>
         },
        "typeName":             "*<OPTIONAL: human readable point type>",
        "name":                 "*<human readable name for the point>"
        "initial": {..}         // OPTIONAL initial value/state specifier for the Point
    }
 */
template <class ARRAYPOINTTYPE>
class Factory1DArray : public FactoryCommon_
{
public:
    /// Constructor
    Factory1DArray( FactoryDatabaseApi& factoryDb ) : FactoryCommon_( factoryDb ) {}

public:
    /// Same as Fxt::Point::FactoryApi - excepts returns the point as a concrete sub-type
    ARRAYPOINTTYPE* createTypeSafe( JsonObject&                        pointObject,
                                    Fxt::Type::Error&                  pointErrorCode,
                                    Cpl::Memory::ContiguousAllocator&  generalAllocator,
                                    Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                                    Fxt::Point::DatabaseApi&           dbForPoints,
                                    const char*                        pointIdKeyName = "id",
                                    bool                               createSetter   = true ) noexcept
    {
        return (ARRAYPOINTTYPE*) create( pointObject, pointErrorCode, generalAllocator, statefulDataAllocator, dbForPoints, pointIdKeyName, createSetter );
    }

    /// See Fxt::Point::FactoryApi
    const char* getGuid() const noexcept { return ARRAYPOINTTYPE::GUID_STRING; }

    /// See Fxt::Point::FactoryApi
    Api* createRawPoint( JsonObject&                        pointObject,
                         Fxt::Type::Error&                  pointErrorCode,
                         Cpl::Memory::ContiguousAllocator&  generalAllocator,
                         Cpl::Memory::ContiguousAllocator&  statefulDataAllocator,
                         Fxt::Point::DatabaseApi&           dbForPoints,
                         JsonObject&                        typeConfigObject,
                         const char*                        pointIdKeyName,
                         Api*                               setter = nullptr ) noexcept
    {
        // Get Point ID
        if ( pointObject[pointIdKeyName].is<unsigned long>() == false )
        {
            pointErrorCode = fullErr( Err_T::MISSING_ID );
            pointErrorCode.logIt();
            return nullptr;
        }
        uint32_t pointId = pointObject[pointIdKeyName].as<unsigned long>();

        // Get array size
        if ( typeConfigObject.isNull() == false &&
             typeConfigObject["numElems"].is<unsigned long>() == false )
        {
            pointErrorCode = fullErr( Err_T::MISSING_TYPE_CFG );
            pointErrorCode.logIt();
            return nullptr;
        }
        size_t numElems = typeConfigObject["numElems"];

        // Create Point
        void* memPtr = generalAllocator.allocate( sizeof( ARRAYPOINTTYPE ) );
        if ( memPtr )
        {
            Api* newPoint = new(memPtr) ARRAYPOINTTYPE( dbForPoints, pointId, statefulDataAllocator, numElems, setter );
            if ( newPoint->getId() != Api::INVALID_ID )
            {
                // When I get here -->everything worked
                newPoint->updateFromSetter();
                return newPoint;
            }
            pointErrorCode = fullErr( Err_T::FAILED_DB_INSERT );
            pointErrorCode.logIt();
        }
        return nullptr;
    }
};

};      // end namespaces
};
#endif  // end header latch
