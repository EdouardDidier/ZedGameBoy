#include "Serial.h"

Serial::Serial() {

}

Serial::~Serial() {

}

std::string Serial::getOutput() {
	return output;
}

void Serial::resetOutput() {
	output = "";
}

void Serial::setMode(uint8_t m) {
	mode = m;
}

void Serial::write(uint16_t addr, uint8_t data) {
	if (addr == 0xFF01) {
		if (mode == 1) {
			std::cout << data;								// Display of serials datas - Blargg
			output.push_back(data);
		}
		else if (mode == 2) {
			std::cout << std::dec << (int)data << " ";		// Display of serials datas - Mooneye

			if (data > 9) {
				output.push_back('0' + data / 10);
				output.push_back('0' + data - ((int)(data / 10) * 10));
			}
			else {
				output.push_back('0' + data);
			}
		}
	}
}

uint8_t Serial::read(uint16_t addr) {
	return 0x00; // Not yet implemented
}