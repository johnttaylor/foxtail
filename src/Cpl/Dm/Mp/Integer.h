#ifndef Cpl_Dm_Mp_Integer_h_
#define Cpl_Dm_Mp_Integer_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */


#include "Cpl/Dm/ModelPointCommon_.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/FString.h"
#include <string.h>


///
namespace Cpl {
///
namespace Dm {
///
namespace Mp {


/** This template class provides a mostly concrete implementation for a Model
    Point who's data is a C integer primitive type of type: 'ELEMTYPE'.

    NOTES:
        1) All methods in this class are NOT thread Safe unless explicitly
        documented otherwise.
 */
template<class ELEMTYPE, class MPTYPE>
class Integer : public Cpl::Dm::ModelPointCommon_
{
protected:
    /// The element's value
    ELEMTYPE    m_data;

public:
    /// Constructor: Invalid MP
    Integer( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        :Cpl::Dm::ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), false )
    {
    }

    /// Constructor: Valid MP (requires initial value)
    Integer( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, ELEMTYPE initialValue )
        :Cpl::Dm::ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), true )
    {
        m_data = initialValue;
    }

public:
    /// Type safe read. See Cpl::Dm::ModelPoint
    inline bool read( ELEMTYPE& dstData, uint16_t* seqNumPtr = 0 ) const noexcept
    {
        return Cpl::Dm::ModelPointCommon_::read( &dstData, sizeof( ELEMTYPE ), seqNumPtr );
    }

    /// Type safe write. See Cpl::Dm::ModelPoint
    inline uint16_t write( ELEMTYPE newValue, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        return Cpl::Dm::ModelPointCommon_::write( &newValue, sizeof( ELEMTYPE ), lockRequest );
    }

    /// Atomic increment
    inline uint16_t increment( ELEMTYPE incSize = 1, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.lock_();
        uint16_t result = write( m_data + incSize, lockRequest );
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.unlock_();
        return result;
    }

    /// Atomic decrement
    inline uint16_t decrement( ELEMTYPE decSize = 1, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.lock_();
        uint16_t result = write( m_data - decSize, lockRequest );
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.unlock_();
        return result;
    }


    /// Type safe register observer
    inline void attach( Cpl::Dm::Subscriber<MPTYPE>& observer, uint16_t initialSeqNumber = SEQUENCE_NUMBER_UNKNOWN ) noexcept
    {
        ModelPointCommon_::attach( observer, initialSeqNumber );
    }

    /// Type safe un-register observer
    inline void detach( Cpl::Dm::Subscriber<MPTYPE>& observer ) noexcept
    {
        ModelPointCommon_::detach( observer );
    }


protected:
    /// See Cpl::Dm::Point.  
    void setJSONVal( JsonDocument& doc ) noexcept
    {
        doc["val"] = m_data;
    }

public:
    /// See Cpl::Dm::Point.  
    bool fromJSON_( JsonVariant& src, Cpl::Dm::ModelPoint::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
    {
        if ( src.is<ELEMTYPE>() )
        {
            retSequenceNumber = write( src.as<ELEMTYPE>(), lockRequest );
            return true;
        }
        if ( errorMsg )
        {
            *errorMsg = "Invalid syntax for the 'val' key/value pair";
        }
        return false;
    }
};


};      // end namespaces
};
};
#endif  // end header latch
