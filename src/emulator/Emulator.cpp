#include "Emulator.hpp"

namespace MicroSim {
	void Emulator::fetch() {
		// TODO: fetch
		// TODO: not sure if this is correct, but it might be?
		registers[CIR_INDEX] = memory[registers[PC_INDEX]];

		// Increment program counter
		registers[PC_INDEX]++;
	}

	void Emulator::decode() {
		// Decode the instruction from the CIR and store it in current_instruction
		// Conceptually, the decoded instruction is not stored anywhere, but in reality we need to store it in current_instruction
		current_instruction = decode_instruction(registers[CIR_INDEX]);
	}

	void Emulator::execute() {
		switch (current_instruction.mode) {
		case AddressingMode::MODE_IMMEDIATE:
		case AddressingMode::MODE_DIRECT:
			// We don't need to do anything, because the operand is already a literal value or memory address
			break;

		case AddressingMode::MODE_REGISTER:
		case AddressingMode::MODE_INDIRECT:
			// Get the literal value or memory address specified by register_b, and store it in the operand
			current_instruction.operand = registers[current_instruction.register_b];
			break;

		default:
			// Addressing mode was not recognised
			throw InvalidAddressingMode(current_instruction.mode);
		}

		// Check that the addressing mode being used is supported by the opcode
		if (!opcode_supports_addressing_mode(current_instruction.opcode, current_instruction.mode)) {
			throw UnsupportedAddressingMode(current_instruction.opcode, current_instruction.mode);
		}

		switch (current_instruction.opcode) {
		case Opcode::OP_HLT: // Halt
			// TODO: Not sure what to do here.
			_finished = true;
			break;

		case Opcode::OP_MOV: // Move
			// Copy operand into register
			registers[current_instruction.register_a] = current_instruction.operand;
			break;

		case Opcode::OP_LDR: // Load
			// Copy value from memory location specified by operand into register
			registers[current_instruction.register_a] = memory[current_instruction.operand];
			break;

		case Opcode::OP_STR: // Store
			// Copy register to memory location specified by operand
			memory[current_instruction.operand] = registers[current_instruction.register_a];
			break;

		case Opcode::OP_ADD: // Add
			uint32_t a = registers[current_instruction.register_a];
			uint32_t b = current_instruction.operand;
			uint32_t r = a + b;
			
			uint32_t extra_bits = r & ~NUMBER_MASK; // Fetch first 12 bits
			r = r & NUMBER_MASK; // Update r to only be last 20 bits

			uint32_t a_sign = a & SIGN_BIT_MASK;
			uint32_t b_sign = b & SIGN_BIT_MASK;
			uint32_t r_sign = r & SIGN_BIT_MASK;

			// Update register
			registers[current_instruction.register_a] = r;

			// Set flags
			ccr.c = extra_bits != 0;
			ccr.z = r == 0;
			ccr.n = r_sign;
			ccr.v = (a_sign && b_sign && !r_sign) || (!a_sign && !b_sign && r_sign);
			break;

		case Opcode::OP_ADC: // Add with carry
			uint32_t a = registers[current_instruction.register_a];
			uint32_t b = current_instruction.operand;
			uint32_t r = a + b + ccr.c; // Include carry bit

			uint32_t extra_bits = r & ~NUMBER_MASK; // Fetch first 12 bits
			r = r & NUMBER_MASK; // Update r to only be last 20 bits

			uint32_t a_sign = a & SIGN_BIT_MASK;
			uint32_t b_sign = b & SIGN_BIT_MASK;
			uint32_t r_sign = r & SIGN_BIT_MASK;

			// Update register
			registers[current_instruction.register_a] = r;

			// Set flags
			// Note that both conditions for the carry flag can never be true at once (since the carry can only ever be one bit, so if it overflowed for r, then it won't overflow for new_r)
			ccr.c = extra_bits != 0;
			ccr.z = r == 0;
			ccr.n = r_sign;
			ccr.v = (a_sign && b_sign && !r_sign) || (!a_sign && !b_sign && r_sign);
			break;

		case Opcode::OP_SUB: // Subtract
			// TODO: Not implemented
			break;

		case Opcode::OP_SBC: // Subtract with carry
			// TODO: Not implemented
			break;

		case Opcode::OP_LSL: // Logical shift left#
			// TODO: check this is correct!
			// http://www.riscos.com/support/developers/asm/instrset.html
			//uint32_t a = registers[current_instruction.register_a];
			//uint32_t b = current_instruction.operand;
			//uint32_t r = a << b;

			//uint32_t extra_bits = r & ~NUMBER_MASK; // Fetch first 12 bits
			//r = r & NUMBER_MASK; // Update r to only be last 20 bits

			//uint32_t a_sign = a & SIGN_BIT_MASK;
			//uint32_t b_sign = b & SIGN_BIT_MASK;
			//uint32_t r_sign = r & SIGN_BIT_MASK;

			//// Update register
			//registers[current_instruction.register_a] = r;

			//ccr.c = extra_bits & 1; // Only use the least significant extra bit
			//ccr.z = r == 0;
			//ccr.n = r_sign;
			//ccr.v = (a_sign && b_sign && !r_sign) || (!a_sign && !b_sign && r_sign);
			break;

		case Opcode::OP_LSR: // Logical shift right
			// TODO: Not implemented
			break;

		case Opcode::OP_ASR: // Arithmetic shift right
			// http://www.riscos.com/support/developers/asm/instrset.html#19933
			// TODO: Not implemented
			break;

		case Opcode::OP_ROL: // Rotate left
			// TODO: Not implemented
			break;

		case Opcode::OP_ROR: // Rotate right
			// TODO: Not implemented
			break;

		case Opcode::OP_AND: // Logical AND
			uint32_t a = registers[current_instruction.register_a];
			uint32_t b = current_instruction.operand;
			uint32_t r = a & b;

			uint32_t r_sign = r & SIGN_BIT_MASK;

			// Update register
			registers[current_instruction.register_a] = r;

			// Set flags
			// Note that both conditions for the carry flag can never be true at once (since the carry can only ever be one bit, so if it overflowed for r, then it won't overflow for new_r)
			ccr.z = r == 0;
			ccr.n = r_sign;
			break;

		case Opcode::OP_ORR: // Logical OR
			uint32_t a = registers[current_instruction.register_a];
			uint32_t b = current_instruction.operand;
			uint32_t r = a | b;

			uint32_t r_sign = r & SIGN_BIT_MASK;

			// Update register
			registers[current_instruction.register_a] = r;

			// Set flags
			// Note that both conditions for the carry flag can never be true at once (since the carry can only ever be one bit, so if it overflowed for r, then it won't overflow for new_r)
			ccr.z = r == 0;
			ccr.n = r_sign;
			break;

		case Opcode::OP_EOR: // Logical XOR
			uint32_t a = registers[current_instruction.register_a];
			uint32_t b = current_instruction.operand;
			uint32_t r = a ^ b;

			uint32_t r_sign = r & SIGN_BIT_MASK;

			// Update register
			registers[current_instruction.register_a] = r;

			// Set flags
			// Note that both conditions for the carry flag can never be true at once (since the carry can only ever be one bit, so if it overflowed for r, then it won't overflow for new_r)
			ccr.z = r == 0;
			ccr.n = r_sign;
			break;

		case Opcode::OP_NOT: // Logical NOT
			uint32_t a = registers[current_instruction.register_a];
			uint32_t r = ~a;

			uint32_t r_sign = r & SIGN_BIT_MASK;

			// Update register
			registers[current_instruction.register_a] = r;

			// Set flags
			// Note that both conditions for the carry flag can never be true at once (since the carry can only ever be one bit, so if it overflowed for r, then it won't overflow for new_r)
			ccr.z = r == 0;
			ccr.n = r_sign;
			break;

		case Opcode::OP_BCC: // Branch if carry clear (C = 0)
			if (ccr.c != 0) break; // Only fall through if C == 0

		case Opcode::OP_BCS: // Branch if carry set (C = 1)
			if (ccr.c != 1) break; // Only fall through if C == 1

		case Opcode::OP_BPL: // Branch if plus (N = 0) i.e. positive or zero
			if (ccr.n != 0) break; // Only fall through if N == 0

		case Opcode::OP_BMI: // Branch if minus (N = 1) i.e. negative
			if (ccr.n != 1) break; // Only fall through if N == 1

		case Opcode::OP_BNE: // Branch if not equal (Z = 0) i.e. not zero
			if (ccr.z != 0) break; // Only fall through if Z == 0

		case Opcode::OP_BEQ: // Branch if equal (Z = 1) i.e. zero
			if (ccr.z != 1) break; // Only fall through if Z == 1

		case Opcode::OP_BVC: // Branch if overflow clear (V = 0)
			if (ccr.v != 0) break; // Only fall through if V == 0

		case Opcode::OP_BVS: // Branch if overflow set (V = 1)
			if (ccr.v != 1) break; // Only fall through if V == 1

		case Opcode::OP_JMP: // Jump unconditionally
			registers[PC_INDEX] = current_instruction.operand;
			break;

		case Opcode::OP_CMP: // Compare
			// TODO: Not implemented
			// Calculate difference but don't save the result - only set the flags needed
			break;

		default:
			throw InvalidOpcode(current_instruction.opcode);
		}
	}

	void Emulator::reset() {
		_finished = false;

		// TODO: wipe memory? or maybe should be wiped when program is loaded in again?
		// Maybe need to store a copy of the memory or program?
	}

	bool Emulator::finished() {
		return _finished;
	}


	Emulator::Instruction Emulator::decode_instruction(uint32_t instruction) {
		Instruction decoded_instruction;

		// Keep the last 20 bits
		decoded_instruction.operand = instruction & 0xfffff;

		// Remove the last 16 bits
		instruction >>= 16;

		// Keep the last 4 bits
		decoded_instruction.register_b = instruction & 0xf;

		// Remove the last 4 bits
		instruction >>= 4;

		// Keep the last 4 bits
		decoded_instruction.register_a = instruction & 0xf;

		// Remove the last 4 bits
		instruction >>= 4;

		// Remove another bit (currently reserved)
		instruction >>= 1;

		// Keep the last 2 bits
		decoded_instruction.mode = static_cast<AddressingMode>(instruction & 0b11);

		// Remove the last 2 bits
		instruction >>= 2;

		// Keep the last 5 bits
		decoded_instruction.opcode = static_cast<Opcode>(instruction);

		return decoded_instruction;
	}

	bool Emulator::opcode_supports_addressing_mode(Opcode opcode, AddressingMode mode) {
		const std::vector<AddressingMode>& supported_modes = SUPPORTED_ADDRESSING_MODES.at(opcode);
		return std::find(supported_modes.begin(), supported_modes.end(), mode) != supported_modes.end();
	}
}