#ifndef Fxt_Component_Math_Scaler64Base_h_
#define Fxt_Component_Math_Scaler64Base_h_
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
#include "Fxt/Point/Api.h"
#include "Fxt/Point/BankApi.h"

///
namespace Fxt {
///
namespace Component {
///
namespace Math {


/** This partially concrete class implements a Component that scales an value
    using a simple mx + b formula. Upto 64 Inputs/Outputs are supported

    The concrete child determines the allowed data type. All inputs/outputs 
    MUST have the same type.
    
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
       "typeName": "Fxt::Component::Analog::Scaler64Float"  // OPTIONAL: Human readable type name
       "inputs": [                                          // Order of the inputs MUST match the order of the outputs.  The max number of inputs is determined by the child class
          {                                             
            "name": "Signal A",                             // human readable name for the input value
            "type": "708745fa-cef6-4364-abad-063a40f35cbc", // REQUIRED Type for the input signal
            "typeName": "Fxt::Point::Float",                // OPTIONAL: Human readable Type name for the input signal. The type is child class specific!
            "m": <numeric>,                                 // 'm' constant in the mx+b formula
            "b": <numeric>,                                 // 'b' constant int the mx+b formula
            "idRef": 4294967295                             // Point ID Reference to the point to read the input value from
          },
          ...
       ],
       "outputs": [                                         // Order of the outputs MUST match the order of the inputs.  The max number of outputs is determined by the child class
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
class Scaler64Base : public Fxt::Component::Common_
{
public:
    /// Maximum number of Input signals
    static constexpr unsigned       MAX_INPUTS = 64;

    /// Maximum number of Output signals (MUST be the same a max inputs)
    static constexpr unsigned       MAX_OUTPUTS = MAX_INPUTS;

public:
    /// Size (in bytes) of Stateful data that will be allocated on the HA Heap
    static constexpr const size_t   HA_STATEFUL_HEAP_SIZE = 0;

protected:
    /// Constructor
    Scaler64Base();

    /// Destructor
    ~Scaler64Base();

public:
    /// See Fxt::Component::Api
    Fxt::Type::Error resolveReferences( Fxt::Point::DatabaseApi& pointDb )  noexcept;

    /// See Fxt::Component::Api
    Fxt::Type::Error execute( int64_t currentTickUsec ) noexcept;

protected:
    /// Helper method to parse the card's JSON config
    bool parseConfiguration( Cpl::Memory::ContiguousAllocator& generalAllocator,
                             JsonVariant&                      obj,
                             unsigned                          minInputs,
                             unsigned                          maxInputs,
                             unsigned                          minOutputs,
                             unsigned                          maxOutputs ) noexcept;

    /// Helper method that allocates memory for the type specific constants
    virtual bool allocateKonstants( Cpl::Memory::ContiguousAllocator& generalAllocator, unsigned numPoints ) noexcept = 0;

    /// Helper method to parse Kconstants
    virtual bool parseJsonKonstants( unsigned ptIdx, JsonObject& elem ) noexcept = 0;

    /// Helper method that performs the actual math. Return false if the input is invalid
    virtual bool calculateOutput( unsigned refIndex ) noexcept = 0;

    /// Helper method that validates the input/output types
    virtual bool validateInputTypes() noexcept = 0;

    /// Helper method that validates the input/output types
    virtual bool validateOututTypes() noexcept = 0;


protected:
};



};      // end namespaces
};
};
#endif  // end header latch