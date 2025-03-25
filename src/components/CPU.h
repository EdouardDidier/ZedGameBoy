#pragma once

#include <cstdint>
#include <string>

class Bus;

class CPU
{
public:
	Bus* bus = nullptr;

	enum cpu_flags_t {
		z = (1 << 7),
		n = (1 << 6),
		h = (1 << 5),
		c = (1 << 4),
		u = 0b1111
	};

	// Registers
	union cpu_register_t {
		struct {
			uint8_t lo;
			uint8_t hi;
		};

		uint16_t full = 0x0000;
	};

	struct cpu_registers_t {
		cpu_register_t AF; // Accumulator & register
		cpu_register_t BC;
		cpu_register_t DE;
		cpu_register_t HL;
		uint16_t SP = 0x0000;
		uint16_t PC = 0x0000;
	} registers;

	uint8_t opcode = 0x00;
	uint16_t fetched_data = 0x0000;
	uint8_t	*dest_reg = nullptr;
	uint16_t *dest_reg16 = nullptr;
	uint16_t dest_address = 0x0000;
	uint8_t cycles = 0;

	bool isCycling = false;

	bool IMEScheduled = false;
	bool IME = false;
	bool isHalt = false;
	bool isStop = false;

public:
	void connectBus(Bus* b);
	void reset();
	void clock();

	uint8_t getCycles() const;

private:
	void computeCycles();
	void prepInstruction();

	uint8_t getFlag(cpu_flags_t f);
	void setFlag(cpu_flags_t f, bool v);
	void setFlags(bool z, bool n, bool h, bool c);

	uint8_t readBus(uint16_t addr);
	void writeBus(uint16_t addr, uint8_t data);

	bool handleInterrupt();

	/// ////////////////////// ///
	///	Address mode functions ///
	/// ////////////////////// ///

	void IMP();
	void RTR();	void HLR();	void RHL(); //TODO: Probably merge in one
	void NTR();	void NTH();
	void IRA();	void IAR();
	void AAB();	void ABA();
	void AA8();	void A8A();
	void IAC();	void ICA();

	void N16();
	void SAB();
	void HTS();	void STH();
	void RTS(); void STR();

	void REG(); void MRG(); void IMM(); void IM6();

	void R16(); void RTH();

	/// ////////////////////// ///
	///	Instructions functions ///
	/// ////////////////////// ///
	
	void NOP();
	void HLT(); void STP();
	void _EI(); void _DI();

	void LD8();
	void L16();	void LDS(); void LSE(); void LHS();
	void PSH(); void POP();

	void ADD(); void ADC();
	void SUB(); void SBC();
	void CMP();
	void INC(); void DEC();
	void AND(); void _OR(); void XOR();
	void CCF(); void SCF();
	void DAA(); void CPL();

	void I16(); void D16();
	void A16(); void ASP();

	void PRE();
	void RLCA(); void RLA(); void RRCA(); void RRA();
	void RLC(); void _RL(); void RRC(); void _RR();
	void SLA(); void SRA(); void SRL();
	void SWP();
	void BIT();	void RES();	void SET();

	void _JP();	void JPH();	void JPC();	void _JR();	void JRC();
	void CLL(); void CLC();
	void RET(); void REC(); void REI(); void RST();

	void XXX(); void ILL();

	/// ///////////// ///
	///	Address masks ///
	/// ///////////// ///
	uint8_t* maskR8(uint8_t bits);
	uint16_t* maskR16(uint8_t bits);
	uint16_t* maskR16stk(uint8_t bits);
	uint16_t maskR16mem(uint8_t bits);
	bool maskCond(uint8_t bits);
	uint8_t maskTarget(uint8_t bits);

	/// ////////////////// ///
	///	Instructions types ///
	/// ////////////////// ///
	struct cpu_instruction_t {
		std::string name;
		void(CPU::*operate)(void) = nullptr;
		void(CPU::*addrMode)(void) = nullptr;
		uint8_t cycles = 0;
	};

	struct cpu_prefixed_t {
		std::string name;
		void(CPU::* operate)(void) = nullptr;
		uint8_t cycles = 0;
	};

	const cpu_instruction_t instructions[0x100] = {
/*		x0								  x1								x2								  x3								x4								  x5								x6								  x7								x8								  x9								xA								  xB								xC								  xD								xE								  xF								*/
/* 0x */{"NOP", &CPU::NOP, &CPU::IMP, 1}, {"LD ", &CPU::L16, &CPU::N16, 3}, {"LD ", &CPU::LD8, &CPU::IAR, 2}, {"INC", &CPU::I16, &CPU::R16, 2}, {"INC", &CPU::INC, &CPU::MRG, 1}, {"DEC", &CPU::DEC, &CPU::MRG, 1}, {"LD ", &CPU::LD8, &CPU::NTR, 2}, {"RLCA",&CPU::RLCA,&CPU::REG, 1}, {"LD ", &CPU::LDS, &CPU::SAB, 5}, {"ADD", &CPU::A16, &CPU::RTH, 2}, {"LD ", &CPU::LD8, &CPU::IRA, 2}, {"DEC", &CPU::D16, &CPU::R16, 2}, {"INC", &CPU::INC, &CPU::MRG, 1}, {"DEC", &CPU::DEC, &CPU::MRG, 1}, {"LD ", &CPU::LD8, &CPU::NTR, 2}, {"RRCA",&CPU::RRCA,&CPU::REG, 1},
/* 1x */{"STOP",&CPU::STP, &CPU::IMM, 2}, {"LD ", &CPU::L16, &CPU::N16, 3}, {"LD ", &CPU::LD8, &CPU::IAR, 2}, {"INC", &CPU::I16, &CPU::R16, 2}, {"INC", &CPU::INC, &CPU::MRG, 1}, {"DEC", &CPU::DEC, &CPU::MRG, 1}, {"LD ", &CPU::LD8, &CPU::NTR, 2}, {"RLA", &CPU::RLA, &CPU::REG, 1}, {"JR ", &CPU::_JR, &CPU::IMM, 3}, {"ADD", &CPU::A16, &CPU::RTH, 2}, {"LD ", &CPU::LD8, &CPU::IRA, 2}, {"DEC", &CPU::D16, &CPU::R16, 2}, {"INC", &CPU::INC, &CPU::MRG, 1}, {"DEC", &CPU::DEC, &CPU::MRG, 1}, {"LD ", &CPU::LD8, &CPU::NTR, 2}, {"RRA", &CPU::RRA, &CPU::REG, 1},
/* 2x */{"JR ", &CPU::JRC, &CPU::IMM, 2}, {"LD ", &CPU::L16, &CPU::N16, 3}, {"LD ", &CPU::LD8, &CPU::IAR, 2}, {"INC", &CPU::I16, &CPU::R16, 2}, {"INC", &CPU::INC, &CPU::MRG, 1}, {"DEC", &CPU::DEC, &CPU::MRG, 1}, {"LD ", &CPU::LD8, &CPU::NTR, 2}, {"DAA", &CPU::DAA, &CPU::IMP, 1}, {"JR ", &CPU::JRC, &CPU::IMM, 2}, {"ADD", &CPU::A16, &CPU::RTH, 2}, {"LD ", &CPU::LD8, &CPU::IRA, 2}, {"DEC", &CPU::D16, &CPU::R16, 2}, {"INC", &CPU::INC, &CPU::MRG, 1}, {"DEC", &CPU::DEC, &CPU::MRG, 1}, {"LD ", &CPU::LD8, &CPU::NTR, 2}, {"CPL", &CPU::CPL, &CPU::IMP, 1},
/* 3x */{"JR ", &CPU::JRC, &CPU::IMM, 2}, {"LD ", &CPU::L16, &CPU::N16, 3}, {"LD ", &CPU::LD8, &CPU::IAR, 2}, {"INC", &CPU::I16, &CPU::R16, 2}, {"INC", &CPU::INC, &CPU::MRG, 3}, {"DEC", &CPU::DEC, &CPU::MRG, 3}, {"LD ", &CPU::LD8, &CPU::NTH, 3}, {"SCF", &CPU::SCF, &CPU::IMP, 1}, {"JR ", &CPU::JRC, &CPU::IMM, 2}, {"ADD", &CPU::A16, &CPU::RTH, 2}, {"LD ", &CPU::LD8, &CPU::IRA, 2}, {"DEC", &CPU::D16, &CPU::R16, 2}, {"INC", &CPU::INC, &CPU::MRG, 1}, {"DEC", &CPU::DEC, &CPU::MRG, 1}, {"LD ", &CPU::LD8, &CPU::NTR, 2}, {"CCF", &CPU::CCF, &CPU::IMP, 1},
/* 4x */{"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::HLR, 2}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::HLR, 2}, {"LD ", &CPU::LD8, &CPU::RTR, 1},
/* 5x */{"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::HLR, 2}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::HLR, 2}, {"LD ", &CPU::LD8, &CPU::RTR, 1},
/* 6x */{"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::HLR, 2}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::HLR, 2}, {"LD ", &CPU::LD8, &CPU::RTR, 1},
/* 7x */{"LD ", &CPU::LD8, &CPU::RHL, 2}, {"LD ", &CPU::LD8, &CPU::RHL, 2}, {"LD ", &CPU::LD8, &CPU::RHL, 2}, {"LD ", &CPU::LD8, &CPU::RHL, 2}, {"LD ", &CPU::LD8, &CPU::RHL, 2}, {"LD ", &CPU::LD8, &CPU::RHL, 2}, {"HALT",&CPU::HLT, &CPU::IMP, 2}, {"LD ", &CPU::LD8, &CPU::RHL, 2}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::RTR, 1}, {"LD ", &CPU::LD8, &CPU::HLR, 2}, {"LD ", &CPU::LD8, &CPU::RTR, 1},
/* 8x */{"ADD", &CPU::ADD, &CPU::REG, 1}, {"ADD", &CPU::ADD, &CPU::REG, 1}, {"ADD", &CPU::ADD, &CPU::REG, 1}, {"ADD", &CPU::ADD, &CPU::REG, 1}, {"ADD", &CPU::ADD, &CPU::REG, 1}, {"ADD", &CPU::ADD, &CPU::REG, 1}, {"ADD", &CPU::ADD, &CPU::REG, 2}, {"ADD", &CPU::ADD, &CPU::REG, 1}, {"ADC", &CPU::ADC, &CPU::REG, 1}, {"ADC", &CPU::ADC, &CPU::REG, 1}, {"ADC", &CPU::ADC, &CPU::REG, 1}, {"ADC", &CPU::ADC, &CPU::REG, 1}, {"ADC", &CPU::ADC, &CPU::REG, 1}, {"ADC", &CPU::ADC, &CPU::REG, 1}, {"ADC", &CPU::ADC, &CPU::REG, 2}, {"ADC", &CPU::ADC, &CPU::REG, 1},
/* 9x */{"SUB", &CPU::SUB, &CPU::REG, 1}, {"SUB", &CPU::SUB, &CPU::REG, 1}, {"SUB", &CPU::SUB, &CPU::REG, 1}, {"SUB", &CPU::SUB, &CPU::REG, 1}, {"SUB", &CPU::SUB, &CPU::REG, 1}, {"SUB", &CPU::SUB, &CPU::REG, 1}, {"SUB", &CPU::SUB, &CPU::REG, 2}, {"SUB", &CPU::SUB, &CPU::REG, 1}, {"SBC", &CPU::SBC, &CPU::REG, 1}, {"SBC", &CPU::SBC, &CPU::REG, 1}, {"SBC", &CPU::SBC, &CPU::REG, 1}, {"SBC", &CPU::SBC, &CPU::REG, 1}, {"SBC", &CPU::SBC, &CPU::REG, 1}, {"SBC", &CPU::SBC, &CPU::REG, 1}, {"SBC", &CPU::SBC, &CPU::REG, 2}, {"SBC", &CPU::SBC, &CPU::REG, 1},
/* Ax */{"AND", &CPU::AND, &CPU::REG, 1}, {"AND", &CPU::AND, &CPU::REG, 1}, {"AND", &CPU::AND, &CPU::REG, 1}, {"AND", &CPU::AND, &CPU::REG, 1}, {"AND", &CPU::AND, &CPU::REG, 1}, {"AND", &CPU::AND, &CPU::REG, 1}, {"AND", &CPU::AND, &CPU::REG, 2}, {"AND", &CPU::AND, &CPU::REG, 1}, {"XOR", &CPU::XOR, &CPU::REG, 1}, {"XOR", &CPU::XOR, &CPU::REG, 1}, {"XOR", &CPU::XOR, &CPU::REG, 1}, {"XOR", &CPU::XOR, &CPU::REG, 1}, {"XOR", &CPU::XOR, &CPU::REG, 1}, {"XOR", &CPU::XOR, &CPU::REG, 1}, {"XOR", &CPU::XOR, &CPU::REG, 2}, {"XOR", &CPU::XOR, &CPU::REG, 1},
/* Bx */{"OR ", &CPU::_OR, &CPU::REG, 1}, {"OR ", &CPU::_OR, &CPU::REG, 1}, {"OR ", &CPU::_OR, &CPU::REG, 1}, {"OR ", &CPU::_OR, &CPU::REG, 1}, {"OR ", &CPU::_OR, &CPU::REG, 1}, {"OR ", &CPU::_OR, &CPU::REG, 1}, {"OR ", &CPU::_OR, &CPU::REG, 2}, {"OR ", &CPU::_OR, &CPU::REG, 1}, {"CP ", &CPU::CMP, &CPU::REG, 1}, {"CP ", &CPU::CMP, &CPU::REG, 1}, {"CP ", &CPU::CMP, &CPU::REG, 1}, {"CP ", &CPU::CMP, &CPU::REG, 1}, {"CP ", &CPU::CMP, &CPU::REG, 1}, {"CP ", &CPU::CMP, &CPU::REG, 1}, {"CP ", &CPU::CMP, &CPU::REG, 2}, {"CP ", &CPU::CMP, &CPU::REG, 1},
/* Cx */{"RET", &CPU::REC, &CPU::IMP, 2}, {"POP", &CPU::POP, &CPU::STR, 3}, {"JPC", &CPU::JPC, &CPU::IM6, 3}, {"JP ", &CPU::_JP, &CPU::IM6, 4}, {"CALL",&CPU::CLC, &CPU::IM6, 3}, {"PSH", &CPU::PSH, &CPU::RTS, 4}, {"ADD", &CPU::ADD, &CPU::IMM, 2}, {"RST", &CPU::RST, &CPU::IMP, 4}, {"RET", &CPU::REC, &CPU::IMP, 2}, {"RET", &CPU::RET, &CPU::IMP, 4}, {"JPC", &CPU::JPC, &CPU::IM6, 3}, {"PRE", &CPU::PRE, &CPU::IMP, 0}, {"CALL",&CPU::CLC, &CPU::IM6, 3}, {"CALL",&CPU::CLL, &CPU::IM6, 6}, {"ADC", &CPU::ADC, &CPU::IMM, 2}, {"RST", &CPU::RST, &CPU::IMP, 4},
/* Dx */{"RET", &CPU::REC, &CPU::IMP, 2}, {"POP", &CPU::POP, &CPU::STR, 3}, {"JPC", &CPU::JPC, &CPU::IM6, 3}, {"ILL", &CPU::ILL, &CPU::IMP, 1}, {"CALL",&CPU::CLC, &CPU::IM6, 3}, {"PSH", &CPU::PSH, &CPU::RTS, 4}, {"SUB", &CPU::SUB, &CPU::IMM, 2}, {"RST", &CPU::RST, &CPU::IMP, 4}, {"RET", &CPU::REC, &CPU::IMP, 2}, {"RETI",&CPU::REI, &CPU::IMP, 4}, {"JPC", &CPU::JPC, &CPU::IM6, 3}, {"ILL", &CPU::ILL, &CPU::IMP, 1}, {"CALL",&CPU::CLC, &CPU::IM6, 3}, {"ILL", &CPU::ILL, &CPU::IMP, 1}, {"SBC", &CPU::SBC, &CPU::IMM, 2}, {"RST", &CPU::RST, &CPU::IMP, 4},
/* Ex */{"LD ", &CPU::LD8, &CPU::AA8, 3}, {"POP", &CPU::POP, &CPU::STR, 3}, {"LD ", &CPU::LD8, &CPU::IAC, 2}, {"ILL", &CPU::ILL, &CPU::IMP, 1}, {"ILL", &CPU::ILL, &CPU::IMP, 1}, {"PSH", &CPU::PSH, &CPU::RTS, 4}, {"AND", &CPU::AND, &CPU::IMM, 2}, {"RST", &CPU::RST, &CPU::IMP, 4}, {"ADD", &CPU::ASP, &CPU::IMM, 4}, {"JP ", &CPU::JPH, &CPU::IMP, 1}, {"LD ", &CPU::LD8, &CPU::AAB, 4}, {"ILL", &CPU::ILL, &CPU::IMP, 1}, {"ILL", &CPU::ILL, &CPU::IMP, 1}, {"ILL", &CPU::ILL, &CPU::IMP, 1}, {"XOR", &CPU::XOR, &CPU::IMM, 2}, {"RST", &CPU::RST, &CPU::IMP, 4},
/* Fx */{"LD ", &CPU::LD8, &CPU::A8A, 3}, {"POP", &CPU::POP, &CPU::STR, 3}, {"LD ", &CPU::LD8, &CPU::ICA, 2}, {"DI ", &CPU::_DI, &CPU::IMP, 1}, {"ILL", &CPU::ILL, &CPU::IMP, 1}, {"PSH", &CPU::PSH, &CPU::RTS, 4}, {"OR ", &CPU::_OR, &CPU::IMM, 2}, {"RST", &CPU::RST, &CPU::IMP, 4}, {"LD ", &CPU::LHS, &CPU::STH, 3}, {"LD ", &CPU::L16, &CPU::HTS, 2}, {"LD ", &CPU::LD8, &CPU::ABA, 4}, {"EI ", &CPU::_EI, &CPU::IMP, 1}, {"ILL", &CPU::ILL, &CPU::IMP, 1}, {"ILL", &CPU::ILL, &CPU::IMP, 1}, {"CP ", &CPU::CMP, &CPU::IMM, 2}, {"RST", &CPU::RST, &CPU::IMP, 4}
	};

	const cpu_prefixed_t prefixed[0x100] = {
/*		x0					   x1					  x2					 x3						x4					   x5					  x6					 x7						x8					   x9					  xA					 xB					    xC					   xD					  xE					 xF						*/
/* 0x */{"RLC", &CPU::RLC, 2}, {"RLC", &CPU::RLC, 2}, {"RLC", &CPU::RLC, 2}, {"RLC", &CPU::RLC, 2}, {"RLC", &CPU::RLC, 2}, {"RLC", &CPU::RLC, 2}, {"RLC", &CPU::RLC, 4}, {"RLC", &CPU::RLC, 2}, {"RRC", &CPU::RRC, 2}, {"RRC", &CPU::RRC, 2}, {"RRC", &CPU::RRC, 2}, {"RRC", &CPU::RRC, 2}, {"RRC", &CPU::RRC, 2}, {"RRC", &CPU::RRC, 2}, {"RRC", &CPU::RRC, 4}, {"RRC", &CPU::RRC, 2},
/* 1x */{"RL ", &CPU::_RL, 2}, {"RL ", &CPU::_RL, 2}, {"RL ", &CPU::_RL, 2}, {"RL ", &CPU::_RL, 2}, {"RL ", &CPU::_RL, 2}, {"RL ", &CPU::_RL, 2}, {"RL ", &CPU::_RL, 4}, {"RL ", &CPU::_RL, 2}, {"RR ", &CPU::_RR, 2}, {"RR ", &CPU::_RR, 2}, {"RR ", &CPU::_RR, 2}, {"RR ", &CPU::_RR, 2}, {"RR ", &CPU::_RR, 2}, {"RR ", &CPU::_RR, 2}, {"RR ", &CPU::_RR, 4}, {"RR ", &CPU::_RR, 2},
/* 2x */{"SLA", &CPU::SLA, 2}, {"SLA", &CPU::SLA, 2}, {"SLA", &CPU::SLA, 2}, {"SLA", &CPU::SLA, 2}, {"SLA", &CPU::SLA, 2}, {"SLA", &CPU::SLA, 2}, {"SLA", &CPU::SLA, 4}, {"SLA", &CPU::SLA, 2}, {"SRA", &CPU::SRA, 2}, {"SRA", &CPU::SRA, 2}, {"SRA", &CPU::SRA, 2}, {"SRA", &CPU::SRA, 2}, {"SRA", &CPU::SRA, 2}, {"SRA", &CPU::SRA, 2}, {"SRA", &CPU::SRA, 4}, {"SRA", &CPU::SRA, 2},
/* 3x */{"SWAP",&CPU::SWP, 2}, {"SWAP",&CPU::SWP, 2}, {"SWAP",&CPU::SWP, 2}, {"SWAP",&CPU::SWP, 2}, {"SWAP",&CPU::SWP, 2}, {"SWAP",&CPU::SWP, 2}, {"SWAP",&CPU::SWP, 4}, {"SWAP",&CPU::SWP, 2}, {"SRL", &CPU::SRL, 2}, {"SRL", &CPU::SRL, 2}, {"SRL", &CPU::SRL, 2}, {"SRL", &CPU::SRL, 2}, {"SRL", &CPU::SRL, 2}, {"SRL", &CPU::SRL, 2}, {"SRL", &CPU::SRL, 4}, {"SRL", &CPU::SRL, 2},
/* 4x */{"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 3}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 3}, {"BIT", &CPU::BIT, 2},
/* 5x */{"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 3}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 3}, {"BIT", &CPU::BIT, 2},
/* 6x */{"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 3}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 3}, {"BIT", &CPU::BIT, 2},
/* 7x */{"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 3}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 2}, {"BIT", &CPU::BIT, 3}, {"BIT", &CPU::BIT, 2},
/* 8x */{"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 4}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 4}, {"RES", &CPU::RES, 2},
/* 9x */{"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 4}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 4}, {"RES", &CPU::RES, 2},
/* Ax */{"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 4}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 4}, {"RES", &CPU::RES, 2},
/* Bx */{"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 4}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 2}, {"RES", &CPU::RES, 4}, {"RES", &CPU::RES, 2},
/* Cx */{"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 4}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 4}, {"SET", &CPU::SET, 2},
/* Dx */{"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 4}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 4}, {"SET", &CPU::SET, 2},
/* Ex */{"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 4}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 4}, {"SET", &CPU::SET, 2},
/* Fx */{"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 4}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 2}, {"SET", &CPU::SET, 4}, {"SET", &CPU::SET, 2}
	};
};

