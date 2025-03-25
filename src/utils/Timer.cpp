#include "Timer.h"

#include "../components/Bus.h"

Timer::Timer() {

}

Timer::~Timer() {

}

void Timer::connectBus(Bus* b) {
	bus = b;
}

uint8_t Timer::read(uint16_t addr) {
	switch (addr)
	{
	case 0xFF04:	// Divider Register
		return divider;
		break;
	case 0xFF05:	// TIMA Register
		return tima;
		break;
	case 0xFF06:	// TMA Register
		return tma;
		break;
	case 0xFF07:	// TAC Register
		return tac;
		break;
	}

	return 0x00;
}

void Timer::write(uint16_t addr, uint8_t data) {
	switch (addr)
	{
	case 0xFF04:	// Divider Register
		// Updating this address reset the System Counter and thus, the Devider Register
		updateCounter(0x0000);
		//TODO: reset when stop
		break;
	case 0xFF05:	// TIMA Register
		// To write to TIMA Register while it's reloading will be ignored
		if (!justReload) {
			timaJustSet = true;
			tima = data;
		}
		break;
	case 0xFF06:	// TMA Register
		tma = data;

		// Emulate a hardware behavior  : If written to TMA Register on the next cycle of TIMA overflow, the value of TMA is passed to TIMA
		if (justReload) {
			tima = data;
		}
		break;
	case 0xFF07:	// TAC Register
		updateTAC(data);
		break;
	}
}

void Timer::clock() {
	if (justReload) {
		justReload = false;
	}

	// Resetting TIMA Register 1 M-Cycle after that TIMA overflow
	if (isReloading) {
		isReloading = false;
		justReload = true;

		if (!timaJustSet) {
			tima = tma; // TIMA Register reset to the value of the TMA Register
			bus->write(0xFF0F, bus->getInterruptFlags() | Bus::interrupt_flags_t::t); // Schedule timer interrupt
		}
	}

	if (timaJustSet) {
		timaJustSet = false;
	}

	updateCounter(counter + 1);
}

void Timer::updateCounter(uint16_t v) {
	// TIMA Register is incremented if the 3rd bit of TAC Register is set
	// &&
	// TIMA is incremented a specific bit of the counter go from 1 to 0 depending of the lowest 2 bits of TAC Register
	// This induce several hardware bugs one is emulated below : When TMA is incremented if DIV is reset but reached half the requiring count to increase TIMA
	if ((tac & 0b100)
		&& (counter & modulo_bit[tac & 0b11])
		&& !(v & modulo_bit[tac & 0b11])) {
		incrementTIMA();
	}

	counter = v;

	// Gameboy clocks runs at 4.194304 MHz Frequency
	// We emulate the CPU at 1/4 of this value: 1.048576 MHz (M-Cycle)
	// Divider register is update at a rate of 16384 Hz (1/64 of CPU cycles)
	// It corresponds to bits 0b0011111111000000 of the System Counter in this emulator (upper 2 bytes in real hardware)
	divider = counter >> 6;
}

void Timer::updateTAC(uint8_t v) {
	// Emulate hardware bug that increment TIMA if timer is disable and Counter reached half the requiring count to increase TIMA
	// Reaching half of the requiring count is represented by the bit in 'modulo_bit' to be set
	if ((tac & 0b100) && !(v & 0b100) && (counter & modulo_bit[tac & 0b11])) {	//TODO: consider if it triggers a 'DIV-APU' event
		incrementTIMA();
	}
	// Emulate hardware bug that increase TIMA when TAC is written to in a specific way (to understand better)
	else if ((counter & modulo_bit[tac & 0b11]) && !(counter & modulo_bit[v & 0b11])) {
		incrementTIMA();
	}
	
	tac = 0xF8 | (v & 0b111); // Only the 3 lower bits are writable (others are set to 1)
}

void Timer::incrementTIMA() {
	tima++;

	// If TIMA Register overflow, it reset to TMA Register
	// It also schedule a timer interrupt by updating the IF Flags Register at address 0xFF0F
	// TODO: Emulate a behavior where the interrupt is delayed by 1 M-Cycle
	if (tima == 0x00) {
		isReloading = true;
	}
}
