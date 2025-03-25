#pragma once

#include <cstdint>
#include <string>

#include "./components/Bus.h"
#include "./components/CPU.h"
#include "./components/Cartridge.h"

class Gameboy
{
public:
	Bus bus;
	CPU cpu;
	Cartridge cart;
	Serial serial;

public:
	Gameboy(std::string filename);
	~Gameboy();

	void start();
};