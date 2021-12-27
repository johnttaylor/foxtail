#ifndef Fxt_Point_DatabaseApi_h_
#define Fxt_Point_DatabaseApi_h_
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
#include "Fxt/Point/Uint32.h"

///
namespace Fxt {
///
namespace Point {


/** This class defines the basic operations that can be performed on a Point
    Database.  A Point Database is a collection of instantiated Points.

    All methods in this class are thread Safe unless explicitly documented 
    otherwise.
 */
class DatabaseApi
{
public:
    /** This method performs a type-safe look-up of a Point instance by its numeric 
        Identifier and returns a reference to the instance.  If the Point 
        identifier cannot be found, a fatal error is generated
     */
    inline Uint32& operator() ( const Uint32::Id_T numericId ) const noexcept { return *((Uint32 *)lookupById( numericId )); }

//public:
//    /** This method returns a pointer to the first Model Point in the Database.
//        The model points are traversed in order by Point name.  If there
//        are not Model Points in the Database, the method returns 0.
//     */
//    virtual ModelPoint* getFirstByName() noexcept = 0;
//
//    /** This method returns the next (in sorted order) Model Point in the
//        Database. If the current Point is the last Model Point in the
//        Database the method returns 0.
//     */
//    virtual ModelPoint* getNextByName( ModelPoint& currentModelPoint ) noexcept = 0;
//
//
public:
    /** This method attempts to convert the null terminated JSON formated 'src' 
        string to its binary format and copies the result to the Point's 
        internal data. The Point instance being updated is selected by specifying
        its numeric Identifier. The expected format of the JSON string is specific to 
        the concrete leaf class.
 
        If the conversion is successful true is returned. If the contents of 
        the 'src' is not a valid JSON object and/or not parse-able, OR the Point 
        does not support a full/complete conversion from Text to binary, OR 
        the conversion fails, OR the specified Point identifier does not exist
        then the method returns false.  When the conversion fails, the optional 
        'errorMsg' argument is updated with a plain text error message.

        
        The general input format: 
        \code

        { id=12, locked:true|false }              // Locks/unlocks the MP
        { id=12, valid=0 }                        // Invalidates the MP
        { id=12, valid=0, locked=true }           // Invalidates the MP and locks the MP
        { id=12, val:<value> }                    // Writes a new (valid) value to the MP
        { id=12, val:<value>, locked=true }       // Writes a new (valid) value to the MP and locks the MP

         
        \endcode
     */
    virtual bool fromJSON( const char* src, Cpl::Text::String* errorMsg=0 ) noexcept = 0;

protected:
    /** This internal method performs a generic looks-up of a Point instance by
        its numeric Identifier and returns a pointer to the instance.  If the 
        Point identifier cannot be found and 'fatalOnNotFound' is true than a 
        fatal error is generated; else nullptr is returned

     */
    virtual Fxt::Point::Api* lookupById( const Identifier_T pointIdToFind, bool fatalOnNotFound=true ) const noexcept = 0;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~DatabaseApi() {}
};


};      // end namespaces
};
#endif  // end header latch
