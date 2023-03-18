#ifndef Fxt_Component_Analog_SimpleScaler8_h_
#define Fxt_Component_Analog_SimpleScaler8_h_
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


#include "Fxt/Component/Common_.h"
#include "Cpl/Json/Arduino.h"
#include "Fxt/Point/Float.h"
#include "Fxt/Point/BankApi.h"

///
namespace Fxt {
///
namespace Component {
///
namespace Analog {


/** This concrete class implements a Component that scales an analog value
    (aka a float) using a simple mx + b formula. Up to 8 inputs/output pairs
    are supported.

    Every defined input must have a defined output.  In addition the order of
    the inputs IS the order of the outputs, i.e. ORDER IS IMPORTANT!

    IF an input value is invalid, then its corresponding output value is set to
    invalid.

    The component has NO stateful data

    \code

    JSON Definition
    --------------------
    {
       "name": "mx+b Scaler"                                // Text label for the component
       "type": "0eb51702-677f-4022-91ab-bc84efcc4ed1",      // Identifies the card type.  Value comes from the Supported/Available-card-list
       "typeName": "Fxt::Component::Analog::SimpleScaler8"  // OPTIONAL: Human readable type name
       "inputs": [                                          // Array of Point references that supply the Component's input values.  Number of elements: 1-16
          {
            "name": "Signal A",                             // human readable name for the input value
            "type": "708745fa-cef6-4364-abad-063a40f35cbc", // REQUIRED Type for the input signal
            "typeName": "Fxt::Point::Float",                // OPTIONAL: Human readable Type name for the input signal
            "m": <float>,                                   // 'm' constant in the mx+b formula
            "b": <float>,                                   // 'b' constant int the mx+b formula
            "idRef": 4294967295                             // Point ID Reference to the point to read the input value from
          },
          ...
       ],
       "outputs": [                                         // Array of Point reference where the Component's writes it output values to.  Number of elements: 1-2
          {
            "name":"Scaled Signal A"                        // Human readable name for the output signal
            "type": "708745fa-cef6-4364-abad-063a40f35cbc", // REQUIRED Type for the output signal
            "typeName": "Fxt::Point::Float",                // OPTIONAL: Human readable Type name for the output signal
            "idRef":4294967295,                             // Point ID reference to the point that is updated with the output value
          },
          ...
       ]
    }


    \endcode
 */
class SimpleScaler8 : public Fxt::Component::Common_
{
public:
    /// Type ID for the card
    static constexpr const char*    GUID_STRING = "0eb51702-677f-4022-91ab-bc84efcc4ed1";

    /// Type name for the card
    static constexpr const char*    TYPE_NAME   = "Fxt::Component::Analog::SimpleScaler8";

    /// Size (in bytes) of Stateful data that will be allocated on the HA Heap
    static constexpr const size_t   HA_STATEFUL_HEAP_SIZE = 0;

public:
    /// Maximum number of Input signals
    static constexpr unsigned MAX_INPUTS = 8;

    /// Maximum number of Output signals (MUST be the same a max inputs)
    static constexpr unsigned MAX_OUTPUTS = MAX_INPUTS;

public:
    /// Constructor
    SimpleScaler8( JsonVariant&                       componentObject,
                   Cpl::Memory::ContiguousAllocator&  generalAllocator,
                   Cpl::Memory::ContiguousAllocator&  haStatefulDataAllocator,
                   Fxt::Point::FactoryDatabaseApi&    pointFactoryDb,
                   Fxt::Point::DatabaseApi&           dbForPoints );

    /// Destructor
    ~SimpleScaler8();

public:
    /// See Fxt::Component::Api
    Fxt::Type::Error resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept;

    /// See Fxt::Component::Api
    Fxt::Type::Error execute( int64_t currentTickUsec ) noexcept;

public:
    /// See Fxt::Component::Api
    const char* getTypeGuid() const noexcept;

    /// See Fxt::Component::Api
    const char* getTypeName() const noexcept;


protected:
    /// Helper method to parse the card's JSON config
    bool parseConfiguration( JsonVariant& obj ) noexcept;

protected:
    /// Struct to hold the scaling constants
    struct Konstants_T
    {
        float m;        //!< 'm' constant in the mx+b formula
        float b;        //!< 'b' constant in the mx+b formula
    };

protected:
    /// List of Input Points.  Note: Initially the point IDs are stored instead of pointers
    Fxt::Point::Float*  m_inputRefs[MAX_INPUTS];

    /// List of Output Points. Note: Initially the point IDs are stored instead of pointers
    Fxt::Point::Float*  m_outputRefs[MAX_OUTPUTS];

    /// Scaling constants
    Konstants_T         m_konstants[MAX_INPUTS];

    /// Number of Input/Output point pairs
    uint8_t             m_numInputOuputPairs;
};



};      // end namespaces
};
};
#endif  // end header latch