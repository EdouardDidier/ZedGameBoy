#pragma once

#include <iostream>
#include <string>
#include <stdint.h>

class Serial
{
public:
	uint8_t mode = 0;
	std::string output;

public:
	Serial();
	~Serial();

	std::string getOutput();
	void resetOutput();
	void setMode(uint8_t m);

	void write(uint16_t addr, uint8_t data);
	uint8_t read(uint16_t addr);
};

