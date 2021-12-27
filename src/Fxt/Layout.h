#ifndef Fxt_Layout_h_
#define Fxt_Layout_h_
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
namespace Fxt {

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