#pragma once

#include <stdexcept>

#include "Constants.hpp"

namespace MicroSim {

	class EmulatorError : public std::runtime_error {
		using std::runtime_error::runtime_error;
	};

	class InvalidDataError : public EmulatorError {
		using EmulatorError::EmulatorError;
	};

	class InvalidAddressingMode : public InvalidDataError {
	public:
		InvalidAddressingMode() : InvalidDataError("Invalid addressing mode: Mode is not recognised.") { }
		InvalidAddressingMode(int mode) : InvalidDataError("Invalid addressing mode: " + std::to_string(mode) + " is not recognised as an addressing mode.") { }
	};

	class UnsupportedAddressingMode : public InvalidDataError {
	public:
		UnsupportedAddressingMode() : InvalidDataError("Unsupported addressing mode: Mode is not supported by opcode.") { }
		UnsupportedAddressingMode(int opcode, int mode) : InvalidDataError("Unsupported addressing mode: " + std::to_string(mode) + " is not supported by opcode " + std::to_string(opcode) + ".") { }
	};

	class InvalidOpcode : public InvalidDataError {
	public:
		InvalidOpcode() : InvalidDataError("Invalid opcode: Opcode is not recognised.") { }
		InvalidOpcode(int opcode) : InvalidDataError("Invalid opcode: " + std::to_string(opcode) + " is not recognised as a valid opcode.") { }
	};
}