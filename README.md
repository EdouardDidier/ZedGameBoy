# ZedGameBoy

This project is an attempt to create a GameBoy emulator for learning purposes. My goal is to gain an understanding on how emulator works.

## Presentation

This is a work in progress. Currently, the emulator is able to emulate the CPU (SM83), RAM and Serial Port of the GameBoy.
All these components are connected to a BUS that allow them to communicate with each others.
It can also load ROM files and interface them with the BUS.

All the CPU instructions set is supported and tested using Blargg's and Mooneye test ROMs (see [Ressources](#Ressources) section).

## Dependencies
[Visual Studio](https://visualstudio.microsoft.com/fr/) was used for this project but any IDE can be used.

## Build instructions

- Create a new project in your IDE
- Import files under the "src" directory as source files
- Build and run

## Ressources

Documentation used :
- [Game Boy Development community](https://gbdev.io/) 
- [SM832 instruction set](https://gbdev.io/gb-opcodes/optables/) 

Test ROMs :
- [Mooneye test suite](https://github.com/Gekkio/mooneye-test-suite)
- [Blargg's GameBoy hardware test ROMs](https://github.com/retrio/gb-test-roms)
