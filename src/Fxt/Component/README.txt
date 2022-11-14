/** @namespace Fxt::Component

The 'Component' namespace provide the framework for defining and creating Foxtail
Components.  Each component has zero or more inputs, and zero or more outputs. 
A component itself performs some type of operation(s) to transform its inputs to
its outputs. A Component is the fundamental unit of execution when building
logic chains.

NOTES:

- Each Component has a GUID that is used to identify/create an instance of the
  Component. GUIDs are self-assigned by the developer/implementor of the 
  component.

- For each Component, there is a 'factory' class that knows how to create the
  type specific Component.

- A Component can have 'stateful' data that persists between executing cycles.

- Components are EXPECTED to play nice an not consume large amounts of CPU time
  when executing.  This is because the execution of Components is essential done
  via cooperating scheduling in a single thread.

- Component instances are dynamically allocated/destroyed when their containing 
  Node is provisioned.

*/ 

  