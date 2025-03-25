#include "Tester.h"

Tester::Tester() {
	bus.connectCPU(&cpu);
	bus.connectSerial(&serial);
}

Tester::~Tester() {

}

bool Tester::ends_with(std::string const& value, std::string const& ending)
{
	if (ending.size() > value.size()) return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

void Tester::start() {
	uint8_t mooneyePassed = 0x00;
	uint8_t mooneyeFailed = 0x00;
	uint8_t blarggPassed = 0x00;
	uint8_t blarggFailed = 0x00;
	
	// Testing Mooneye
	bus.serial->setMode(2);
	for (int i = 0; i < 14; i++) {
		cart = new Cartridge(mooneyeTests[i]);

		if (!cart->isLoaded) {
			return;
		}

		bus.connectCartridge(cart);

		cpu.reset();

		bool testRunning = true;

		while (testRunning) {
			bus.clock();

			if (ends_with(serial.getOutput(), "358132134")) {
				serial.resetOutput();
				testRunning = false;
				mooneyePassed++;

				std::cout << std::endl << std::endl;
			}
		}

		delete cart;
	}

	// Testing Blargg
	bus.serial->setMode(1);
	for (int i = 0; i < 12; i++) {
		cart = new Cartridge(blarggTests[i]);

		if (!cart->isLoaded) {
			return;
		}

		bus.connectCartridge(cart);

		cpu.reset();

		bool testRunning = true;

		while (testRunning) {
			bus.clock();

			if (ends_with(serial.getOutput(), "Passed")) {
				serial.resetOutput();
				testRunning = false;
				blarggPassed++;

				std::cout << std::endl << std::endl;
			}
		}

		delete cart;
	}

	std::cout << std::dec;

	std::cout << "==================" << std::endl;

	std::cout << "Mooneye tests:" << std::endl;
	std::cout << "\tPassed: " << (int)mooneyePassed << std::endl;
	std::cout << "\tFailed: " << (int)mooneyeFailed << std::endl << std::endl;

	std::cout << "Blargg tests:" << std::endl;
	std::cout << "\tPassed: " << (int)blarggPassed << std::endl;
	std::cout << "\tFailed: " << (int)blarggFailed << std::endl;
}