#ifndef Fxt_Point_Double_h_
#define Fxt_Point_Double_h_
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


#include "Fxt/Point/Basic_.h"
#include "Cpl/Memory/Allocator.h"

///
namespace Fxt {
///
namespace Point {


/** This class provides a concrete implementation for a Point who's data is a
    double.

    The toJSON()/fromJSON format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, locked:true|false, val:numericVal }

        \endcode

 */
class Double : public BasicReal_<double>
{
public:
    /// Type ID for the point
    static constexpr const char* GUID_STRING = "05e542d9-2e54-4a32-9b64-a99084b2910b";

    /// Type name for the card
    static constexpr const char* TYPE_NAME   = "Fxt::Point::Double";

public:
    /** Constructor. Invalid Point.
     */
    Double( DatabaseApi&                         db,
            uint32_t                             pointId,
            Cpl::Memory::ContiguousAllocator&    allocatorForPointStatefulData,
            Api*                                 setterPoint = nullptr )
        : BasicReal_<double>(db, pointId, allocatorForPointStatefulData, setterPoint ) {}

public:
    /// Pull in overloaded methods from base class
    using BasicReal_<double>::write;

    /// Updates the MP's data from 'src'. Note: The lock state of 'src' is NOT-USED/IGNORED
    void write( Double& src, Fxt::Point::Api::LockRequest_T lockRequest = Fxt::Point::Api::eNO_REQUEST ) noexcept
    {
        updateFrom_( &(((Basic_<double>::Stateful_T*)(src.m_state))->data), sizeof( double ), src.isNotValid(), lockRequest );
    }

    ///  See Fxt::Point::Api
    void updateFromSetter() noexcept { if ( m_setter ) { write( *((Double*) m_setter) ); } }

    ///  See Fxt::Point::Api
    void updateFromSetter( const Api&    srcPt,
                           LockRequest_T lockRequest = eNO_REQUEST,
                           bool          beSafe      = true ) noexcept
    {
        if ( !beSafe || isSameType( srcPt ) ) { write( *((Double*) &srcPt), lockRequest ); }
    }


public:
    ///  See Fxt::Point::Api
    const char* getTypeGuid() const noexcept { return GUID_STRING; }

    ///  See Fxt::Point::Api
    const char* getTypeName() const noexcept { return TYPE_NAME; }

};



};      // end namespaces
};
#endif  // end header latch
