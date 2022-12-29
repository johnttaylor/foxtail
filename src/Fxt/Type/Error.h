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

        - Where each level is a single byte where values 0-127 are defined by
          a BETTER_ENUM.  Values 128-255 are identifiers for children error codes.
        - The top/root/L0 level is defined by the 'ErrBase_T' enum.
        - A value of zero is SUCCESS
        - Each level is limited to 127 possible error codes ad defined by a BETTER_ENUM.
        - Each level is limited to 127 possible children/sub-errors.
        - Each level enum's must define a value of zero as no-error/success
        - The number of characters in a individual BETTER_NUM symbol is limited
          to a maximum of 32 characters
        - Children/sub-errors self register with the parent Error instance.  The
          Numeric value for children/sub-errors are assigned when they register.
          This means that if the construction order changes - the absolute
          error values will change (note: the semantic error values do NOT change)
*/


#include "Cpl/Type/enum.h"
#include "Cpl/Text/String.h"
#include "Cpl/Container/Item.h"
#include "Cpl/Container/SList.h"
#include <stdint.h>




///
namespace Fxt {
///
namespace Type {

/// Forward reference
class ErrorBase;

/** This class contains a full/final/composite error code.
 */
class Error
{
public:
    /// Short hand for SUCCESS 
    inline static Error SUCCESS() { return Error( (uint32_t)0 ); };

public:
    /// Constructor. No error
    constexpr Error() : errVal( 0 ) {}

    /// Constructor. From full value
    constexpr Error( uint32_t fullErrorValue ) : errVal( fullErrorValue ) {}

    /// Copy Constructor. 
    Error( const Error& other ) : errVal( other.errVal ) {}

    /// Constructor from a Error Category
    Error( const ErrorBase& leafErrorCategory, uint8_t localCategoryErrorNumber );

    /// Construct by level
    constexpr Error( uint8_t level0 )
        : errVal( level0 )
    {
    }

    /// Construct by level
    constexpr Error( uint8_t level0, uint8_t level1 )
        : errVal( level1 ? (level0 | (level1 << 8)) : 0 )
    {
    }

    /// Construct by level
    constexpr Error( uint8_t level0, uint8_t level1, uint8_t level2 )
        : errVal( level2 ? (level0 | (level1 << 8) | (level2 << 16)) : 0 )
    {
    }

    /// Construct by level
    constexpr Error( uint8_t level0, uint8_t level1, uint8_t level2, uint8_t level3 )
        : errVal( level3 ? (level0 | (level1 << 8) | (level2 << 16) | (level3 << 4)) : 0 )
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

    /// Copy operator
    inline const Error& operator = ( const Error& other ) 
    {
        errVal = other.errVal;
        return *this;
    }

public:
    /** Maximum string size when converting a error code to text (NOT including
        the null terminator
     */
    static constexpr unsigned MAX_TEXT_LEN = (32 + 1 + 32 + 1 + 32 + 1 + 32);

    /** Magic text that is returned when an invalid/unsupported error code is
        converted to text
     */
    static constexpr const char* UNKNOWN_TEXT = "<unknown>";

    /** Convert the error number to a human readable string. The format of the
        string is: "L0:L1:L2:L3".  For example:
           errVal   0:          "NO_ERROR"
                    0x0101      "CARD:OUT_OF_MEMORY"
                    0x010101    "CARD:DIGITAL:MISSING_REFERENCE"

        NOTE: If the error value is not valid (i.e. individual levels do not
              map back to valid BETTER_ENUM values, the UNKNOWN_TEXT is returned
              and the conversion process is stopped.
     */
    static const char* toText( Error erroCode, Cpl::Text::String& bufferToHoldTheText ) noexcept;

public:
    /** This method will print out a COMPLETE list of errors to stdout.  It
        returns the number of error codes found.  The code does NOT include
        the 'success' codes.
     */
    static size_t printAllErrorCodes() noexcept;

    /** This method returns the root for Error Category tree
     */
    static ErrorBase& getErrorCategoriesRoot() noexcept;

public:
    /// Error value
    uint32_t errVal;        //!< Error value
};


/*---------------------------------------------------------------------------*/
/** Base class for Error class
 */
class ErrorBase : public Cpl::Container::Item
{
protected:
    /// Constructor.  
    ErrorBase( ErrorBase&   parentCategory,
               uint8_t      levelIndex,
               const char*  nameForErrorCategory );

    /** Constructor. This constructor is used to create the category
        when the category is STATICALLY allocated.
     */
    ErrorBase( ErrorBase&   parentCategory,
               uint8_t      levelIndex,
               const char*  nameForErrorCategory,
               const char*  ignoreThisParameter_usedToCreateAUniqueConstructor );

protected:
    /// Constructor. This constructor is used to create the 'root' category
    ErrorBase();

    /** Constructor.  This constructor is used to create the 'root' category
        when the root is STATICALLY allocated.
     */
    ErrorBase( const char* ignoreThisParameter_usedToCreateAUniqueConstructor );

public:
    /// Returns the numeric identifier for the SubError category
    inline uint8_t getCategoryIdentifier() const noexcept
    {
        return m_categoryId;
    }

    // Returns the text label for the Category identifier
    inline const char* getCategoryName() const noexcept
    {
        return m_name;
    }

    /// Returns the Category's parent.  
    inline ErrorBase* getParent() const noexcept
    {
        return m_parent;
    }

    /// Returns the Category's level index (is zero based)
    inline uint8_t getLevelIndex() const noexcept
    {
        return m_level;
    }

    /// Returns the number of child categories
    inline uint8_t getNumChildren() const noexcept
    {
        return m_numChildren;
    }

    /** This method returns the 'local' category's text label, i.e. the category's
        enum symbol.  Returns nullptr if not a local 'enum' error value
     */
    virtual const char* getLocalText( uint8_t localCategoryErrorNumber ) const noexcept = 0;

public:
    /// Returns the category' first child.  Returns nullptr if there is no children
    ErrorBase* getFirstChild() const noexcept;

    /// Returns the category's 'next' child. Returns nullptr if there is no more children
    ErrorBase* getNextChild( ErrorBase& currentChild ) const noexcept;

protected:
    /// Child categories
    Cpl::Container::SList<ErrorBase>    m_children;

    /// Parent category 
    ErrorBase*                          m_parent;

    /// Category name (must be 32 characters or less)
    const char*                         m_name;

    /// Numeric ID for the category
    uint8_t                             m_categoryId;

    /// Number of children
    uint8_t                             m_numChildren;

    /// Category level number           
    uint8_t                             m_level;
};

/** This class define the interface for a Error Category
 */
template <class ERR_ENUM>
class ErrorCategory : public ErrorBase
{
public:
    /** Constructor.  Name must be 32 characters or less.
     */
    ErrorCategory( const char*    nameForSubErrorCategory,
                   uint8_t        levelIndex,
                   ErrorBase&     parentCategory )
        : ErrorBase( parentCategory, levelIndex, nameForSubErrorCategory )
    {
    }

    /** Constructor.  This constructor is used to create the category
        when the root is STATICALLY allocated.
     */
    ErrorCategory( const char*    nameForSubErrorCategory,
                   uint8_t        levelIndex,
                   ErrorBase&     parentCategory,
                   const char*    ignoreThisParameter_usedToCreateAUniqueConstructor )
    : ErrorBase( parentCategory, levelIndex, nameForSubErrorCategory, ignoreThisParameter_usedToCreateAUniqueConstructor )
    {
    }

public:
    /// See ErrorBase
    const char* getLocalText( uint8_t localCategoryErrorNumber ) const noexcept
    {
        better_enums::optional<ERR_ENUM> maybe = ERR_ENUM::_from_integral_nothrow( localCategoryErrorNumber );
        if ( maybe )
        {
            return maybe->_to_string();
        }
        else
        {
            return nullptr;
        }
    }

};

/** This class is the 'root' of the Error Category tree
 */
class ErrorCategoryRoot : public ErrorBase
{
public:
    /// Constructor. 
    ErrorCategoryRoot();

    /** Constructor.  This constructor is used when the root is STATICALLY allocated.
     */
    ErrorCategoryRoot( const char* ignoreThisParameter_usedToCreateAUniqueConstructor );

public:
    const char* getLocalText( uint8_t localCategoryErrorNumber ) const noexcept;

};



};      // end namespaces
};
#endif  // end header latch
