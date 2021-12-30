/** @namespace Cpl::Point

The 'Point' namespace is the root namespace for a framework of a 'Polled' 
variant of the Data Model architecture pattern.  

The Data Model software architecture pattern is a data-oriented pattern where
modules interact with each other via data instances (a.k.a. model points) with
no direct dependencies between modules.  The Polled variant of the pattern is 
one that assumes a single threaded model and there are no event/change 
notification features.

POINT FEATURES:
---------------
- The value stored in a model point can be of any type or any data
  structure. A model point’s value should be strictly data and not an
  object. That is, model points do not contain business rules or enforce
  policies (except for things like value range checking).

- Model point instances are type safe. That is, all read and write
  operations are specific to the model point’s value type.

- Points have unique (to the application) numeric identifier.  Points can
  be 'looked-up' from the Point Database using their identifier.  Note: The
  look-up is a O(1) operation.

- A Symbolic name (aka a text label) can be associated with the Point.

- Model points have atomic operations for accessing their value or
  state. 

- Model points have a valid or invalid state independent of their value.

- The execution/access paradigm for Points is 'polled', i.e. the assumption is 
  made that the consumers of Points execute on a periodic basis.  There are no
  event driven APIs/semantics with respect to Points.

- Points can be 'locked'.  When point is in the locked state all write 
  operations will silently fail. This is a very useful feature when testing 
  because the tester can override/force a Point to a specific value regardless
  of how frequently the application is updating the Point.

- Points can be serialized/deserialized to/from JSON string

*/ 

  