/** @namespace Fxt

The 'Fxt' namespace is the root namespace for the Foxtail package

Foxtail is an attempt at a Distributed Control System (DCS).  Given that is
is extremely ambition scope and this is not my day job - its going to be a
while ;-). 

The following features are planned for Foxtail (in no particular order):

o Hardware agnostic
o High Available, i.e. supports a real-time switch over to redundant controller 
  hardware.
o Control Logic is implemented as a collection of 'Components' that are logically
  wired together.
    o The Control logic can be downloaded at run time
    o Users can create their own customized components (at compile time)
o Execution Rate of the Control logic is typically 1KHz (depending on HW 
  resources).  Note: Foxtail does NOT attempt to play/compete in the PLC space
  with respect to real-time control performance).
o Scalable to a large number of IO points (i.e. logical addressing for IO points 
  within a given Foxtail system instance is 64bits).



POINT FEATURES:
---------------
o Points are used the principal interface between Components
  and Virtual IO for the Foxtail DCS.
o The execution/access paradigm for Points is 'polled', i.e. the assumption is 
  made that the consumers of Points execute on a periodic basis.  There are no
  event driven APIs/semantics with respect to Points.


THREADING MODEL:
----------------
The Point sub-system works on a single thread model, i.e. thread safe is pushed
to higher layer(s) of the system.  The basic assumptions, design choices,
characteristics of the threading model are:

o Points are consumed/used by components in Logic Chains.  Logic chains execute
  in the context of Virtual Chassis. Each Virtual chassis executes in a single
  thread.  A given Point is scoped to a single Virtual Chassis.
  Note: For single core MCUs - all Virtual Chassis execute in a single/same
        thread.  This design maximize the real time performance of Virtual
        Chassis. For multi-core MCUs - individual Virtual Chassis can be 
        assigned to different threads.  However, the maximum number of 
        "Chassis threads" is number of cores on the MCU.

o Data (aka Point data) can be shared across Virtual Chassis, however this is
  done via 'Loopback IO' - which is transparent/separate from the Point mechanism

o External or debug access to Points is allowed - but only when no Logic chain
  is executing on a given Virtual Chassis.  The external access is done via
  a synchronous blocking interface

o The Point database is created/populated in a single threaded environment


*/ 

  