#ifndef Fxt_Point_Database_h_
#define Fxt_Point_Database_h_
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

#include "colony_config.h"
#include "Fxt/Point/DatabaseApi.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/Container/Map.h"
#include "Cpl/Json/Arduino.h"


/** This symbol defines the size, in bytes, of a single/global JSON document
    buffer that is used for the toJSON() and fromJSON() operations. Only one
    instance of this buffer is allocated.
*/
#ifndef OPTION_FXT_POINT_DATABASE_MAX_CAPACITY_JSON
#define OPTION_FXT_POINT_DATABASE_MAX_CAPACITY_JSON          (1024*2)
#endif

/** This symbol defines the size, in bytes, of temporary storage allocated for
    use by the fromJSON_() method (e.g. create a temporary array instance)
 */
#ifndef OPTION_FXT_POINT_DATABASE_TEMP_STORAGE_SIZE
#define OPTION_FXT_POINT_DATABASE_TEMP_STORAGE_SIZE         (1024*2)
#endif


///
namespace Fxt {
///
namespace Point {


/** This concrete class implements a simple Point Database.
  */
class Database : public DatabaseApi
{
public:
    /// Constructor
    Database() noexcept;

public:
    /// See Fxt::Point::DatabaseApi
    Fxt::Point::Api* lookupById( const Identifier_T pointIdToFind, bool fatalOnNotFound=true ) const noexcept;

    /// See Fxt::Point::DatabaseApi
    //ModelPoint* getFirstByName() noexcept;

    ///// See Fxt::Point::DatabaseApi
    //ModelPoint* getNextByName( ModelPoint& currentModelPoint ) noexcept;

    /// See Fxt::Point::DatabaseApi
    bool fromJSON( const char* src, Cpl::Text::String* errorMsg=0 ) noexcept;

public:
   // /** This method has 'PACKAGE Scope' in that is should only be called by
   //    other classes in the Fxt::Point namespace.  It is ONLY public to avoid
   //    the tight coupling of C++ friend mechanism.

   //    This method inserts a new Model Point into the Point Database.
   //*/
   // void insert_( ModelPoint& mpToAdd ) noexcept;

   // /** This method has 'PACKAGE Scope' in that is should only be called by
   //     other classes in the Fxt::Point namespace.  It is ONLY public to avoid
   //     the tight coupling of C++ friend mechanism.

   //     This method locks the Point Database.  For every call to lock() there must
   //     be corresponding call to unlock();
   // */
   // void lock_() noexcept;

   // /** This method has 'PACKAGE Scope' in that is should only be called by
   //     other classes in the Fxt::Point namespace.  It is ONLY public to avoid
   //     the tight coupling of C++ friend mechanism.

   //     This method unlocks the Point Database.
   // */
   // void unlock_() noexcept;



public:
    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Fxt::Point namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        This method provides a single global lock for ALL Point Database
        instances. The method is used to protect global Point Database (e.g.
        the global parse buffer).

        This method locks the global Point Database lock. For every call to
        globalLock_() there must be corresponding call to globalUnlock_();
    */
    static void globalLock_() noexcept;

    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Fxt::Point namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        This method unlocks the global Point Database lock
    */
    static void globalUnlock_() noexcept;

    /** This variable has 'PACKAGE Scope' in that is should only be called by
        other classes in the Fxt::Point namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        Global/single instance of a JSON document. Model Point's need to have
        acquired the global lock before using this buffer
     */
    static StaticJsonDocument<OPTION_FXT_POINT_DATABASE_MAX_CAPACITY_JSON> g_doc_;

    /** This variable has 'PACKAGE Scope' in that is should only be called by
        other classes in the Fxt::Point namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        Global temporary buffer. Model Point's need to have acquired the global 
        lock before using this buffer
     */
    static uint8_t   g_tempBuffer_[OPTION_FXT_POINT_DATABASE_TEMP_STORAGE_SIZE];


protected:
    /// Helper method to create the database lock
    bool createLock();


private:
    /// Prevent access to the copy constructor -->Point Databases can not be copied!
    Database( const Database& m );

    /// Prevent access to the assignment operator -->Point Databases can not be copied!
    const Database& operator=( const Database& m );
};


};      // end namespaces
};
#endif  // end header latch
