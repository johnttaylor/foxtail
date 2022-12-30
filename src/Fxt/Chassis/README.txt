/** @namespace Fxt::Chassis

The 'Chassis' namespace provide the framework for defining and creating a Foxtail
Chassis.  A Chassis provides the execution timing and is responsible for 'scanning'
IO cards and executing Logic Chains

NOTES:

- Each Chassis has unique local ID.

- Chassis are contained within a Node

- Chassis instances should map 1-to-1 with CPU cores.  Ideally a Node
  is at minimum a 2 core system where 1 core runs a single Chassis and the
  second core handles all of the node's remaining processing.  A two Chassis 
  Node would be at least 3 cores
  
- Chassis are dynamically allocated/destroyed when their containing Node is 
  provisioned.

*/ 

  