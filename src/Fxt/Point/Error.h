#ifndef Fxt_Point_Error_h_
#define Fxt_Point_Error_h_
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


#include "Fxt/Type/Error.h"




///
namespace Fxt {
///
namespace Point {

/** This enumeration defines error codes that are unique to the Point Sub-system

        Note: Each error code symbol must be less than 32 characters
           123456789 123456789 12345678 12

    @param Err_T                            Enum

    @param SUCCESS                          NO ERROR
    @param MEMORY_POINT                     Unable to allocate memory for the Point
    @param UNKNOWN_GUID                     Configuration contains the unknown/unsupported Point type GUID 
    @param MISSING_ID                       Configuration does NOT contain a valid ID value a Point
    @param FAILED_DB_INSERT                 Failed to add the point to Point DB (duplicate ID or out-of-memory in db)
    @param MISSING_TYPE_CFG                 Configuration does NOT contain a valid/properly-form 'typeCfg' key/value pair
    @param BAD_SETTER_VALUE                 Configuration does not contain a valid value for the Setter/Initial value
    @param BANK_CONT_ERROR                  Attempted to continuing populating a Bank instance after a Bank failure
 */
BETTER_ENUM( Err_T, uint8_t
             , SUCCESS = 0
             , MEMORY_POINT
             , UNKNOWN_GUID
             , MISSING_ID       
             , MISSING_TYPE_CFG
             , BAD_SETTER_VALUE
             , BANK_CONT_ERROR
             , FAILED_DB_INSERT
);

/** This concrete class defines the Error Category for the Point namespace.
    This class is designed as SINGLETON.
 */
class ErrCategory : public Fxt::Type::ErrorCategory<Err_T>
{
public:
    /// Constructor
    ErrCategory()
        :Fxt::Type::ErrorCategory<Err_T>( "POINT", 1, Fxt::Type::Error::getErrorCategoriesRoot() )
    {
    }

public:
    /// Singleton
    static ErrCategory g_theOne;
};

/// Helper method to build a 'complete/full' error from a specific error code
inline Fxt::Type::Error fullErr( Err_T localErr )
{
    return Fxt::Type::Error( ErrCategory::g_theOne, localErr );
}


};      // end namespaces
};
#endif  // end header latch
