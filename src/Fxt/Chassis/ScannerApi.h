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


#include "Fxt/Point/DatabaseApi.h"
#include "Cpl/Json/Arduino.h"
#include "Cpl/Memory/ContiguousAllocator.h"

///
namespace Fxt {
///
namespace Chassis {


/** This abstract class defines the operations that can be performed on
    a Scanner
 */
class ScannerApi
{
public:
    /** This method is used to start/activate the Scanner.  If the scanner fails
        to be started the method returns false; else true is returned.  Each
        Scanner is responsible for starting its contained IO Cards

        A scanner can be started/stopped multiple times. When a scanner is created
        it is in the 'stopped' state.
     */
    virtual bool start() noexcept = 0;

    /** This method is used to stop/deactivate the Scanner.  If the Scanner fails
        to be stopped the method returns false; else true is returned. Each
        Scanner is responsible for stopping its contained IO Cards

        A Scanner MUST be in the 'stopped state' before it can be deleted/destroyed
     */
    virtual bool stop() noexcept = 0;

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
    /** This method attempts to parse the provided JSON Object that represents
        a Scanner and create the contained IO cards.  If there is an error
        (e.g. IO card not supported, missing key/value pairs, etc.) the
        method returns nullptr; else a pointer to the newly created Scanner
        object is returned.  When an error occurs, details about the specific
        Scanner error is returned via 'scannerErrorode' argument.
     */
    static Api* createScannerfromJSON( JsonVariant                         scannerJsonObject,
                                       Cpl::Memory::ContiguousAllocator&   generalAllocator,
                                       Cpl::Memory::ContiguousAllocator&   cardStatefulDataAllocator,
                                       Cpl::Memory::ContiguousAllocator&   haStatefulDataAllocator,
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
