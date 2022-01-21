#ifndef Cpl_Layout_h_
#define Cpl_Layout_h_
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

#include "colony_config.h"


/* 1) memcpy for HA switch - # of memcpy must scale
   2) Assume same "user program". but not identical HW -->cannot use absolute addresses
   3) Inputs/Outputs to a Component must be decoupled from actual data storage -->allows for 're-wiring' without component edits
   4) "Memory" is per Virtual Chassis 
   
   5) Time? Ticks? Msec? usec? Generic type?

   6) Types: bytes, word, dword, qword, float, double, arrays of..., text (utf-8), enums, 
 Component execute method() -->takes a reference to the virtual chassis
   Impl:
   'need a virtual pointer/reference

   Mini model points
   - data
   - invalid/valid state
   - no monitors -->no seqnum -->no touch()
   - rmw - NO
   - force 
   - named? YES but not sure what this means right now
   - to/from Json? Probably
   - getSize()? Yes
   - getType()? Probably
   - import/export? NO
*/

/* 
Point Database is populate in a single thread environment
Single CPU -->all 'Virtual Chassis' execute in a single thread
fromJSON/toJSON method can only be called during 'idle time'
  lock/unlock db at the start/end of FER/execute cycle
    tshell commands block for idle time
Multi CPU
    1-to-1 mapping of Cores to number of parallel threads
        requires an assignment of Virtual chassis to threads
    problem: global Node data/ aka inter-chassis data sharing
        could be done through loop-back/bridge IO registers

IO Registers have to be thread safe
    Question is this at individual instance level - or done via 'block/scan updates'

*/
class VirtualChassis;
class TimeMsec_T;

chassis.readInput(m_inputs.abz, uint8_t& dst)
chassis.readData(m_inputs.abz, uint8_t& dst)
chassis.read(m_inputs.abz, uint8_t& dst)
chassis.writeOutput(m_outputs.xyz, uint8_t src, lockRequest = noRequest)
chassis.writeData(m_outputs.xyz, uint8_t src)

void initialize(...);
void execute(VirtualChassis& chassis, TimeMsec_T& cycleTime, TimeMSec_T& tickTime);
void shutdown(...);

ID-- > zereo based index,
static table-- > name / static info look - up
ram table-- > memory pointer

each chunk a heap (memcpy the heap)
allocate order fixed

Type, symoblic name, extra info   (header -->extern, generate ID symbol)


#define MY_COMPENENT_INPUT_XYZ      MY_COMPENENT_INPUT_XYZ_map // won't work - because I have N instance of a component
/// a list of IDs
typedef struct
{
    struct inputs_tag
    {
        Uint8_Id   x;
        Int8_Id    y;
    } inputs;
    struct output_tag
    {
        Uint8_Id  x;
        Int8_Id   y;
    } outputs;
} MyIO_T;


MyIO_T pid1 = { { VINPUT_P0.1, VINPUT_P0_2 }, {VOUTPUT_P0_1, VOUTPUT_P0_1} };


/// 
///
typedef enum {
    UINT8,
    UINT16
} PointType_T;
typedef uint32_t Identifier_T;

#include "Cpl/Container/MapItem.h"
#include "Cpl/Container/Dictionary.h"
#include "Cpl/Memory/Allocator.h"
#include "Cpl/Point/DatabaseApi.h"

// Create Function
typedef MyConcretePoint* (*CreatePointFunc)(Cpl::Memory::Allocator& allocatorForPoints, uint32_t pointId);


class PointDescriptor : public Cpl::Container::DictItem
{
protected:
    CreatePointFunc m_createMethod;          // 'Factory' method to create the point of the correct type

    uint32_t        m_localId;               // Is the 'key' for the Dictionary.. The local ID is assigned/created by the 'user' (in theory at run time).
                                             // The dictionary is used to map the user defined ID to actual Point ID.  
                                             // The construction and look-up of Points is done will the control is in the offline mode (i.e. not executing Logic chains)

    void*        m_initialScalarVal;         // If zero -->not used -->mutually exclusive
    const char*  m_initialComplexJsonVal;    // If zero -->not used -->mutually exclusive
    bool         m_initialInvalid;           // If false ignored, else auto-initial value is invalid -->mutually exclusive. 
    
    Identifier_T m_pointId;                  // 'output': Is assigned when the Point is created
};


class PointBank : public Cpl::Container::Dictionary<PointDescriptor>
{
public:
    /// Allocates a Point for every descriptor
    bool createPoints( PointDescriptor* listOfDescriptors, Cpl::Memory::Allocator& allocatorForPoints, Cpl::Point::DatabaseApi& dbForPoints, uint32_t initialPointIdValu=0 );
}


// An IO Card will contain one more PointBank and for each Point bank there is private Database instance
// The IO card logic accesses Point instances (where are really its IO Registers) via its Point Database(s)


    typedef struct
    {
        int x;
        int y;
    } InputXyz_T;

    typedef struct
    {
        int a;
        int b;
    } InputAbc_T;

    typedef struct
    {
        InputXyz_T  xyz;
        InputAbc_T  abc;
        int         j;

    } Inputs_T;

    void foo( const Inputs_T& input )
    {
        input.xyz.x = 10;
    }
};

};      // end namespaces
#endif  // end header latch

pt::Floor2::TempControl
    -->symoblic pt namespaces 
    -->actual values O to N
Node
    VChassis
        Logic Chain