#include "CPU.h"

#include <iostream>

#include "Bus.h"

void CPU::connectBus(Bus* b) {
	bus = b;
}

void CPU::reset() {
	// Exemple of CPU state with bootrom DMG0
	registers.AF.full = 0x01B0;
	registers.BC.full = 0x0013;
	registers.DE.full = 0x00D8;
	registers.HL.full = 0x014D;
	registers.PC = 0x0100;
	registers.SP = 0xFFFE;

	opcode = 0x00;
	fetched_data = 0x0000;
	dest_reg = nullptr;
	dest_reg16 = nullptr;
	dest_address = 0x0000;
	cycles = 0;

	IME = false;
	isHalt = false;
	isStop = false;

	//computeCycles(); // Preparing the number of cycle to wait to execute first instruction
}

void CPU::clock() {
	if (!isCycling) {
		computeCycles();
		isCycling = true;
	}
		
	if (cycles > 0) {
		cycles--;
	}

	if (cycles == 0) {
		if (!isStop) {
			if (!isHalt) {
				if (!handleInterrupt()) {
					prepInstruction();

					if (IMEScheduled) { // TODO: consider if need to go in prepinstruction function (to handle hardware bug ?)
						IMEScheduled = false;
						IME = true;
					}

					opcode = readBus(registers.PC);
					registers.PC++;

					(this->*instructions[opcode].addrMode)();
					(this->*instructions[opcode].operate)();

					// Always set unused flags to 0
					setFlag(cpu_flags_t::u, false);

					isCycling = false;
				}
			}
			else {
				handleInterrupt();
			}
		}
	}
}

uint8_t CPU::getCycles() const {
	return cycles;
}

void CPU::computeCycles() {
	uint8_t ref = readBus(registers.PC);

	// Getting the expected number of cycle from instructions table
	cycles += instructions[ref].cycles;

	// If it's a JP or JR instruction need to add 1 cycle if condition is fullfill
	if ((ref & 0b11100111) == 0b11000010	 // Mask for JP cc instruction
	|| (ref & 0b11100111) == 0b00100000) // Mask for JR cc instruction
	{
		if (maskCond((ref & 0b00011000) >> 3)) {
			cycles += 1;
		}
	}
	// If it's a CALL or RET instruction need to add 3 cycles if condition is fullfill
	else if ((ref & 0b11100111) == 0b11000100	 // Mask for CALL cc instruction
		 || (ref & 0b11100111) == 0b11000000)
	{
		if (maskCond((ref & 0b00011000) >> 3)) {
			cycles += 3;
		}
	}
	// If it's a prefix instruction, then look cycles from prefixed table
	else if (ref == 0xCB) {
		ref = readBus(registers.PC + 1);
		cycles += prefixed[ref].cycles;
	}
}

void CPU::prepInstruction() {
	fetched_data = 0x0000;
	dest_reg = nullptr;
	dest_reg16 = nullptr;
	dest_address = 0x0000;

	// Always set unused flags to 0
	// TODO: Consider if required before execution of instruction
	setFlag(cpu_flags_t::u, false);
}

uint8_t CPU::getFlag(cpu_flags_t f) {
	return (registers.AF.lo & f) > 0 ? 1 : 0;
}

void CPU::setFlag(cpu_flags_t f, bool v) {
	if (v)	registers.AF.lo |= f;
	else	registers.AF.lo &= ~f;
}

void CPU::setFlags(bool z, bool n, bool h, bool c) {
	setFlag(cpu_flags_t::z, z);
	setFlag(cpu_flags_t::n, n);
	setFlag(cpu_flags_t::h, h);
	setFlag(cpu_flags_t::c, c);
}

uint8_t CPU::readBus(uint16_t addr) {
	return bus->read(addr);
}

void CPU::writeBus(uint16_t addr, uint8_t data) {
	bus->write(addr, data);
}

//TODO: Refactor this function
bool CPU::handleInterrupt() {
	// Interrupts are handle only if IME is set
	if (IME) {
		// Going through each interrupt flags by priority order (from lsb to msb)
		for (int i = 0; i < 4; i++) {
			// Interrupt is services only if the Interrupt Enable bit and Interrupt Flag are set
			if ((bus->getInterruptFlags() & (1 << i)) & (bus->getInterruptEnable() & (1 << i))) {
				isHalt = false; // If CPU was halt, it starts back
				bus->clearInterruptFlag(1 << i); // Resetting the handled flag
				IME = 0; // When interrupt is handle, IME flag is set to 0 so no other interrupt take over (unless IME flag is manually set back)

				prepInstruction();

				opcode = 0xCD; // Setting the CPU in state like it handle a "call function"

				cycles += 5; // Handling an interrupt takes 5 M-Cycle	//TODO: Move to computeCycles

				// Defining the address to jump to for the interrupt
				// It depends of which interrupt is handled
				// Possible addresses are 0x40, 0x48, 0x50, 0x58 and 0x60
				fetched_data = 0x40 + (i << 3);	
				(this->*instructions[opcode].operate)();

				// Always set unused flags to 0
				setFlag(cpu_flags_t::u, false);

				isCycling = false;

				return true;
			}
		}
	}
	else {
		// Going through each interrupt flags by priority order (from lsb to msb)
		for (int i = 0; i < 4; i++) {
			// Interrupt is services only if the Interrupt Enable bit and Interrupt Flag are set
			if ((bus->getInterruptFlags() & (1 << i)) & (bus->getInterruptEnable() & (1 << i))) {
				prepInstruction();

				if (IMEScheduled) { // TODO: consider if need to go in prepinstruction function (to handle hardware bug ?)
					IMEScheduled = false;
					IME = true;
				}

				opcode = readBus(registers.PC);
				registers.PC++;

				(this->*instructions[opcode].addrMode)();
				(this->*instructions[opcode].operate)();

				// Always set unused flags to 0
				setFlag(cpu_flags_t::u, false);

				isCycling = false;
			}
		}
	}

	return false;
}

/// ////////////////////// ///
///	Address mode functions ///
/// ////////////////////// ///

// Imediate
void CPU::IMP() {
	return;
}

// Register to Register
void CPU::RTR() {
	fetched_data = *maskR8(opcode & 0b00000111);
	dest_reg = maskR8((opcode & 0b00111000) >> 3);
}

// [HL] to Register
void CPU::HLR() {
	fetched_data = readBus(registers.HL.full);
	dest_reg = maskR8((opcode & 0b00111000) >> 3);
}

// Register to [HL]
void CPU::RHL() {
	fetched_data = *maskR8(opcode & 0b00000111);
	dest_address = registers.HL.full;
}

// Data to Register
void CPU::NTR() {
	fetched_data = readBus(registers.PC);
	registers.PC++;

	dest_reg = maskR8((opcode & 0b00111000) >> 3);
}

// Data to [HL]
void CPU::NTH() {
	fetched_data = readBus(registers.PC);
	registers.PC++;

	dest_address = registers.HL.full;
}

// Indirect 16-bits Registers to Accumulator
void CPU::IRA() {
	fetched_data = readBus(maskR16mem((opcode & 0b00110000) >> 4));
	dest_reg = &registers.AF.hi;
}

// Accumulator to Indirect 16-bits Registers
void CPU::IAR() {
	fetched_data = registers.AF.hi;
	dest_address = maskR16mem((opcode & 0b00110000) >> 4);
}

// Absolute 16-bits address to Accumulator
void CPU::ABA() {
	uint8_t addr_lo = readBus(registers.PC);
	registers.PC++;
	
	uint8_t addr_hi = readBus(registers.PC);
	registers.PC++;

	fetched_data = readBus((addr_hi << 8) | addr_lo);
	dest_reg = &registers.AF.hi;
}

// Accumulator to Absolute 16-bits
void CPU::AAB() {
	uint8_t addr_lo = readBus(registers.PC);
	registers.PC++;

	uint8_t addr_hi = readBus(registers.PC);
	registers.PC++;

	fetched_data = registers.AF.hi;
	dest_address = (addr_hi << 8) | addr_lo;
}

// Accumulator to Absolute 8-bits
void CPU::AA8() {
	uint8_t addr = readBus(registers.PC);
	registers.PC++;

	fetched_data = registers.AF.hi;
	dest_address = 0xFF00 | addr;
}

// Absolute 8-bits address to Accumulator
void CPU::A8A() {
	uint8_t addr = readBus(registers.PC);
	registers.PC++;

	fetched_data = readBus(0xFF00 | addr);
	dest_reg = &registers.AF.hi;
}

// Accumulator to [C]
void CPU::IAC() {
	fetched_data = registers.AF.hi;
	dest_address = 0xFF00 | registers.BC.lo;
}

// [C] to Accumulator
void CPU::ICA() {
	fetched_data = readBus(0xFF00 | registers.BC.lo);
	dest_reg = &registers.AF.hi;
}


// Data to Register 16-bit
void CPU::N16() {
	fetched_data = readBus(registers.PC);
	registers.PC++;

	fetched_data |= readBus(registers.PC) << 8;
	registers.PC++;

	dest_reg16 = maskR16((opcode & 0b00110000) >> 4);
}

// SP to Absolute 16-bits address to Accumulator
void CPU::SAB() {
	dest_address = readBus(registers.PC);
	registers.PC++;

	dest_address |= readBus(registers.PC) << 8;
	registers.PC++;
}

// HL to SP
void CPU::HTS() {
	fetched_data = registers.HL.full;
	dest_reg16 = &registers.SP;
}

// SP+e to HL
void CPU::STH() {
	fetched_data = registers.SP;
	dest_reg16 = &registers.HL.full;
}

// Register to Stack
void CPU::RTS() {
	fetched_data = *maskR16stk((opcode & 0b00110000) >> 4);
}

// Stack to Register
void CPU::STR() {
	dest_reg16 = maskR16stk((opcode & 0b00110000) >> 4);
}

// Register
void CPU::REG() {
	dest_reg = maskR8(opcode & 0b111);

	if (dest_reg) {
		fetched_data = *dest_reg;
	}
	else {
		dest_address = registers.HL.full;
		fetched_data = readBus(registers.HL.full);
	}
}

// Middle Register
void CPU::MRG() {
	dest_reg = maskR8((opcode & 0b00111000) >> 3);

	if (dest_reg) {
		fetched_data = *dest_reg;
	}
	else {
		dest_address = registers.HL.full;
		fetched_data = readBus(registers.HL.full);
	}
}

// Immediate
void CPU::IMM() {
	fetched_data = readBus(registers.PC);
	registers.PC++;
}

// Immediate 16-Bits
void CPU::IM6() {
	fetched_data = readBus(registers.PC);
	registers.PC++;

	fetched_data |= readBus(registers.PC) << 8;
	registers.PC++;
}

// 16-bit Registers
void CPU::R16() {
	dest_reg16 = maskR16((opcode & 0b00110000) >> 4);
}


// 16-bit Registers to HL
void CPU::RTH() {
	fetched_data = *maskR16((opcode & 0b00110000) >> 4);
}

/// ///////////// ///
///	Address masks ///
/// ///////////// ///

// Mask for 8-bit registed access
uint8_t* CPU::maskR8(uint8_t bits) {
	switch (bits) {
	case 0b000: // B
		return &registers.BC.hi;
	case 0b001: // C
		return &registers.BC.lo;
	case 0b010: // D
		return &registers.DE.hi;
	case 0b011: // E
		return &registers.DE.lo;
	case 0b100: // H
		return &registers.HL.hi;
	case 0b101: // L
		return &registers.HL.lo;
	case 0b110: // [HL]
		return nullptr;
	case 0b111: // A
		return &registers.AF.hi;
	}

	return nullptr;
}

// Mask for 16-bit registed access
uint16_t* CPU::maskR16(uint8_t bits) {
	switch (bits) {
	case 0b00: // BC
		return &registers.BC.full;
	case 0b01: // DE
		return &registers.DE.full;
	case 0b10: // HL
		return &registers.HL.full;
	case 0b11: // SP
		return &registers.SP;
	}

	return 0x0000;
}

// Mask for 16-bit registed access to stack
uint16_t* CPU::maskR16stk(uint8_t bits) {
	switch (bits) {
	case 0b00: // BC
		return &registers.BC.full;
	case 0b01: // DE
		return &registers.DE.full;
	case 0b10: // HL
		return &registers.HL.full;
	case 0b11: // AF
		return &registers.AF.full;
	}

	return 0x0000;
}

// Mask for register-based 16-bits indirect memory access
uint16_t CPU::maskR16mem(uint8_t bits) {
	switch (bits) {
	case 0b00: // BC
		return registers.BC.full;
	case 0b01: // DE
		return registers.DE.full;
	case 0b10: // HL+
		return registers.HL.full++;
	case 0b11: // HL- 
		return registers.HL.full--;
	}

	return 0x0000;
}

// Mask for condition
bool CPU::maskCond(uint8_t bits) {
	switch (bits) {
	case 0b00: // Non-Zero
		return !getFlag(cpu_flags_t::z);
	case 0b01: // Zero
		return getFlag(cpu_flags_t::z);
	case 0b10: // No Carry
		return !getFlag(cpu_flags_t::c);
	case 0b11: // Carry
		return getFlag(cpu_flags_t::c);
	}

	return false;
}

// Mast to target address
uint8_t CPU::maskTarget(uint8_t bits) {
	return  bits << 3;
}


/// ////////////////////// ///
///	Instructions functions ///
/// ////////////////////// ///

// NOP instruction
void CPU::NOP() {
	return;
}

// Enter CPU in low power state (awaiting interrupt)
void CPU::HLT() {
	isHalt = true;
}

// Enter CPU in very low power state
void CPU::STP() {
	isStop = true;
	bus->timer.divider = 0x00; // The timer is reset when CPU is stopped
}

// Load 8-bit
void CPU::LD8() {
	if (dest_reg) {
		*dest_reg = (uint8_t)fetched_data;
	}
	else {
		writeBus(dest_address, (uint8_t)fetched_data);
	}
}

// Load 16-bit
void CPU::L16() {
	if (dest_reg16) {
		*dest_reg16 = (uint16_t)fetched_data;
	}
}

// Load SP to ABS
void CPU::LDS() {
	writeBus(dest_address, registers.SP & 0xFF);
	writeBus(dest_address + 1, (registers.SP & 0xFF00) >> 8);
}

// Load SP + e to HL
void CPU::LHS() {
	if (dest_reg16) {
		int8_t e = readBus(registers.PC);
		registers.PC++;
		
		setFlags(
			0, 
			0, 
			(((fetched_data & 0xF) + (e & 0xF)) & 0x10) == 0x10,
			((fetched_data & 0xFF) + (e & 0xFF) & 0x100) == 0x100
		); 

		*dest_reg16 = fetched_data + e;
	}
}

// Push Register to Stack
void CPU::PSH() {
	registers.SP--;
	writeBus(registers.SP, (fetched_data & 0xFF00) >> 8);

	registers.SP--;
	writeBus(registers.SP, fetched_data & 0xFF);
}

// Pop Stack to Register
void CPU::POP() {
	*dest_reg16 = readBus(registers.SP);
	registers.SP++;

	*dest_reg16 |= readBus(registers.SP) << 8;
	registers.SP++;
}

// ADD : Add to A
void CPU::ADD() {
	uint8_t result = fetched_data + registers.AF.hi;
	setFlags(
		!result,
		0,
		(((fetched_data & 0xF) + (registers.AF.hi & 0xF)) & 0x10) == 0x10,
		fetched_data + registers.AF.hi > 0xFF
	);
	
	registers.AF.hi = result;
}

// ADC : Add with carry to A
void CPU::ADC() {
	uint8_t c = getFlag(cpu_flags_t::c);
	uint8_t result = fetched_data + registers.AF.hi + c;

	setFlags(
		!result,
		0,
		(((fetched_data & 0xF) + (registers.AF.hi & 0xF) + c) & 0x10) == 0x10,
		(fetched_data + registers.AF.hi + c )> 0xFF
	);

	registers.AF.hi = result;
}

// SUB : Aubstract to A
void CPU::SUB() {
	uint8_t result = registers.AF.hi - fetched_data;
	setFlags(
		!result,
		1,
		(((registers.AF.hi & 0xF) - (fetched_data & 0xF)) & 0x10) == 0x10,
		registers.AF.hi < fetched_data
	);

	registers.AF.hi = result;
}

// SBC : Substract with carry to A
void CPU::SBC() {
	uint8_t c = getFlag(cpu_flags_t::c);
	uint8_t result = registers.AF.hi - fetched_data - c;

	setFlags(
		!result,
		1,
		(((registers.AF.hi & 0xF) - (fetched_data & 0xF) - c) & 0x10) == 0x10,
		registers.AF.hi < (fetched_data + c)
	);

	registers.AF.hi = result;
}

// CMP : Compare with A
void CPU::CMP() {
	uint8_t result = registers.AF.hi - fetched_data;
	setFlags(
		!result,
		1,
		(((registers.AF.hi & 0xF) - (fetched_data & 0xF)) & 0x10) == 0x10,
		registers.AF.hi < fetched_data
	);
}

// AND to Accumulator
void CPU::AND() {
	uint8_t result = fetched_data & registers.AF.hi;

	setFlags(
		!result,
		0,
		1,
		0
	);

	registers.AF.hi = result;
}

// OR to Accumulator
void CPU::_OR() {
	uint8_t result = fetched_data | registers.AF.hi;

	setFlags(
		!result,
		0,
		0,
		0
	);

	registers.AF.hi = result;
}

// XOR to Accumulator
void CPU::XOR() {
	uint8_t result = fetched_data ^ registers.AF.hi;

	setFlags(
		!result,
		0,
		0,
		0
	);

	registers.AF.hi = result;
}

// Flip Carry Flag
void CPU::CCF() {
	setFlags(
		getFlag(cpu_flags_t::z),
		0,
		0,
		!getFlag(cpu_flags_t::c)
	);
}

// Set Carry Flag
void CPU::SCF() {
	setFlags(
		getFlag(cpu_flags_t::z),
		0,
		0,
		1
	);
}

// Decimal adjust accumulator
void CPU::DAA() {
	if (!getFlag(cpu_flags_t::n)) {
		if (getFlag(cpu_flags_t::c) || registers.AF.hi > 0x99) {
			registers.AF.hi += 0x60;
			setFlag(cpu_flags_t::c, true);
		}
		if (getFlag(cpu_flags_t::h) || (registers.AF.hi & 0x0F) > 0x09) {
			registers.AF.hi += 0x6;
		}
	}
	else {
		if (getFlag(cpu_flags_t::c)) {
			registers.AF.hi -= 0x60;
		}
		if (getFlag(cpu_flags_t::h)) {
			registers.AF.hi -= 0x6;
		}
	}

	setFlag(cpu_flags_t::z, registers.AF.hi == 0);
	setFlag(cpu_flags_t::h, false);
}

// Complement accumulator
void CPU::CPL() {
	registers.AF.hi = ~registers.AF.hi;

	setFlag(cpu_flags_t::n, true);
	setFlag(cpu_flags_t::h, true);
}


// Increment Register
void CPU::INC() {
	uint8_t result = fetched_data + 0x01;
	setFlags(
		!result,
		0,
		(((fetched_data & 0xF) + 0x01) & 0x10) == 0x10,
		getFlag(cpu_flags_t::c)
	);

	if (dest_reg) {
		*dest_reg = result;
	}
	else {
		writeBus(registers.HL.full, result);
	}
}

// Increment Register 16_bit
void CPU::I16() {
	(*dest_reg16)++;
}

// Decrement Register
void CPU::DEC() {
	uint8_t result = fetched_data - 0x01;
	setFlags(
		!result,
		1,
		(((fetched_data & 0xF) - 0x01) & 0x10) == 0x10,
		getFlag(cpu_flags_t::c)
	);

	if (dest_reg) {
		*dest_reg = result;
	}
	else {
		writeBus(registers.HL.full, result);
	}
}

// Decrement Register 16_bit
void CPU::D16() {
	(*dest_reg16)--;
}

// Add 16-bit
void CPU::A16() {
	uint16_t result = registers.HL.full + fetched_data;
	
	setFlags(
		getFlag(cpu_flags_t::z),
		0,
		(((registers.HL.full & 0xFFF) + (fetched_data & 0xFFF)) & 0x1000) == 0x1000,
		registers.HL.full + fetched_data > 0xFFFF
	);
	
	registers.HL.full = result;
}

// ADD to SP E value
void CPU::ASP() {
	uint16_t result = registers.SP + (int8_t)fetched_data;

	setFlags(
		0,
		0,
		(((registers.SP & 0xF) + ((int8_t)fetched_data & 0xF)) & 0x10) == 0x10,
		((registers.SP & 0xFF) + ((int8_t)fetched_data & 0xFF) & 0x100) == 0x100
	);

	registers.SP = result;
}

// Rotate Left on Accumulator
void CPU::RLCA() {
	setFlags(
		0,
		0,
		0,
		fetched_data & 0x80
	);

	*dest_reg = (fetched_data << 1) | getFlag(cpu_flags_t::c);
} 

// Rotate Leftthrough Carry on Accumulator
void CPU::RLA() {
	uint8_t tmp = getFlag(cpu_flags_t::c);

	setFlags(
		0,
		0,
		0,
		fetched_data & 0x80
	);

	*dest_reg = (fetched_data << 1) | tmp;
}	

// Rotate Right on Accumulator
void CPU::RRCA() {
	setFlags(
		0,
		0,
		0,
		fetched_data & 0x01
	);

	*dest_reg = (fetched_data >> 1) | (getFlag(cpu_flags_t::c) << 7);
}

// Rotate Right through Carry on Accumulator
void CPU::RRA() {
	uint8_t tmp = getFlag(cpu_flags_t::c) << 7;

	setFlags(
		0,
		0,
		0,
		fetched_data & 0x01
	);

	*dest_reg = (fetched_data >> 1) | tmp;
}

/// ///////////////////// ///
///	Prefixed Instructions ///
/// ///////////////////// ///

// Prefix instruction
void CPU::PRE() {
	opcode = readBus(registers.PC);
	registers.PC++;

	REG();
	(this->*prefixed[opcode].operate)();
}

// Rotate Left
void CPU::RLC() {
	uint8_t result = (fetched_data << 1) | ((fetched_data & 0x80) >> 7);

	setFlags(
		!result,
		0,
		0,
		fetched_data & 0x80
	);

	if (dest_reg) {
		*dest_reg = result;
	}
	else {
		writeBus(dest_address, result);
	}
}

// Rotate Leftthrough Carry
void CPU::_RL() {
	uint8_t result = (fetched_data << 1) | getFlag(cpu_flags_t::c);

	setFlags(
		!result,
		0,
		0,
		fetched_data & 0x80
	);

	if (dest_reg) {
		*dest_reg = result;
	}
	else {
		writeBus(dest_address, result);
	}
}

// Rotate Right
void CPU::RRC() {
	uint8_t result = (fetched_data >> 1) | ((fetched_data & 0x01) << 7);

	setFlags(
		!result,
		0,
		0,
		fetched_data & 0x01
	);

	if (dest_reg) {
		*dest_reg = result;
	}
	else {
		writeBus(dest_address, result);
	}
}

// Rotate Right through Carry
void CPU::_RR() {
	uint8_t result = (fetched_data >> 1) | getFlag(cpu_flags_t::c) << 7;

	setFlags(
		!result,
		0,
		0,
		fetched_data & 0x01
	);

	if (dest_reg) {
		*dest_reg = result;
	}
	else {
		writeBus(dest_address, result);
	}
}

// Shift Left
void CPU::SLA() {
	uint8_t result = fetched_data << 1;

	setFlags(
		!result,
		0,
		0,
		fetched_data & 0x80
	);

	if (dest_reg) {
		*dest_reg = result;
	}
	else {
		writeBus(dest_address, result);
	}
}

// Shift Right
void CPU::SRA() {
	uint8_t result = (fetched_data & 0x80) | (fetched_data >> 1);

	setFlags(
		!result,
		0,
		0,
		fetched_data & 0x01
	);

	if (dest_reg) {
		*dest_reg = result;
	}
	else {
		writeBus(dest_address, result);
	}
}

// Shift Right Logically
void CPU::SRL() {
	uint8_t result = fetched_data >> 1;

	setFlags(
		!result,
		0,
		0,
		fetched_data & 0x01
	);

	if (dest_reg) {
		*dest_reg = result;
	}
	else {
		writeBus(dest_address, result);
	}
}

// Swap upper and lower 4 bits
void CPU::SWP() {
	uint8_t result = ((fetched_data & 0xF) << 4) | ((fetched_data & 0xF0) >> 4);

	setFlags(
		!result,
		0,
		0,
		0
	);

	if (dest_reg) {
		*dest_reg = result;
	}
	else {
		writeBus(dest_address, result);
	}
}

// Test the x Bit of Register
void CPU::BIT() {
	setFlags(
		!(fetched_data & (1 << ((opcode & 0b00111000) >> 3))),
		0,
		1,
		getFlag(cpu_flags_t::c)
	);
}

// Reset the x Bit of Register
void CPU::RES() {
	uint8_t result = fetched_data & ~(1 << ((opcode & 0b00111000) >> 3));

	if (dest_reg) {
		*dest_reg = result;
	}
	else {
		writeBus(dest_address, result);
	}
}

// Set the x Bit of Register
void CPU::SET() {
	uint8_t result = fetched_data | (1 << ((opcode & 0b00111000) >> 3));

	if (dest_reg) {
		*dest_reg = result;
	}
	else {
		writeBus(dest_address, result);
	}
}

// Jump unconditonal to n16
void CPU::_JP() {
	registers.PC = fetched_data;
}

// Jump unconditonal to HL
void CPU::JPH() {
	registers.PC = registers.HL.full;
}

// Jump conditionally to HL
void CPU::JPC() {
	if (maskCond((opcode & 0b00011000) >> 3)) {
		registers.PC = fetched_data;
	}
}

// Jump unconditonal to E
void CPU::_JR() {
	registers.PC += (int8_t)fetched_data;
}

// Jump conditionally to E
void CPU::JRC() {
	uint16_t dest = registers.PC + (int8_t)fetched_data;

	if (maskCond((opcode & 0b00011000) >> 3)) {
		registers.PC = dest;
	}
}

// Call unconditionally n16
void CPU::CLL() {
	registers.SP--;
	writeBus(registers.SP, (registers.PC & 0xFF00) >> 8);

	registers.SP--;
	writeBus(registers.SP, registers.PC & 0xFF);

	registers.PC = fetched_data;
}

// Call conditionally n16
void CPU::CLC() {

	if (maskCond((opcode & 0b00011000) >> 3)) {
		registers.SP--;
		writeBus(registers.SP, (registers.PC & 0xFF00) >> 8);

		registers.SP--;
		writeBus(registers.SP, registers.PC & 0xFF);

		registers.PC = fetched_data;
	}
}

// Return from function uncondtionally
void CPU::RET() {
	uint16_t dest = readBus(registers.SP);
	registers.SP++;

	dest |= readBus(registers.SP) << 8;
	registers.SP++;

	registers.PC = dest;
}

// Return from function condtionally
void CPU::REC() {
	if (maskCond((opcode & 0b00011000) >> 3)) {
		uint16_t dest = readBus(registers.SP);
		registers.SP++;

		dest |= readBus(registers.SP) << 8;
		registers.SP++;

		registers.PC = dest;
	}
}

// Return from interrupt handler
void CPU::REI() {
	uint16_t dest = readBus(registers.SP);
	registers.SP++;

	dest |= readBus(registers.SP) << 8;
	registers.SP++;

	registers.PC = dest;

	IME = true;
}

// Restart to Target
void CPU::RST() {
	registers.SP--;
	writeBus(registers.SP, (registers.PC & 0xFF00) >> 8);

	registers.SP--;
	writeBus(registers.SP, registers.PC & 0xFF);

	registers.PC = maskTarget((opcode & 0b00111000) >> 3);
}

// Scedule IME
void CPU::_EI() {
	IMEScheduled = true;
}

// Disable IME
void CPU::_DI() {
	IME = false;
}

// Not supported insctruction
void CPU::XXX() {
	std::cout << "Opcode non supported: " << std::hex << std::uppercase << (opcode < 0x10 ? "0" : "") << opcode << std::endl;
}

// Illegal opcode
void CPU::ILL() {
	std::cout << "Illegal opcode non supported: " << std::hex << std::uppercase << (opcode < 0x10 ? "0" : "") << opcode << std::endl;
}


