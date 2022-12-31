#ifndef Fxt_Chassis_Error_h_
#define Fxt_Chassis_Error_h_
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


#include "Fxt/Type/Error.h"




///
namespace Fxt {
///
namespace Chassis {

/** This enumeration defines error codes that are unique to the Chassis Sub-system

        Note: Each error code symbol must be less than 32 characters
           123456789 123456789 12345678 12

    @param Err_T                            Enum

    @param SUCCESS                          NO ERROR
    @param NO_MEMORY_CARD_LIST              Unable to allocate memory for list of contained IO cards
    @param NO_MEMORY_LOGIC_CHAIN_LIST       Unable to allocate memory for list of Logic Chains
    @param CARD_FAILED_START                One or more IO cards failed to start
    @param MISSING_CARDS                    At least one or more IO Cards where not added to a Scanner (as defined by the number specified in the Scanner constructor)
    @param MISSING_LOGIC_CHAINS             At least one or more Logic Chains where not added to the ExecutionSet (as defined by the number specified in the ExecutionSet constructor)
    @param TOO_MANY_CARDS                   Attempted to add more IO Cards that what was specified when the Scanner was constructed
    @param TOO_MANY_LOGIC_CHAINS            Attempted to add more Logic Chains that what was specified when the ExecutionSet was constructed
    @param CARD_SCAN_FAILURE                One or more IO Cards failed to scan their inputs
    @param CARD_FLUSH_FAILURE               One ore more IO Cards failed to flush their outputs
    @param PARSE_CARDS_ARRAY                JSON input does not contain "cards"/array
    @param PARSE_LOGIC_CHAIN_ARRAY          JSON input does not contain "logic chain"/array
    @param NO_CARDS                         JSON Input has an empty array of "cards"
    @param NO_LOGIC_CHAINS                  JSON Input has an empty array of "logic chains"
    @param NO_MEMORY_SCANNER                Unable to allocate memory for the Scanner instance
    @param FAILED_CREATE_CARD               Failed to create one or more IO Cards
    @param FAILED_CREATE_LOGIC_CHAINS       Failed to create one or more Logic Chains
    @param CARD_CREATE_ERROR                One or more IO Cards were not successfully created
    @param LOGIC_CHAIN_CREATE_ERROR         One or more Logic CHains were not successfully created
    @param SCANNER_MISSING_SRM              A Scanner's JSON object is missing the ScanRateMultiplier key/value pair
    @param EXECUTION_SET_MISSING_SRM        A ExecutionSet's JSON object is missing the ScanRateMultiplier key/value pair
    @param COMPONENT_FAILURE                One or more components failed during execution
    @param FAILED_START                     Unable to start one or more components
    @param FAILED_CREATE_POINTS             Failed to create one or more connection points
    @param FAILED_CREATE_AUTO_POINTS        Failed to create one or more auto points
    @param NO_INITIAL_VAL_AUTO_POINT        One or more auto points are missing the required initial value specification
    @param POINT_CREATE_ERROR               One or more connector points were not successfully created
    @param AUTO_POINT_CREATE_ERROR          One or more connector points were not successfully created
    @param FAILED_POINT_RESOLVE             One or more Components failed when resolving their' point references
 */
BETTER_ENUM( Err_T, uint8_t
             , SUCCESS = 0
             , NO_MEMORY_CARD_LIST
             , NO_MEMORY_LOGIC_CHAIN_LIST
             , CARD_FAILED_START
             , MISSING_CARDS
             , MISSING_LOGIC_CHAINS
             , TOO_MANY_CARDS
             , TOO_MANY_LOGIC_CHAINS
             , CARD_SCAN_FAILURE
             , CARD_FLUSH_FAILURE
             , PARSE_CARDS_ARRAY
             , PARSE_LOGIC_CHAIN_ARRAY
             , NO_CARDS
             , NO_LOGIC_CHAINS
             , NO_MEMORY_SCANNER
             , FAILED_CREATE_CARD
             , FAILED_CREATE_LOGIC_CHAIN
             , CARD_CREATE_ERROR
             , LOGIC_CHAIN_CREATE_ERROR
             , SCANNER_MISSING_SRM
             , EXECUTION_SET_MISSING_SRM

             , COMPONENT_FAILURE
             , FAILED_START
             , FAILED_CREATE_POINTS
             , FAILED_CREATE_AUTO_POINTS
             , NO_INITIAL_VAL_AUTO_POINT
             , POINT_CREATE_ERROR
             , AUTO_POINT_CREATE_ERROR
             , FAILED_POINT_RESOLVE
);

/** This concrete class defines the Error Category for the Logic Chain namespace.
    This class is designed as SINGLETON.
 */
class ErrCategory : public Fxt::Type::ErrorCategory<Err_T>
{
public:
    /// Constructor
    ErrCategory( Fxt::Type::ErrorBase& parent = Fxt::Type::Error::getErrorCategoriesRoot() )
        :Fxt::Type::ErrorCategory<Err_T>( "CHASSIS", 1, parent )
    {
    }

public:
    /// Singleton
    static ErrCategory g_theOne;
};

/// Helper method to build a 'complete/full' error from a specific error code
inline Fxt::Type::Error fullErr( Err_T localErr )
{
    return Fxt::Type::Error( ErrCategory::g_theOne, localErr );
}

};      // end namespaces
};
#endif  // end header latch