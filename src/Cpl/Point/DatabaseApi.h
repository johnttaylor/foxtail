#ifndef Cpl_Point_DatabaseApi_h_
#define Cpl_Point_DatabaseApi_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2021  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/Text/String.h"
#include "Cpl/Point/Info.h"
#include "Cpl/Point/Identifier.h"

///
namespace Cpl {
///
namespace Point {


/** This class defines the basic operations that can be performed on a Point
    Database.  A Point Database is a collection of instantiated Points.

    Unless explicitly commented - the Database Interface can ONLY be called 
    from the "Point Thread"
 */
class DatabaseApi
{
public:
    /** This method performs a generic looks-up of a Point instance by
        its numeric Identifier and returns a pointer to the instance.  If the
        Point identifier cannot be found and 'fatalOnNotFound' is true than a
        fatal error is generated; else nullptr is returned

        This method is ONLY okay to call from ANY thread AFTER the Point database
        has been populated.

        This method is not type-safe because it returns point to the Point base
        class (which does not have the read/write operations).  The application
        is required to properly down case the pointer to appropriate child class.
        The recommendation is for the Application to provide a type safe wrappers
        on top of the lookupById() method.  This can be done by provide a creating
        a child class of the Database implementation and add wrapper methods 
        for each unique Point type used by the application.  The following is
        example:
        \code
        
            class MyApplicationDb: public Cpl::Point::Database
            {
            public:
               /// Constructor
               MyApplicationDb(...):Database(...){}

            public:
               // Type-safe lookup (using the 'function' operator)
               inline Cpl::Point::Uint32& operator() ( const Cpl::Point::Uint32::Id_T numericId ) const noexcept 
                 { 
                    return *((Cpl::Point::Uint32 *)lookupById( numericId, true )); 
                 }

            ...
            };
            
            ...

            extern MyApplicationDb          g_database;
            extern Cpl::Point::Uint32::Id_T inputSignalAbc;
            Cpl::Point::Uint32&             myPt = g_database(inputSignalAbc);

        \endcode
     */
    virtual Cpl::Point::Api* lookupById( const Identifier_T pointIdToFind, bool fatalOnNotFound=false ) const noexcept = 0;


public:
    /** This method returns the symbolic name associated with the specified Point.
        If the Point identifier is not valid or not in the database, than a
        nullptr is returned.

        This method is ONLY okay to call from ANY thread AFTER the Point database
        has been populated.
     */
    const char* getSymbolicName( const Identifier_T pointIdToFind ) const noexcept;

public:
    /** This method converts the Point's data to JSON string and
        copies the resultant string into 'dst'.  If the Point's data
        cannot be represented as a JSON object then the contents of 'dst' is
        set to an empty string and the method returns false; else the method
        returns true. The format of the string is specific to the concrete leaf class.
        However, it is strongly recommended that the output of this method be
        the same format that is expected for the fromJSON() method.

        This method is can ONLY be called from a "Point Thread" AND that database
        has been populated.

        If the specified 'srcPoint' Point is not in the database - the method
        does nothing and returns false.

        NOTE: If the converted string is larger than the memory allocated by
              'dst' then the string result in 'dst' will be truncated. The
              caller is required to check 'truncated' flag for the truncated
              scenario.


        The general output format:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:<value> }

        Notes:
            - The 'val' key/value pair is omitted if the Point is in the invalid state
            - The 'val' key/value pair can be a single element, an object, or
              array. etc. -- it is specific to the concrete Point type/class.

        \endcode
     */
    virtual bool toJSON( const Identifier_T srcPoint,
                         char*              dst,
                         size_t             dstSize,
                         bool&              truncated,
                         bool               verbose = true ) noexcept = 0;

    /** This method attempts to convert the null terminated JSON formated 'src'
        string to its binary format and copies the result to the Point's
        internal data. The Point instance being updated is selected by specifying
        its numeric Identifier. The expected format of the JSON string is specific to
        the concrete leaf class.

        This method is can ONLY be called from a "Point Thread" AND that database
        has been populated.

        If the conversion is successful true is returned. If the contents of
        the 'src' is not a valid JSON object and/or not parse-able, OR the Point
        does not support a full/complete conversion from Text to binary, OR
        the conversion fails, OR the specified Point identifier does not exist
        then the method returns false.  When the conversion fails, the optional
        'errorMsg' argument is updated with a plain text error message.


        The general input format:
        \code

        { id=12, locked:true|false }              // Locks/unlocks the Point
        { id=12, valid:false }                    // Invalidates the Point
        { id=12, valid:false, locked:true }       // Invalidates the Point and locks the Point
        { id=12, val:<value> }                    // Writes a new (valid) value to the Point
        { id=12, val:<value>, locked:true }       // Writes a new (valid) value to the Point and locks the Point

        \endcode
     */
    virtual bool fromJSON( const char* src, Cpl::Text::String* errorMsg=0 ) noexcept = 0;


public:
    /** This adds the Point instance to the database.  False is returned when 
        there is insufficient memory to add the Point; else true is returned.

        It is okay for 'pointInfo' to go out-of-scope after calling this method.

        This method is can ONLY be called from a "Point Thread" 

        The Application is REPSONSIBLE FOR:
        - All 'numericId' values are unique.
        - All 'numericId' values are within the range of 0 to 'maximumNumberOfPoints' 
          supported by the concrete database implementation.
     */
    virtual bool add( const Identifier_T numericId, Info_T& pointInfo ) noexcept = 0;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~DatabaseApi() {}
};


};      // end namespaces
};
#endif  // end header latch
