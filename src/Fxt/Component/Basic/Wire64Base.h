#ifndef Fxt_Component_Basic_Wire64Base_h_
#define Fxt_Component_Basic_Wire64Base_h_
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
namespace Basic {


/** This partially concrete class implements a Component that is a pass-through
    connector between two points. Up to 64 Inputs/Outputs are supported

    The concrete child determines the allowed data type. For given instance
    All inputs/outputs MUST have the same type.

    Every defined input must have a defined output.  In addition the order of
    the inputs IS the order of the outputs, i.e. ORDER IS IMPORTANT!

    IF an input value is invalid, then its corresponding output value is set to
    invalid.

    The component has NO stateful data

    \code

    JSON Definition
    --------------------
    {
       "name": "connection#1"                               // Text label for the component
       "type": "e6759a22-06c1-4aad-8190-67bf36425903",      // Identifies the card type.  Value comes from the Supported/Available-card-list
       "typeName": "Fxt::Component::Basic::Wire64"          // OPTIONAL: Human readable type name
       "inputs": [                                          // Order of the inputs MUST match the order of the outputs.  The max number of inputs is determined by the child class
          {
            "name": "Signal A",                             // human readable name for the input value
            "type": "708745fa-cef6-4364-abad-063a40f35cbc", // REQUIRED Type for the input signal
            "typeName": "Fxt::Point::Float",                // OPTIONAL: Human readable Type name for the input signal. The type is child class specific!
            "idRef": 4294967295                             // Point ID Reference to the point to read the input value from
          },
          ...
       ],
       "outputs": [                                         // Order of the outputs MUST match the order of the inputs.  The max number of outputs is determined by the child class
          {
            "name":"Signal AA"                              // Human readable name for the output signal
            "type": "708745fa-cef6-4364-abad-063a40f35cbc", // REQUIRED Type for the output signal
            "typeName": "Fxt::Point::Float",                // OPTIONAL: Human readable Type name for the output signal
            "idRef":4294967295,                             // Point ID reference to the point that is updated with the output value
          },
          ...
       ]
    }


    \endcode
 */
class Wire64Base : public Fxt::Component::Common_
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
    Wire64Base();

    /// Destructor
    ~Wire64Base();

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


    /// Helper method that returns the point type's GUID
    virtual const char* getPointTypeGuid() const noexcept = 0;

    /// Copy the data+state from the input point to the output point.  
    virtual void copyPoint( unsigned refIndex ) noexcept = 0;
};



};      // end namespaces
};
};
#endif  // end header latch