#pragma once

#include <stdint.h>

class Bus;

class Timer
{
public:
	Bus* bus = nullptr;

	uint16_t counter = 0x2AF3;			// Expected value at PC = 0x0100 id 0xABCC on DMG which would be 0x2AF3 M-cycles
	uint8_t divider = 0xAB;				// Expected value at PC = 0x0100 id 0xAB on DMG
	uint8_t tima = 0x00;
	uint8_t tma = 0x00;
	uint8_t tac = 0xF8;

	const uint16_t modulo_bit[4] = {
		(1 << 7),	// 0b00 for 4096 Hz
		(1 << 1),	// 0b01 for 262144 Hz
		(1 << 3),	// 0b10 for 65536 Hz
		(1 << 5)	// 0b11 for 16386 Hz
	};

	// Emulator flags to emulate the behavior of the TIMA and TMA behavior when TIMA is reloading after an overflow
	bool timaJustSet = false;
	bool justReload = false;
	bool isReloading = false;

public:
	Timer();
	~Timer();

	void connectBus(Bus* b);

	uint8_t read(uint16_t addr);
	void write(uint16_t addr, uint8_t data);

	void clock();
	void updateCounter(uint16_t v);
	void updateTAC(uint8_t v);
	void incrementTIMA();
};

