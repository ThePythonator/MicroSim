# MicroSim

_A simple RISC-style processor instruction set, assembly code and processor architechture, partially influenced by the BBC Micro and Motorola 68k instruction sets._

# Instruction set

```
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
```