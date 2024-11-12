# Utilities for my homebrew 8 bit computer.
- EEPROM programmer (you have to configure the pins yourself)
- A basic Assembler for custom instruction set.
# 8 bit computer specs
- 2 registers (8 bit)
- 16 bit program counter
- 8 bit bus
- an ALU with ADD/SUB insctructions
- Jump instruction
- 128 bits of ram (4 bit address and 8 bit data)

#TODO  (to make it turing complete.  probably will continue developing after YKS)
- add flags
- add condiditonal jump
- maybe a basic display (not related to other todos)


i learned all this from [Ben Eater](https://eater.net/).
thanks Ben Eater

The register and ALU part is mostly the same as the Ben Eater's one.
But for the control unit i had to adapt to the chips at hand so that part is my design.


Picture of the computer in its current state:
![pc](https://github.com/user-attachments/assets/e1f0dfb9-1e9b-4a5a-8c95-9783dee6444c)
There are some random components on the board which are not a part of the computer.
I didnt bother to put them in the storage box.
