#pragma once

#include <algorithm>

#include "Constants.hpp"
#include "Exceptions.hpp"

namespace MicroSim {
	class Emulator {
	public:
		Emulator();

		void step();

		void fetch();
		void decode();
		void execute();

		bool finished();
		void reset();

	private:

		struct Instruction {
			Opcode opcode;
			AddressingMode mode;
			uint8_t register_a, register_b;
			uint16_t operand;
			
			// Either operand or register_b should be used (never both)
		};

		struct CCR {
			uint8_t c, z, n, v;
		};

		Instruction decode_instruction(uint32_t instruction);

		bool opcode_supports_addressing_mode(Opcode opcode, AddressingMode mode);

		uint32_t memory[MEMORY_SIZE] = { 0 };
		uint32_t registers[REGISTER_COUNT] = { 0 };

		CCR ccr = { 0, 0, 0, 0 };

		Instruction current_instruction;

		bool _finished = true;
	};
}
