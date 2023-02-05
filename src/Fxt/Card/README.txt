/** @namespace Fxt::Card

The 'Card' namespace provide the framework for defining and creating Foxtail
IO Cards.  IO Cards sample/drive the "physical" input/outputs of the system.

NOTES:

- Each IO Card Type has a GUID that is used to identify/create a the Card.  GUIDs
  are self-assigned by the developer/implementor of the card.

- For each IO Card, there is a 'factory' class that knows how to create an the
  type specific card.

- A set of IO Registers (aka Points) are used to latch/buffer a card's 
  inputs/outputs. The IO registers are NOT thread safe, but are only updated via 
  RegisterBank block copy operations.  The IO card itself maintains it own 
  internal points that are the src/dst for the block copy operations. 
  
  - IO cards are responsible for creating the following Points:
    o Internal Points that are used to transfer data to/from the IO registers.
        - These points do not have a public 'Descriptor'
    o IO Registers
        - These points do not have a public 'Descriptor'
    o Virtual Points that are used to transfer data to/from the IO registers
        - These points do HAVE a public 'Descriptor'

- The mapping of Internal Point <-> IO Register <-> Virtual Point is 1-to-1.
  
- IO Cards are dynamically allocated/destroyed when their containing Node is 
  provisioned.


FUTURE:
    Depending on the functionality of an IO Card - it may be desirable for the
    IO card to 'execute' in a different thread/core.  In theory this can be done
    by the following:

    - Create a interface that provides a reference to the MailboxServer/Runnable-object
      in which the card should execute in.  This interface should also provide a
      mutex (for implementing a critical section between the card thread and
      the chassis thread).
        o This interface needs to be available when the constructor of the IO card
          class executes. In theory since the constructor is only called at 
          run time when the node/chassis is being started - this shouldn't be
          overly burdensome.

    - In the concrete IO Card class:
        o Change the start()/stop() methods from simple function calls to 
          synchronous ITC calls
        o wrap the scanInputs()/flushOutputs() methods in a critical section
        o the collection of input 'data' should be done in IO card thread
          and stored in the card's input IO registers
        o How the output 'data' is translated from the output IO registers to
          physical signals is up to the card implementation.  One option is that
          it can be done directly in the flushOutputs() method ASSUMING there
          are no dependency on code running in the card's thread (in which case
          not critical section is needed). 
       
*/ 

  