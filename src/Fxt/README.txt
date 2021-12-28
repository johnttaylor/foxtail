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


*/ 

  