/** @namespace Fxt::Card

The 'Card' namespace provide the framework for defining and creating Foxtail
IO Cards.  IO Cards read/drive the "physical" input/outputs of the system.

NOTES:

- Each IO Card has a GUID that is used to identify/create a the Card.  GUIDs
  are self-assigned by the developer/implementor of the card.

- For each IO Card, there is a 'factory' class that knows how to create an the
  type specific card.

- A set of IO Registers (aka Points) are used to latch/buffer a card inputs/outputs.
  The IO registers are thread safe, but are only updated via RegisterBank block
  copy operations.  The IO card itself maintains it own internal points that
  are the src/dst for the block copy operations. IO cards are responsible for
  creating the following Points.  
    o Internal Points that are used to transfer data to/from the IO registers.
        - These points do not a public 'Descriptor'
    o IO Registers
        - These points do not a public 'Descriptor'
    o Virtual Points that are used to transfer data to/from the IO registers
        - These points do HAVE a public 'Descriptor'

  The IO Registers and Application Points are mapped 1-to-1 with the IO card's 
  internal points.

- IO Cards are dynamically allocated/destroyed when their containing Node is 
  provisioned.

*/ 

  