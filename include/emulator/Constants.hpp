#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace MicroSim {
	/*
	* Instruction Layout (32 bits):
	* OooooAa-XxxxVvvvvvvvvvvvvvvvvvvv
	* OooooAa-XxxxYyyy----------------
	* 
	* O: Opcode
	* A: Addressing mode
	* X: Register X
	* Y: Register Y
	* V: Memory location or literal value
	* -: Reserved
	*/

	// 20-bit addresses, so 2^20 memory locations
	const uint32_t MEMORY_SIZE = 1 << 20;

	// 16 user-accessible registers, plus 1 additional register (CIR)
	const uint8_t REGISTER_COUNT = 17;

	// User-accessible registers
	const uint8_t SP_INDEX = 14;
	const uint8_t PC_INDEX = 15;
	// Additional registers are index 16 onwards
	// These cannot be accessed by normal programs (since the registers are indexed by 4-bit numbers)
	const uint8_t CIR_INDEX = 16;

	enum CCR_FLAGS {
		CCR_Z = 0b1000,
		CCR_C = 0b0100,
		CCR_N = 0b0010,
		CCR_V = 0b0001
	};

	enum Opcode : uint8_t {
		OP_HLT = 0b00000,

		OP_MOV = 0b00001,

		OP_LDR = 0b00010,
		OP_STR = 0b00011,

		OP_ADD = 0b00100,
		OP_ADC = 0b00101,
		OP_SUB = 0b00110,
		OP_SBC = 0b00111,

		OP_LSL = 0b01000,
		OP_LSR = 0b01001,
		OP_ROL = 0b01010,
		OP_ROR = 0b01011,
		
		OP_AND = 0b01100,
		OP_ORR = 0b01101,
		OP_EOR = 0b01110,
		OP_NOT = 0b01111,

		OP_BCC = 0b10000,
		OP_BCS = 0b10001,
		OP_BPL = 0b10010,
		OP_BMI = 0b10011,
		OP_BNE = 0b10100,
		OP_BEQ = 0b10101,
		OP_BVC = 0b10110,
		OP_BVS = 0b10111,

		OP_JMP = 0b11000,

		OP_CMP = 0b11001,

		OP_ASR = 0b11010

	};

	enum AddressingMode : uint8_t {
		MODE_IMPLICIT = 0b00, // Operand is not needed

		MODE_IMMEDIATE = 0b00, // Operand is a literal
		MODE_REGISTER = 0b01, // Operand is a register
		MODE_DIRECT = 0b10, // Operand is a memory location
		MODE_INDIRECT = 0b11,  // Operand is a register pointing to a memory location

		// MODE_INDIRECT is technically "register indirect" (as opposed to "memory indirect")
	};

	// Associate opcodes with supported addressing modes
	const std::map<Opcode, std::vector<AddressingMode>> SUPPORTED_ADDRESSING_MODES {
		{ OP_HLT, { MODE_IMPLICIT } },

		{ OP_MOV, { MODE_IMMEDIATE, MODE_REGISTER } },

		{ OP_LDR, { MODE_DIRECT, MODE_INDIRECT } },
		{ OP_STR, { MODE_DIRECT, MODE_INDIRECT } },

		{ OP_ADD, { MODE_IMMEDIATE, MODE_REGISTER } },
		{ OP_ADC, { MODE_IMMEDIATE, MODE_REGISTER } },
		{ OP_SUB, { MODE_IMMEDIATE, MODE_REGISTER } },
		{ OP_SBC, { MODE_IMMEDIATE, MODE_REGISTER } },

		{ OP_LSL, { MODE_IMMEDIATE, MODE_REGISTER } },
		{ OP_LSR, { MODE_IMMEDIATE, MODE_REGISTER } },
		{ OP_ROL, { MODE_IMMEDIATE, MODE_REGISTER } },
		{ OP_ROR, { MODE_IMMEDIATE, MODE_REGISTER } },

		{ OP_AND, { MODE_IMMEDIATE, MODE_REGISTER } },
		{ OP_ORR, { MODE_IMMEDIATE, MODE_REGISTER } },
		{ OP_EOR, { MODE_IMMEDIATE, MODE_REGISTER } },
		{ OP_NOT, { MODE_IMPLICIT } },

		{ OP_BCC, { MODE_DIRECT, MODE_INDIRECT } },
		{ OP_BCS, { MODE_DIRECT, MODE_INDIRECT } },
		{ OP_BPL, { MODE_DIRECT, MODE_INDIRECT } },
		{ OP_BMI, { MODE_DIRECT, MODE_INDIRECT } },
		{ OP_BNE, { MODE_DIRECT, MODE_INDIRECT } },
		{ OP_BEQ, { MODE_DIRECT, MODE_INDIRECT } },
		{ OP_BVC, { MODE_DIRECT, MODE_INDIRECT } },
		{ OP_BVS, { MODE_DIRECT, MODE_INDIRECT } },

		{ OP_JMP, { MODE_DIRECT, MODE_INDIRECT } },

		{ OP_CMP, { MODE_IMMEDIATE, MODE_REGISTER } },

		{ OP_ASR, { MODE_IMMEDIATE, MODE_REGISTER } },
	};

	const uint32_t SIGN_BIT_MASK = 1 << 19; // 20th bit is the sign
	const uint32_t NUMBER_MASK = 0x000fffff // Least significant 20 bits are used for calculations
}