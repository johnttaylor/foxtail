#ifndef Fxt_Chassis_ScannerApi_h_
#define Fxt_Chassis_ScannerApi_h_
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


#include "Fxt/Point/FactoryDatabaseApi.h"
#include "Fxt/Point/DatabaseApi.h"
#include "Fxt/Card/Api.h"
#include "Fxt/Card/FactoryDatabaseApi.h"
#include "Cpl/Json/Arduino.h"
#include "Cpl/Memory/ContiguousAllocator.h"

///
namespace Fxt {
///
namespace Chassis {


/** This abstract class defines the operations that can be performed on
    a Scanner.  A Scanner is collection of IO Cards where all of the contained
    cards are 'scanned' at the same frequency/rate.

    NOTE: Scanner semantics are NOT thread-safe.

   \code
    Required/Defined JSON fields/structure:
        {
          "name":                   "*<human readable name for the Scanner - not required to be unique>",
          "id":                     <*Local ID for the Scanner.  Range: 0-64K>,
          "scanRateMultiplier": 1,  <Scan Rate Multiplier (i.e. the scanner executes every: (multiplier * chassis.fer) microseconds>,
          "cards": [                // List of IO Cards  (must be at least one)
            {...},
            ...
          ]
        }

    *The field is NOT parsed/used by the firmware

    \endcode
 */
class ScannerApi
{
public:
    /// This method returns the Scanner' Scan Rate Multiplier
    virtual size_t getScanRateMultiplier() const noexcept = 0;

public:
    /** This method is used to start/activate the Scanner.  If the scanner fails
        to be started the method returns false; else true is returned.  Each
        Scanner is responsible for starting its contained IO Cards

        The 'currentElapsedTimeUsec' argument represents the current elapsed
        time in microseconds since power-up of the application.

        A scanner can be started/stopped multiple times. When a scanner is created
        it is in the 'stopped' state.
     */
    virtual bool start( uint64_t currentElapsedTimeUsec ) noexcept = 0;

    /** This method is used to stop/deactivate the Scanner.  Each Scanner is
        responsible for stopping its contained IO Cards

        A Scanner MUST be in the 'stopped state' before it can be deleted/destroyed
     */
    virtual void stop() noexcept = 0;

    /** This method returns true if the Scanner is in the started state
     */
    virtual bool isStarted() const noexcept = 0;

public:
    /** This method cause the contained IO cards to update the their virtual
        input points (associated with the card) from the content of the IO
        Registers.

        The method return false if unrecoverable error occurred; else true
        is returned.
     */
    virtual bool scanInputs() noexcept = 0;

    /** This method causes the contained IO Cards to flush their virtual
        output points (associated with the card) to the card's IO Registers

        The method return false if unrecoverable error occurred; else true
        is returned.
     */
    virtual bool flushOutputs() noexcept = 0;

public:
    /** This method returns the current error state of the Scanner.  A value
        of Fxt::Type::Err_T::SUCCESS indicates the card is operating properly
     */
    virtual Fxt::Type::Error getErrorCode() const noexcept = 0;

public:
    /** This method is used to add a IO Card to the Scanner.  If the add is
        successful then Fxt::Type::Err_T::SUCCESS is returned; else and error
        code is returned.
     */
    virtual Fxt::Type::Error add( Fxt::Card::Api& cardToAdd ) noexcept = 0;

public:
    /** This method attempts to parse the provided JSON Object that represents
        a Scanner and create the contained IO cards.  If there is an error
        (e.g. IO card not supported, missing key/value pairs, etc.) the
        method returns nullptr; else a pointer to the newly created Scanner
        object is returned.  When an error occurs, details about the specific
        Scanner error is returned via 'scannerErrorode' argument.
     */
    static ScannerApi* createScannerfromJSON( JsonVariant                         scannerJsonObject,
                                              Cpl::Memory::ContiguousAllocator&   generalAllocator,
                                              Cpl::Memory::ContiguousAllocator&   cardStatefulDataAllocator,
                                              Cpl::Memory::ContiguousAllocator&   haStatefulDataAllocator,
                                              Fxt::Card::FactoryDatabaseApi&      cardFactoryDb,
                                              Fxt::Point::FactoryDatabaseApi&     pointFactoryDb,
                                              Fxt::Point::DatabaseApi&            dbForPoints,
                                              Fxt::Type::Error&                   scannerErrorode ) noexcept;
public:
    /// Virtual destructor
    virtual ~ScannerApi(){};
};


};      // end namespaces
};
#endif  // end header latch
