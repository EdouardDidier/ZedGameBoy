#include "Cartridge.h"

Cartridge::Cartridge(std::string filename) {
	std::ifstream ifs;
	ifs.open(filename.c_str(), std::ifstream::binary);

	if (!ifs.is_open()) {
		std::cout << "Failed to open file: " << filename << std::endl;
		return;
	}

	ifs.seekg(0, ifs.end);
	rom_size = (uint32_t)ifs.tellg();
	ifs.seekg(0, ifs.beg);

	rom_data = new uint8_t[rom_size];

	if (!rom_data) {
		std::cout << "Failed to load rom in memory: " << filename << std::endl;
		return;
	}

	ifs.read((char *)rom_data, rom_size);
	ifs.close();

	if (rom_size < 0x014F) {
		std::cout << "ROM non valid." << std::endl;
		return;
	}

	header = (cart_header_t*)(rom_data + 0x0100);

	std::cout << "Cartridge loaded:" << std::endl;
	std::cout << "\tTitle:\t\t" << header->title << std::endl;	
	std::cout << "\tLiscence:\t"
		<< (header->license_code == 0x33 ? new_licence_table[header->new_license_code & 0xFF] : old_licence_table[header->license_code])
		<< std::endl;
	std::cout << "\tType:\t\t" << type_table[header->type] << std::endl;
	std::cout << "\tROM size:\t" << (int)(32 * (1 << header->rom_size)) << " KB (Loaded:\t" << (rom_size / 1024) << " KB)" << std::endl;
	std::cout << "\tRAM size:\t" << std::hex << std::uppercase << (int)header->ram_size << std::endl;
	std::cout << "\tVersion:\t" << (int)header->ram_size << std::endl;

	uint8_t checksum = 0;
	for (uint16_t i = 0x0134; i <= 0x014C; i++) {
		checksum = checksum - rom_data[i] - 1;
	}

	std::cout << "\tChecksum:\t" 
		<< (((checksum & 0xFF) == header->checksum) ? "PASSED" : "FAILED") 
		<< " - 0x" 
		<< std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)checksum << " (0x" << (int)header->checksum << ")" << std::endl << std::endl;

	isLoaded = true;
}

Cartridge::~Cartridge() {
	delete[] rom_data;
}

uint8_t Cartridge::read(uint16_t addr) {
	if (addr >= 0x0000 && addr <= 0x3FFF) {			// ROM Bank 00
		return rom_data[addr];
	}
	else if (addr >= 0x4000 && addr <= 0x7FFF) {	// ROM Switchable bank via mapper
		return rom_data[addr];
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {	// RAM switchable bank
		// TODO (not supported yet)
	}

	return 0x00;
}

void Cartridge::write(uint16_t addr, uint8_t data) {
	if (addr >= 0x0000 && addr <= 0x3FFF) {			// ROM Bank 00
		// ROM - not supposed to be writable
	}
	else if (addr >= 0x4000 && addr <= 0x7FFF) {	// ROM Switchable bank via mapper
		// ROM - not supposed to be writable
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {	// RAM switchable bank
		// TODO (not supported yet)
	}
}