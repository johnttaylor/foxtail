#ifndef Fxt_Type_Error_h_
#define Fxt_Type_Error_h_
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

    Error codes a hierarchical in that are categorized into 4 levels.  Each
    level in the hierarchical tree adds sub-system/entity specific information.
    This is done to provide as much information as possible when errors occurred
    with having to have a single file that contains ALL POSSIBLE error codes.

    \code
  
    Details:
                           BYTE0   BYTE1   BYTE2   BYTE3
        final error code: [  L0  ][  L1  ][  L2  ][  L3  ]

        - Where each level is defined by a BETTER_ENUM
        - Where the top/root/L0 level is defined by the 'ErrBase_T' enum.  
        - Each level is limited to 127 possible error codes.
        - Each level enum must define a value of zero as no-error/success
*/


#include "Cpl/Type/enum.h"
#include "Cpl/Text/String.h"
#include <stdint.h>




///
namespace Fxt {
///
namespace Type {

/** This enumeration defines error codes that global/generic to the entire
    FoxTail namespace

        Note: Each error code symbol must be less than 32 characters
           123456789 123456789 12345678 12

    @param Err_T                            Enum

    @param SUCCESS                          NO Error.  The operation was successful
    @param POINT                            A Point sub-system specific error
    @param CARD                             A Card sub-system specific error
    @param COMPONENT                        A Component sub-system specific error
    @param LOGIC_CHAIN                      A Logic Chain sub-system specific error
    @param CHASSIS                          A Chassis sub-system specific error
 */
BETTER_ENUM( Err_T, uint8_t
             , SUCCESS = 0
             , POINT
             , CARD
             , COMPONENT
             , LOGIC_CHAIN
             , CHASSIS
);

/** This class contains a full/final/composite error code. 

    Note: This class is NOT fully thread safe, i.e. specifically the
          constructors.  This means the is the FXT Namespace responsible
          to ONLY construct error value from the appropriate BETTER_ENUM
          definitions.
 */
class Error
{
public:
    /// Short hand for SUCCESS
    inline static Error SUCCESS() { return Error(0); };

public:
    /// Constructor. No error
    constexpr Error() : errVal( 0 ) {}

    /// Construct by level
    constexpr Error( uint8_t level0 )
        : errVal( level0  )
    {
    }

    /// Construct by level
    constexpr Error( uint8_t level0, uint8_t level1 )
        : errVal( level1? (level0|(level1<<8)): 0 )
    {
    }

    /// Construct by level
    constexpr Error( uint8_t level0, uint8_t level1, uint8_t level2 )
        : errVal( level2 ? (level0 | (level1 << 8) | (level2<<16)) : 0 )
    {
    }

    /// Construct by level
    constexpr Error( uint8_t level0, uint8_t level1, uint8_t level2, uint8_t level3 )
        : errVal( level3 ? (level0 | (level1 << 8) | (level2 << 16) | (level3<<4)) : 0 )
    {
    }

    /// Compare
    inline bool operator == ( Error other ) const
    {
        return errVal == other.errVal;
    }

    /// Compare
    inline bool operator == ( uint32_t other ) const
    {
        return errVal == other;
    }

    /// Compare
    inline bool operator != ( Error other ) const
    {
        return errVal != other.errVal;
    }

    /// Compare
    inline bool operator != ( uint32_t other ) const
    {
        return errVal != other;
    }

public:
    /// Maximum string size when converting a error code to text
    static constexpr unsigned MAX_TEXT_LEN = (32+1+32+1+32+1+32);

    /** Magic text that is returned when an invalid/unsupported error code is
        converted to text
     */
    static constexpr const char* UNKNOWN_TEXT = "<unknown>";

    /** Convert the error number to (for some values) a human readable string.
        The format of the string is: "L0:L1:L2:L3".  For example:
           errVal   0:          "NO_ERROR"
                    0x0101      "CARD:OUT_OF_MEMORY"
                    0x010101    "CARD:DIGITAL:MISSING_REFERENCE"

        NOTE: If the error value is not valid (i.e. individual levels do not
              map back to valid BETTER_ENUM values, the '<unknown>' is returned
              and the conversion process is stopped.
     */
    const char* toText( Cpl::Text::String& bufferToHoldTheText ) const noexcept;

public:
    /** This method will print out a COMPLETE list of errors to stdout.  It
        returns the number of error codes found.  The code does NOT include
        the 'success' codes.
     */
    static size_t printAllErrorCodes() noexcept;

public:
    /// Error value
    uint32_t errVal;        //!< Error value
};

/// Helper method to build a 'complete/full' error from the top level enum
inline Fxt::Type::Error fullErr( Err_T topErr )
{
    return Fxt::Type::Error( topErr );
}

};      // end namespaces
};
#endif  // end header latch
