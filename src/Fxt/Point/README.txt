/** @namespace Fxt::Point

The 'Point' namespace is the root namespace for a framework of a 'Polled' 
variant of the Data Model architecture pattern. Essential the Fxt::Point 
namespace is a 'fork' of the Cpl::Point namespace that has been customized to 
needs of the Foxtail environment.  The main differences are: 1) Foxtail
dynamically creates all of it Points, and 2) Foxtail manages the Points' state 
data such that is can be efficiently block-copied to support High Availability
(HA) architecture.

*/ 

  