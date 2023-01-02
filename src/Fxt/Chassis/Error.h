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
    @param LOGIC CHAIN FAILED_START         One or more Logic Chains failed to start
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
    @param NO_MEMORY_EXECUTION_SET          Unable to allocate memory for the Execution Set instance
    @param FAILED_CREATE_CARD               Failed to create one or more IO Cards
    @param FAILED_CREATE_LOGIC_CHAINS       Failed to create one or more Logic Chains
    @param CARD_CREATE_ERROR                One or more IO Cards were not successfully created
    @param LOGIC_CHAIN_CREATE_ERROR         One or more Logic CHains were not successfully created
    @param SCANNER_MISSING_SRM              A Scanner's JSON object is missing the ScanRateMultiplier key/value pair
    @param EXECUTION_SET_MISSING_ERM        A ExecutionSet's JSON object is missing the ExecutionRateMultiplier key/value pair
    @param LOGIC_CHAIN_FAILURE              One or more Logic Chains failed during execution
    @param FAILED_POINT_RESOLVE             One or more Components failed when resolving their' point references
    @param NO_MEMORY_SCANNER_LIST           Unable to allocate memory for list of Scanners
    @param NO_MEMORY_EXESET_LIST            Unable to allocate memory for list of Execution Sets
    @param NO_MEMORY_PERIOD_LISTS           Unable to allocate memory for list of schedulable periods
    @param MISSING_EXECUTION_SETS           At least one or more ExecutionSets where not added to the Chassis (as defined by the number specified in the Chassis constructor)
    @param MISSING_SCANNERS                 At least one or more Scanners where not added to the Chassis (as defined by the number specified in the Chassis constructor)
    @param SCANNER_FAILED_START             One or more Scanners failed to start
    @param EXECUTION_SET_FAILED_START       One or more ExecutionSets failed to start
    @param TOO_MANY_SCANNERS                Attempted to add more Scanners that what was specified when the Chassis was constructed
    @param TOO_MANY_EXECUTION_SETS          Attempted to add more ExecutionSets that what was specified when the Chassis was constructed
    @param PARSE_EXECUTION_SET_ARRAY        JSON input does not contain "executionSet"/array
    @param PARSE_SCANNER_ARRAY              JSON input does not contain "scanner"/array
    @param MISSING_FER                      A Chassis's JSON object is missing the 'fer' key/value pair
    @param NO_MEMORY_CHASSIS                Unable to allocate memory for the Chassis instance
    @param SCANNER_CREATE_ERROR             One or more Scanners were not successfully created
    @param FAILED_CREATE_SCANNER            Failed to create one or more Scanners
    @param EXESET_CREATE_ERROR              One or more ExecutionSets were not successfully created
    @param FAILED_CREATE_EXESET             Failed to create one or more ExecutionSets
    @param FAILED_CREATE_SHARED_POINTS      Failed to create one or more Shared Points
    @param FAILED_SCANNER_SCHEDULE_BUILD    Failed to build the list of Periods for the Scanner schedules
    @param FAILED_EXESET_SCHEDULE_BUILD     Failed to build the list of Periods for the ExecutionSet schedule
    @param NO_MEMORY_SHARED_PTS_LIST        Unable to allocate memory for list of Shared Points
    @param TOO_MANY_SHARED_PTS              Attempted to add more Shared Points that what was specified when the Chassis was constructed
    @param MISSING_SHARED_PTS               At least one or more Shared Points where not added to the Chassis (as defined by the number specified in the Chassis constructor)
 */
BETTER_ENUM( Err_T, uint8_t
             , SUCCESS = 0
             , NO_MEMORY_CARD_LIST
             , NO_MEMORY_LOGIC_CHAIN_LIST
             , CARD_FAILED_START
             , LOGIC_CHAIN_FAILED_START
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
             , NO_MEMORY_EXECUTION_SET
             , FAILED_CREATE_CARD
             , FAILED_CREATE_LOGIC_CHAIN
             , CARD_CREATE_ERROR
             , LOGIC_CHAIN_CREATE_ERROR
             , SCANNER_MISSING_SRM
             , EXECUTION_SET_MISSING_ERM
             , LOGIC_CHAIN_FAILURE
             , FAILED_POINT_RESOLVE
             , NO_MEMORY_SCANNER_LIST 
             , NO_MEMORY_EXESET_LIST  
             , NO_MEMORY_PERIOD_LISTS     
             , MISSING_EXECUTION_SETS
             , MISSING_SCANNERS
             , SCANNER_FAILED_START
             , EXECUTION_SET_FAILED_START
             , TOO_MANY_SCANNERS
             , TOO_MANY_EXECUTION_SETS
             , PARSE_EXECUTION_SET_ARRAY
             , PARSE_SCANNER_ARRAY
             , MISSING_FER
             , NO_MEMORY_CHASSIS
             , SCANNER_CREATE_ERROR
             , FAILED_CREATE_SCANNER
             , EXESET_CREATE_ERROR
             , FAILED_CREATE_EXESET
             , FAILED_CREATE_SHARED_POINTS
             , FAILED_SCANNER_SCHEDULE_BUILD
             , FAILED_EXESET_SCHEDULE_BUILD
             , NO_MEMORY_SHARED_PTS_LIST
             , TOO_MANY_SHARED_PTS
             , MISSING_SHARED_PTS
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
