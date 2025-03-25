#include <iostream>
#include <fstream>

#include "Gameboy.h"
#include "./Tests/Tester.h"

int main() {
	//*
	Tester gb;	// A class that will load test roms and run tests
	/*/
	Gameboy gb("roms/gb-test-roms-master/cpu_instrs/individual/02-interrupts.gb");
	gb.serial.setMode(1);
	//*/

	gb.start();
	
	return 0;
}