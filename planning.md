# MicroSim

_A simple RISC-style processor instruction set, assembly code and processor architechture, partially influenced by the BBC Micro and Motorola 68k instruction sets._



Short and long term goals:
- Create instruction set
- Design basic high-level processor layout
- Create compiler for assembly to binary instructions
- Create simulator for the processor



Useful information and topics:
---

Barrel shifter:
- Used to perform extremely fast (rotation) bit shifts, of any number of bits
- Implemented in hardware, rather than by multiplying by a power of two
- Uses a decoder to activate the corresponding set of tri-state buffers
- Might be able to use this to perform both (normal) right and left shifts, if there is a way to remove the bits which wrap around (since often you want to truncate/pad with zeros)

- Resources:
  - https://en.wikipedia.org/wiki/Barrel_shifter
  - https://upload.wikimedia.org/wikipedia/commons/thumb/2/2b/Crossbar_barrel_shifter.svg/1280px-Crossbar_barrel_shifter.svg.png

CS132 Lecture 14
- Processor architecture is discussed, including the ARM1 microprocessor (apparently used in the BBC Micro)
- Some concepts used can be borrowed, to simplify things:
  - No cache

Arithmetic Logic Unit:
- Can keep relatively simple, only implementing the core functions (keeping it very RISC-style or even stricter, for example, no direct multiplication instruction).
- Required operations:
  - Addition/subtraction
  - AND, OR, NOT, XOR
  - Left/right shift

Addition/subtraction:
- Both can be performed by the same adder, if XOR gates are added to the second input along with an optional input carry
- This allows the second input's bits to be optionally flipped and one added to it (i.e. converting the number to the negative version)
- This is mentioned in CS132 Lecture 5

Program status register:
- 6502 uses NVBDIZC flags:
  - Negative
  - Overflow
  - BRK command (1 = BRK, 0 = IRQB)
  - Decimal mode
  - IRQB disable (1 = disable)
  - Zero
  - Carry
- CNVZ flags report results of ALU operations (conditional branches test these flags)
- IRQB is the interrupt disable flag. It's not needed if interrupts are not directly supported in hardware (it may be possible to implement them in software instead).
- Not sure exactly what BRK and decimal flags are for :/
  - It seems that decimal mode is to do with how hex is interpreted - $12 is either 18 in decimal (non-decimal mode), or 12 (decimal mode)
  - It is unlikely that this flag is needed in a simplified architecture
- It seems that CNVZ are the most important flags. It seems possible to avoid implementing the other flags, which would be much easier.
- C (carry) flag is used when unsigned arithmetic causes an "overflow".
  - E.g. 255 + 255 (unsigned) = 11111111 + 11111111 = 00000000 (carry 1), so C would be set.
- V (overflow) flag is used to denote overflows when doing signed arithmetic.
  - E.g. 127 + 127 (in a 1 byte 2's complement format) = 11111110, which is -2 (not 254 as expected), so V would be set.

Instruction set:

HLT			; End the program. This is all zeroes, so attempting to execute blank memory also halts the program.

MOV Rx, Ry		; Ry -> Rx

LDR Rx, mem		; value at mem -> Rx
STR Rx, mem		; Rx -> mem location


ADD Rx, Ry		; Rx + Ry -> Rx
ADC Rx, Ry		; Rx + Ry + C -> Rx
SUB Rx, Ry		; Rx - Ry -> Rx
SBC Rx, Ry		; Rx - Ry - C -> Rx

LSL Rx, Ry		; Rx << Ry -> Rx
LSR Rx, Ry		; Rx >> Ry -> Rx
ASL Rx, Ry		; Rx << Ry -> Rx (synonym for LSL, and so assessmbles to the same binary)
ASR Rx, Ry		; Rx >> Ry -> Rx (high bits are filled with bit 19 of Rx instead of zeroes)
ROL Rx, Ry		; Rx << Ry -> Rx (carry bits get moved to the other side)
ROR Rx, Ry		; Rx >> Ry -> Rx (carry bits get moved to the other side)

AND Rx, Ry		; Rx & Ry -> Rx
ORR Rx, Ry		; Rx | Ry -> Rx
EOR Rx, Ry		; Rx ^ Ry -> Rx

NOT Rx		; ~Rx -> Rx


BCC label		; Branch on C = 0 (carry clear)
BCS label		; Branch on C = 1 (carry set)
BPL label		; Branch on N = 0 (positive or zero)
BMI label		; Branch on N = 1 (negative)
BNE label		; Branch on Z = 0 (not zero)
BEQ label		; Branch on Z = 1 (zero)
BVC label		; Branch on V = 0 (overflow clear)
BVS label		; Branch on V = 1 (overflow set)

JMP label		; Unconditional branch

CMP Rx, Ry		; Rx - Ry (store flags but don't store result)

CLC ?
CLV ?
SEC ?

Note: Ry can be #10 for a decimal 10, #0x10 for a hex 10 (= decimal 16), #0b10 for a binary 10 (= decimal 2)
Note: By default, constants are signed, but can be changed to unsigned by appending a u: #10u, #0x10u, #0b10u
Note: mem can be [Ry] to use the value in Ry as a pointer to a memory location, or 10 for memory location 10, or a label
Note: label is the same as mem

Note: if a shift by zero occurs, the carry and overflow flags are not modified (Z and N are still updated)

22 instructions minimum, 30 maximum (so far)
Therefore 5 bits are needed for the opcode.

Addressing modes:
- Implied (No operand needed)
  - E.g. CLC
- Immediate (Operand is a literal)
  - E.g. ADD Rx, #10
- Register (Operand is a register)
  - E.g. ADD Rx, Ry
- Direct (Operand is a memory location)
  - E.g. LDR Rx, 10
- Register indirect (Operand is a register which is treated as a memory location)
  - E.g. LDR Rx, [Ry]
- Memory indirect (Operand is a memory location pointing to another memory location)
  - E.g. LDR Rx, [10]

Memory indirect can be performed using two commands (requiring a spare register):
LDR Ry, 10		; Direct
ADD Rx, [Ry]	; Register indirect

This results in 5 required addressing modes, but implied does not care which addressing mode it uses.
This means we only need to distinguish four modes: immediate, register, direct, register indirect
Therefore 2 bits are needed for the addressing mode.

Branch statements and LDR/STR can use direct and register indirect addressing.
Mathematical and logical operations can use immediate and register addressing.

Registers:
- 10 Data registers (20-bit)
- 4 System registers (20-bit)
  - These are used for any temporary data storage needed to allow 4-letter operations, which get expanded into 3-letter mnemonics before assembly.
- Stack pointer and program counter (20-bit)
- CCR register (4-bit)

Note:
- Implementation-specific: 20-bit integers are implemented as 32-bit integers, but only the least significant 20 bits are used.
- This could be changed so that registers and memory contain 32-bit integers, but operands still have to be 20 bits.
- Currently RGB mode is not possible because it required 24-bit integers.
- A cut-down version could be used until the system is expanded to support 32-bit integers.

Memory:
- 20-bit addresses -> 1048576 memory locations (4MiB)

Instruction Layout (32 bits):
OooooAa-XxxxVvvvvvvvvvvvvvvvvvvv
OooooAa-XxxxYyyy----------------

O: Opcode
A: Addressing mode
X: Register X
Y: Register Y
V: Memory location or literal value
-: Reserved



Extras:

Screen:
- Several screen resolutions:
  - 512x512
  - 256x256 (uses 1/4 of the memory of 512x512, good if you need lots of memory)
- Several screen modes:
  - Black and white mode (uses 1 bit per pixel, total usage 8192/32768 bytes)
  - Paletted mode (uses 8 bits per pixel, 256 possible colours, total usage 65536/262144 bytes)
  - RGB mode (uses 24 bits per pixel, total usage 196608/786432 bytes)

Text output:
- TODO something something ascii
