#pragma once

#include <cstdint>
#include <string>

#include "../components/Bus.h"
#include "../components/CPU.h"
#include "../components/Cartridge.h"
#include "../io/Serial.h"

class Tester
{
public:
	Bus bus;
	CPU cpu;
	Cartridge* cart = nullptr;
	Serial serial;

private:
	const std::string blarggTests[12] = {
		"roms/gb-test-roms-master/instr_timing/instr_timing.gb",
		"roms/gb-test-roms-master/cpu_instrs/individual/01-special.gb",
		"roms/gb-test-roms-master/cpu_instrs/individual/02-interrupts.gb",
		"roms/gb-test-roms-master/cpu_instrs/individual/03-op sp,hl.gb",
		"roms/gb-test-roms-master/cpu_instrs/individual/04-op r,imm.gb",
		"roms/gb-test-roms-master/cpu_instrs/individual/05-op rp.gb",
		"roms/gb-test-roms-master/cpu_instrs/individual/06-ld r,r.gb",
		"roms/gb-test-roms-master/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb",
		"roms/gb-test-roms-master/cpu_instrs/individual/08-misc instrs.gb",
		"roms/gb-test-roms-master/cpu_instrs/individual/09-op r,r.gb",
		"roms/gb-test-roms-master/cpu_instrs/individual/10-bit ops.gb",
		"roms/gb-test-roms-master/cpu_instrs/individual/11-op a,(hl).gb"
	};

	const std::string mooneyeTests[14] = {
		"roms/mts-20240127-1204-74ae166/acceptance/timer/div_write.gb",
		"roms/mts-20240127-1204-74ae166/acceptance/timer/rapid_toggle.gb",
		"roms/mts-20240127-1204-74ae166/acceptance/timer/tim00.gb",
		"roms/mts-20240127-1204-74ae166/acceptance/timer/tim00_div_trigger.gb",
		"roms/mts-20240127-1204-74ae166/acceptance/timer/tim01.gb",
		"roms/mts-20240127-1204-74ae166/acceptance/timer/tim01_div_trigger.gb",
		"roms/mts-20240127-1204-74ae166/acceptance/timer/tim10.gb",
		"roms/mts-20240127-1204-74ae166/acceptance/timer/tim10_div_trigger.gb",
		"roms/mts-20240127-1204-74ae166/acceptance/timer/tim11.gb",
		"roms/mts-20240127-1204-74ae166/acceptance/timer/tim11_div_trigger.gb",
		"roms/mts-20240127-1204-74ae166/acceptance/timer/tima_reload.gb",
		"roms/mts-20240127-1204-74ae166/acceptance/timer/tima_write_reloading.gb",
		"roms/mts-20240127-1204-74ae166/acceptance/timer/tma_write_reloading.gb",
		"roms/mts-20240127-1204-74ae166/acceptance/instr/daa.gb"
	};

public:
	Tester();
	~Tester();

	bool ends_with(std::string const& value, std::string const& ending);

	void start();
};

