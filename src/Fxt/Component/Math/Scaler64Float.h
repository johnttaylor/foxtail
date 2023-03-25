#ifndef Fxt_Component_Analog_Scaler64Float_h_
#define Fxt_Component_Analog_Scaler64Float_h_
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


#include "Fxt/Component/Math/Scaler64Base.h"
#include "Fxt/Point/Float.h"

///
namespace Fxt {
///
namespace Component {
///
namespace Math {


/** This concrete class implements a Component that scales an float value
    (using a simple mx + b formula. Up to 64 inputs/output pairs
    are supported.
 */
class Scaler64Float : public Scaler64Base
{
public:
    /// Type ID for the card
    static constexpr const char*    GUID_STRING = "0eb51702-677f-4022-91ab-bc84efcc4ed1";

    /// Type name for the card
    static constexpr const char*    TYPE_NAME   = "Fxt::Component::Math::Scaler64Float";

    /// Size (in bytes) of Stateful data that will be allocated on the HA Heap
    static constexpr const size_t   HA_STATEFUL_HEAP_SIZE = 0;

public:
    /// Constructor
    Scaler64Float( JsonVariant&                       componentObject,
                   Cpl::Memory::ContiguousAllocator&  generalAllocator,
                   Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                   Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                   Fxt::Point::DatabaseApi&           dbForPoints )
        :Scaler64Base()
    {
        parseConfiguration( generalAllocator, componentObject );
    }

public:
    /// See Fxt::Component::Api
    const char* getTypeGuid() const noexcept
    {
        return GUID_STRING;
    }

    /// See Fxt::Component::Api
    const char* getTypeName() const noexcept
    {
        return TYPE_NAME;
    }


protected:
    /// See Fxt::Component::Math::ScalerBase
    bool allocateKonstants( Cpl::Memory::ContiguousAllocator& generalAllocator, unsigned numPoints ) noexcept
    {
        m_konstants = (Konstants_T*) generalAllocator.allocate( sizeof( Konstants_T ) * numPoints );
        return m_konstants != nullptr;
    }

    /// See Fxt::Component::Math::ScalerBase
    bool parseJsonKonstants( unsigned ptIdx, JsonObject& elem ) noexcept
    {
        if ( elem["m"].is<float>() && elem["b"].is<float>() )
        {
            m_konstants[ptIdx].m = elem["m"];
            m_konstants[ptIdx].b = elem["b"];
            return true;
        }
        return false;
    }

    /// See Fxt::Component::Math::ScalerBase
    bool validateInputTypes() noexcept
    {
        return Fxt::Point::Api::validatePointTypes( (Fxt::Point::Api **) m_inputRefs, m_numInputOuputPairs, Fxt::Point::Float::GUID_STRING );
    }

    /// See Fxt::Component::Math::ScalerBase
    bool validateOututTypes() noexcept
    {
        return Fxt::Point::Api::validatePointTypes( (Fxt::Point::Api **) m_outputRefs, m_numInputOuputPairs, Fxt::Point::Float::GUID_STRING );
    }

    /// See Fxt::Component::Math::ScalerBase
    bool calculateOutput( unsigned refIndex ) noexcept
    {
        Fxt::Point::Float* inPt  = (Fxt::Point::Float*) m_inputRefs[refIndex];
        Fxt::Point::Float* outPt = (Fxt::Point::Float*) m_outputRefs[refIndex];

        float inVal;
        if ( inPt->read( inVal ) )
        {
            float output = inVal * m_konstants[refIndex].m + m_konstants[refIndex].b;
            outPt->write( output );
            return true;
        }

        return false;
    }

protected:
    /// Struct to hold the scaling constants
    struct Konstants_T
    {
        float m;        //!< 'm' constant in the mx+b formula
        float b;        //!< 'b' constant in the mx+b formula
    };

    /// List of Scaling constants
    Konstants_T*        m_konstants;
};



};      // end namespaces
};
};
#endif  // end header latch