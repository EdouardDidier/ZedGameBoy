#pragma once

#include <cstdint>

#include "../utils/Timer.h"
#include "CPU.h"
#include "Cartridge.h"
#include "../io/Serial.h"

class Bus
{
public:
	enum interrupt_flags_t {
		j = (1 << 4),
		s = (1 << 3),
		t = (1 << 2),
		l = (1 << 1),
		v = 1
	};

	Timer timer;
	CPU* cpu = nullptr;
	Cartridge* cart = nullptr;
	Serial* serial = nullptr;

	uint8_t interruptEnable = 0x00;
	uint8_t interruptFlags = 0x00;

	uint8_t wRam[0x2000];
	uint8_t hRam[0x7F];

	uint32_t clockCounter = 0;

public:
	Bus();
	~Bus();

	void connectCPU(CPU* c);
	void connectCartridge(Cartridge* c);
	void connectSerial(Serial* s);

	void clock();

	uint8_t read(uint16_t addr);
	void write(uint16_t addr, uint8_t data);

	void clearInterruptFlag(uint8_t mask);
	uint8_t getInterruptFlags() const;
	uint8_t getInterruptEnable() const;
};

