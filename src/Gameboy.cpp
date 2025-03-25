#include "Gameboy.h"

Gameboy::Gameboy(std::string filename)
	: bus(), cpu(), cart(filename) {
	bus.connectCartridge(&cart);
	bus.connectCPU(&cpu);
	bus.connectSerial(&serial);

	cpu.reset();
}

Gameboy::~Gameboy() {

}

void Gameboy::start() {
	std::ofstream ofs("output.txt", std::ofstream::out);

	while (!cpu.isStop) {
		/*
		if (cpu.cycles == 1) {
			ofs
				<< "A:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)cpu.registers.AF.hi
				<< " F:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)cpu.registers.AF.lo
				<< " B:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)cpu.registers.BC.hi
				<< " C:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)cpu.registers.BC.lo
				<< " D:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)cpu.registers.DE.hi
				<< " E:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)cpu.registers.DE.lo
				<< " H:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)cpu.registers.HL.hi
				<< " L:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)cpu.registers.HL.lo
				<< " SP:" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << (int)cpu.registers.SP
				<< " PC:" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << (int)cpu.registers.PC
				<< " PCMEM:"
					<< std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)bus.read(cpu.registers.PC) << ","
					<< std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)bus.read(cpu.registers.PC + 1) << ","
					<< std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)bus.read(cpu.registers.PC + 2) << ","
					<< std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)bus.read(cpu.registers.PC + 3)
				<< std::endl;
		}
		//*/
		/*/
		if (cpu.cycles == 1) {
			std::cout << "";
			if (cpu.registers.PC == 0x016A)
				std::cout << "";
			if (cpu.registers.PC == 0x0267)
				std::cout << "";
			if (cpu.registers.PC == 0x0274)
				std::cout << "";
		}
		//*/
		bus.clock();
	}

	ofs.close();

	/*
	for (int i = 0; i < 100000; i++) {
		gb.clock();
	}
	*/
}