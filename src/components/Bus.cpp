#include "Bus.h"

Bus::Bus() {
	for (uint16_t i = 0; i < 0x2000; i++) {
		wRam[i] = 0x00;
	}
		
	for (uint16_t i = 0; i < 0x7F; i++) {
		hRam[i] = 0x00;
	}
}

Bus::~Bus() {

}

void Bus::connectCPU(CPU* c) {
	timer.connectBus(this);

	cpu = c;	
	cpu->connectBus(this);
}

void Bus::connectCartridge(Cartridge* c) {
	cart = c;
}

void Bus::connectSerial(Serial* s) {
	serial = s;
}

void Bus::clock() {
	// Timers are incremented only if CPU is not stopped
	if (!cpu->isStop) {
		timer.clock();
	}

	if (cpu->cycles == 1) {				//TODO remove ater testing CPU
		std::cout << "";
	
		if (cpu->registers.PC == 0x01B2)
			std::cout << "";			
	}
	
	cpu->clock();

	clockCounter++;
}

uint8_t Bus::read(uint16_t addr) {
	if (addr >= 0x0000 && addr <= 0x3FFF) {			// From Cartridge - ROM Bank 00
		return cart->read(addr);
	}
	else if (addr >= 0x4000 && addr <= 0x7FFF) {	// From Cartridge - ROM Switchable bank via mapper
		return cart->read(addr);
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {	// From Cartridge - RAM switchable bank
		cart->read(addr);
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF) {	// Work RAM (8 KiB)
		return wRam[addr - 0xC000];
	}
	else if (addr >= 0xFF01 && addr <= 0xFF02) {	// Serial port
		return serial->read(addr);
	}
	else if (addr >= 0xFF04 && addr <= 0xFF07) {	// Timer register
		return timer.read(addr);
	}
	else if (addr == 0xFF44) {						// LY - hardcoded return peding LCD implementation
		// return 0x90; // Blaggrs
		return 0xFF; // Mooneye
	}
	else if (addr == 0xFF02) {						// hardcoded return peding LCD implementation
		return 0xFF; // Mooneye
	}
	else if (addr == 0xFF0F) {						// Interrupt flags
		return interruptFlags;
	}
	else if (addr >= 0xFF80 && addr <= 0xFFFE) {	// High Ram
		return hRam[addr - 0xFF80];
	}
	else if (addr == 0xFFFF) {						// Interrupt enable
		return interruptEnable;
	}

	return 0x00;
}

void Bus::write(uint16_t addr, uint8_t data) {
	if (addr >= 0x0000 && addr <= 0x3FFF) {			// From Cartridge - ROM Bank 00
		cart->write(addr, data);
	}
	else if (addr >= 0x4000 && addr <= 0x7FFF) {	// From Cartridge - ROM Switchable bank via mapper
		cart->write(addr, data);
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {	// From Cartridge - RAM switchable bank
		cart->write(addr, data);
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF) {	// Work RAM (8 KiB)
		wRam[addr - 0xC000] = data;
	}
	else if (addr >= 0xFF01 && addr <= 0xFF02) {	// Serial port
		serial->write(addr, data);
	}
	else if (addr >= 0xFF04 && addr <= 0xFF07) {	// Timer register
		timer.write(addr, data);
	}
	else if (addr == 0xFF0F) {						// Interrupt flags
		interruptFlags = data;
	}
	else if (addr >= 0xFF80 && addr <= 0xFFFE) {	// High Ram
		hRam[addr - 0xFF80] = data;
	}
	else if (addr == 0xFFFF) {						// Interrupt enable
		interruptEnable = data;
	}
}

void Bus::clearInterruptFlag(uint8_t mask) {
	interruptFlags &= ~mask;
}

uint8_t Bus::getInterruptFlags() const {
	return interruptFlags;
}

uint8_t Bus::getInterruptEnable() const {
	return interruptEnable;
}
