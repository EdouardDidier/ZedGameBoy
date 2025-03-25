#pragma once

#include <cstdint>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream> 

class Cartridge
{
private:
    struct cart_header_t {
        uint8_t entry_point[4];     // Addresse 0x0100 - 0x0103
        uint8_t logo[0x30];         // Addresse 0x0104 - 0x0133
        char title[0x0F];           // Addresse 0x0134 - 0x0143
//      char manufacturer[0x04];    // Addresse 0x013F - 0x0142
//      uint8_t cgb_flag;           // Addresse 0x143
        uint16_t new_license_code;  // Addresse 0x0144 - 0x0145
        uint8_t sgb_flag;           // Addresse 0x0146
        uint8_t type;               // Addresse 0x0147
        uint8_t rom_size;           // Addresse 0x0148
        uint8_t ram_size;           // Addresse 0x0149
        uint8_t dest_code;          // Addresse 0x014A
        uint8_t license_code;       // Addresse 0x014B
        uint8_t version;            // Addresse 0x014C
        uint8_t checksum;           // Addresse 0x014D
        uint16_t global_checksum;   // Addresse 0x014E - 0x014D
    } *header = nullptr;

    uint32_t rom_size = 0;
    uint8_t* rom_data = nullptr;

public:
    bool isLoaded = false;

public:
    Cartridge(std::string filename);
    ~Cartridge();

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);

private:
    const char* type_table[0x23] = {
        "ROM ONLY",                         // 0x00
        "MBC1",                             // 0x01
        "MBC1+RAM",                         // 0x02
        "MBC1+RAM+BATTERY",                 // 0x03
        "??? 0x04",                         // 0x04
        "MBC2",                             // 0x05
        "MBC2+BATTERY",                     // 0x06
        "??? 0x07",                         // 0x07
        "ROM+RAM",                          // 0x08
        "ROM+RAM+BATTERY",                  // 0x09
        "0x0A ???",                         // 0x0A
        "MMM01",                            // 0x0B
        "MMM01+RAM",                        // 0x0C
        "MMM01+RAM+BATTERY",                // 0x0D
        "0x0E ???",                         // 0x0E
        "MBC3+TIMER+BATTERY",               // 0x0F
        "MBC3+TIMER+RAM+BATTERY",           // 0x10
        "MBC3",                             // 0x11
        "MBC3+RAM",                         // 0x12
        "MBC3+RAM+BATTERY",                 // 0x13
        "??? 0x14",                         // 0x14
        "??? 0x15",                         // 0x15
        "??? 0x16",                         // 0x16
        "??? 0x17",                         // 0x17
        "??? 0x18",                         // 0x18
        "MBC5",                             // 0x19
        "MBC5+RAM",                         // 0x1A
        "MBC5+RAM+BATTERY",                 // 0x1B
        "MBC5+RUMBLE",                      // 0x1C
        "MBC5+RUMBLE+RAM",                  // 0x1D
        "MBC5+RUMBLE+RAM+BATTERY",          // 0x1E
        "??? 0x1F",                         // 0x1F
        "MBC6",                             // 0x20
        "??? 0x21",                         // 0x21
        "MBC7+SENSOR+RUMBLE+RAM+BATTERY"    // 0x22
    };
    //TODO: Expand new_licence 0x10000 and correct Cartridge header to unbound to 0xFF (temoporary fix)
    const char* new_licence_table[0x100] = {
        "None",                                 // 0x00
        "Nintendo Research & Development 1",    // 0x01
        "None",                                 // 0x02
        "None",                                 // 0x03
        "None",                                 // 0x04
        "None",                                 // 0x05
        "None",                                 // 0x06
        "None",                                 // 0x07
        "Capcom",                               // 0x08
        "None",                                 // 0x09
        "None",                                 // 0x0A
        "None",                                 // 0x0B
        "None",                                 // 0x0C
        "None",                                 // 0x0D
        "None",                                 // 0x0E
        "None",                                 // 0x0F
        "None",                                 // 0x10
        "None",                                 // 0x11
        "None",                                 // 0x12
        "EA (Electronic Arts)",                 // 0x13
        "None",                                 // 0x14
        "None",                                 // 0x15
        "None",                                 // 0x16
        "None",                                 // 0x17
        "Hudson Soft",                          // 0x18
        "B-AI",                                 // 0x19
        "None",                                 // 0x1A
        "None",                                 // 0x1B
        "None",                                 // 0x1C
        "None",                                 // 0x1D
        "None",                                 // 0x1E
        "None",                                 // 0x1F
        "KSS",                                  // 0x20
        "None",                                 // 0x21
        "Planning Office WADA",                 // 0x22
        "None",                                 // 0x23
        "PCM Complete",                         // 0x24
        "San-X",                                // 0x25
        "None",                                 // 0x26
        "None",                                 // 0x27
        "Kemco",                                // 0x28
        "SETA Corporation",                     // 0x29
        "None",                                 // 0x2A
        "None",                                 // 0x2B
        "None",                                 // 0x2C
        "None",                                 // 0x2D
        "None",                                 // 0x2E
        "None",                                 // 0x2F
        "Viacom",                               // 0x30
        "Nintendo",                             // 0x31
        "Bandai",                               // 0x32
        "Ocean Software/Acclaim Entertainment", // 0x33
        "Konami",                               // 0x34
        "HectorSoft",                           // 0x35
        "None",                                 // 0x36
        "Taito",                                // 0x37
        "Hudson Soft",                          // 0x38
        "Banpresto",                            // 0x39
        "None",                                 // 0x3A
        "None",                                 // 0x3B
        "None",                                 // 0x3C
        "None",                                 // 0x3D
        "None",                                 // 0x3E
        "None",                                 // 0x3F
        "None",                                 // 0x40
        "Ubi Soft",                             // 0x41
	    "Atlus",                                // 0x42
        "None",                                 // 0x43
        "Malibu Interactive",                   // 0x44
        "None",                                 // 0x45
        "Angel",                                // 0x46
        "Bullet-Proof Software",                // 0x47
        "None",                                 // 0x48
        "Irem",                                 // 0x49
        "None",                                 // 0x4A
        "None",                                 // 0x4B
        "None",                                 // 0x4C
        "None",                                 // 0x4D
        "None",                                 // 0x4E
        "None",                                 // 0x4F
        "Absolute",                             // 0x50
        "Acclaim Entertainment",                // 0x51
        "Activision",                           // 0x52
        "Sammy USA Corporation",                // 0x53
        "Konami",                               // 0x54
        "Hi Tech Expressions",                  // 0x55
        "LJN",                                  // 0x56
        "Matchbox",                             // 0x57
        "Mattel",                               // 0x58
        "Milton Bradley Company",               // 0x59
        "None",                                 // 0x5A
        "None",                                 // 0x5B
        "None",                                 // 0x5C
        "None",                                 // 0x5D
        "None",                                 // 0x5E
        "None",                                 // 0x5F
        "Titus Interactive",                    // 0x60
        "Virgin Games Ltd.",                    // 0x61
        "None",                                 // 0x62
        "None",                                 // 0x63
        "Lucasfilm Games",                      // 0x64
        "None",                                 // 0x65
        "None",                                 // 0x66
        "Ocean Software",                       // 0x67
        "None",                                 // 0x68
        "EA (Electronic Arts)",                 // 0x69
        "None",                                 // 0x6A
        "None",                                 // 0x6B
        "None",                                 // 0x6C
        "None",                                 // 0x6D
        "None",                                 // 0x6E
        "None",                                 // 0x6F
        "Infogrames",                           // 0x70
        "Interplay Entertainment",              // 0x71
        "Broderbund",                           // 0x72
        "Sculptured Software",                  // 0x73
        "None",                                 // 0x74
        "The Sales Curve Limited",              // 0x75
        "None",                                 // 0x76
        "None",                                 // 0x77
        "THQ",                                  // 0x78
        "Accolade",                             // 0x79
        "None",                                 // 0x7A
        "None",                                 // 0x7B
        "None",                                 // 0x7C
        "None",                                 // 0x7D
        "None",                                 // 0x7E
        "None",                                 // 0x7F
        "Misawa Entertainment",                 // 0x80
        "None",                                 // 0x81
        "None",                                 // 0x82
        "lozc",                                 // 0x83
        "None",                                 // 0x84
        "None",                                 // 0x85
        "Tokuma Shoten",                        // 0x86
        "Tsukuda Original",                     // 0x87
        "None",                                 // 0x88
        "None",                                 // 0x89
        "None",                                 // 0x8A
        "None",                                 // 0x8B
        "None",                                 // 0x8C
        "None",                                 // 0x8D
        "None",                                 // 0x8E
        "None",                                 // 0x8F
        "None",                                 // 0x90
        "Chunsoft Co.",                         // 0x91
        "Video System",                         // 0x92
        "Ocean Software/Acclaim Entertainment", // 0x93
        "None",                                 // 0x94
        "Varie",                                // 0x95
        "Yonezawa/s’pal",                       // 0x96
        "Kaneko",                               // 0x97
        "None",                                 // 0x98
        "Pack-In-Video",                        // 0x99
        "None",                                 // 0x9A
        "None",                                 // 0x9B
        "None",                                 // 0x9C
        "None",                                 // 0x9D
        "None",                                 // 0x9E
        "None",                                 // 0x9F
        "None",                                 // 0xA0
        "None",                                 // 0xA1
        "None",                                 // 0xA2
        "None",                                 // 0xA3
        "Konami(Yu-Gi-Oh!)",                    // 0xA4
        "None",                                 // 0xA5
        "None",                                 // 0xA6
        "None",                                 // 0xA7
        "None",                                 // 0xA8
        "None",                                 // 0xA9
        "None",                                 // 0xAA
        "None",                                 // 0xAB
        "None",                                 // 0xAC
        "None",                                 // 0xAD
        "None",                                 // 0xAE
        "None",                                 // 0xAF
        "None",                                 // 0xB0
        "None",                                 // 0xB1
        "None",                                 // 0xB2
        "None",                                 // 0xB3
        "None",                                 // 0xB4
        "None",                                 // 0xB5
        "None",                                 // 0xB6
        "None",                                 // 0xB7
        "None",                                 // 0xB8
        "None",                                 // 0xB9
        "None",                                 // 0xBA
        "None",                                 // 0xBB
        "None",                                 // 0xBC
        "None",                                 // 0xBD
        "None",                                 // 0xBE
        "None",                                 // 0xBF
        "None",                                 // 0xC0
        "None",                                 // 0xC1
        "None",                                 // 0xC2
        "None",                                 // 0xC3
        "None",                                 // 0xC4
        "None",                                 // 0xC5
        "None",                                 // 0xC6
        "None",                                 // 0xC7
        "None",                                 // 0xC8
        "None",                                 // 0xC9
        "None",                                 // 0xCA
        "None",                                 // 0xCB
        "None",                                 // 0xCC
        "None",                                 // 0xCD
        "None",                                 // 0xCE
        "None",                                 // 0xCF
        "None",                                 // 0xD0
        "None",                                 // 0xD1
        "None",                                 // 0xD2
        "None",                                 // 0xD3
        "None",                                 // 0xD4
        "None",                                 // 0xD5
        "None",                                 // 0xD6
        "None",                                 // 0xD7
        "None",                                 // 0xD8
        "None",                                 // 0xD9
        "None",                                 // 0xDA
        "None",                                 // 0xDB
        "None",                                 // 0xDC
        "None",                                 // 0xDD
        "None",                                 // 0xDE
        "None",                                 // 0xDF
        "None",                                 // 0xE0
        "None",                                 // 0xE1
        "None",                                 // 0xE2
        "None",                                 // 0xE3
        "None",                                 // 0xE4
        "None",                                 // 0xE5
        "None",                                 // 0xE6
        "None",                                 // 0xE7
        "None",                                 // 0xE8
        "None",                                 // 0xE9
        "None",                                 // 0xEA
        "None",                                 // 0xEB
        "None",                                 // 0xEC
        "None",                                 // 0xED
        "None",                                 // 0xEE
        "None",                                 // 0xEF
        "None",                                 // 0xF0
        "None",                                 // 0xF1
        "None",                                 // 0xF2
        "None",                                 // 0xF3
        "None",                                 // 0xF4
        "None",                                 // 0xF5
        "None",                                 // 0xF6
        "None",                                 // 0xF7
        "None",                                 // 0xF8
        "None",                                 // 0xF9
        "None",                                 // 0xFA
        "None",                                 // 0xFB
        "None",                                 // 0xFC
        "None",                                 // 0xFD
        "None",                                 // 0xFE
        "None"                                  // 0xFF
    }; 
    const char* old_licence_table[0x100] = {
        "None",                     // 0x00
        "Nintendo",                 // 0x01
        "None",                     // 0x02
        "None",                     // 0x03
        "None",                     // 0x04
        "None",                     // 0x05
        "None",                     // 0x06
        "None",                     // 0x07
        "Capcom",                   // 0x08
        "HOT-B",                    // 0x09
        "Jaleco",                   // 0x0A
        "Coconuts Japan",           // 0x0B
        "Elite Systems",            // 0x0C
        "None",                     // 0x0D
        "None",                     // 0x0E
        "None",                     // 0x0F
        "None",                     // 0x10
        "None",                     // 0x11
        "None",                     // 0x12
        "EA (Electronic Arts)",     // 0x13
        "None",                     // 0x14
        "None",                     // 0x15
        "None",                     // 0x16
        "None",                     // 0x17
        "Hudson Soft",              // 0x18
        "ITC Entertainment",        // 0x19
        "Yanoman",                  // 0x1A
        "None",                     // 0x1B
        "None",                     // 0x1C
        "Japan Clary",              // 0x1D
        "None",                     // 0x1E
        "Virgin Games Ltd.",        // 0x1F
        "None",                     // 0x20
        "None",                     // 0x21
        "None",                     // 0x22
        "None",                     // 0x23
        "PCM Complete",             // 0x24
        "San-X",                    // 0x25
        "None",                     // 0x26
        "None",                     // 0x27
        "Kemco",                    // 0x28
        "SETA Corporation",         // 0x29
        "None",                     // 0x2A
        "None",                     // 0x2B
        "None",                     // 0x2C
        "None",                     // 0x2D
        "None",                     // 0x2E
        "None",                     // 0x2F
        "Infogrames",               // 0x30
        "Nintendo",                 // 0x31 
        "Bandai",                   // 0x32
        "",                         // 0x33 Indicates that the New licensee code should be used instead.
        "Konami",                   // 0x34
        "HectorSoft",               // 0x35
        "None",                     // 0x36
        "None",                     // 0x37
        "Capcom",                   // 0x38
        "Banpresto",                // 0x39
        "None",                     // 0x3A
        "None",                     // 0x3B
        ".Entertainment i",         // 0x3C
        "None",                     // 0x3D
        "Gremlin",                  // 0x3E
        "None",                     // 0x3F
        "None",                     // 0x40
        "Ubi Soft",                 // 0x41
        "Atlus",                    // 0x42
        "None",                     // 0x43
        "Malibu Interactive",       // 0x44
        "None",                     // 0x45
        "Angel",                    // 0x46
        "Spectrum Holoby",          // 0x47
        "None",                     // 0x48
        "Irem",                     // 0x49
        "Virgin Games Ltd.",        // 0x4A
        "None",                     // 0x4B
        "None",                     // 0x4C
        "Malibu Interactive",       // 0x4D
        "None",                     // 0x4E
        "U.S. Gold",                // 0x4F
        "Absolute",                 // 0x50
        "Acclaim Entertainment",    // 0x51
        "Activision",               // 0x52
        "Sammy USA Corporation",    // 0x53
        "GameTek",                  // 0x54
        "Park Place",               // 0x55
        "LJN",                      // 0x56
        "Matchbox",                 // 0x57
        "None",                     // 0x58
        "Milton Bradley Company",   // 0x59
        "Mindscape",                // 0x5A
        "Romstar",                  // 0x5B
        "Naxat Soft",               // 0x5C
        "Tradewest",                // 0x5D
        "None",                     // 0x5E
        "None",                     // 0x5F
        "Titus Interactive",        // 0x60
        "Virgin Games Ltd.",        // 0x61
        "None",                     // 0x62
        "None",                     // 0x63
        "None",                     // 0x64
        "None",                     // 0x65
        "None",                     // 0x66
        "Ocean Software",           // 0x67
        "None",                     // 0x68
        "EA (Electronic Arts)",     // 0x69
        "None",                     // 0x6A
        "None",                     // 0x6B
        "None",                     // 0x6C
        "None",                     // 0x6D
        "Elite Systems",            // 0x6E
        "Electro Brain",            // 0x6F
        "Infogrames",               // 0x70
        "Interplay Entertainment",  // 0x71
        "Broderbund",               // 0x72
        "Sculptured Software",      // 0x73
        "None",                     // 0x74
        "The Sales Curve Limited",  // 0x75
        "None",                     // 0x76
        "None",                     // 0x77
        "THQ",                      // 0x78
        "Accolade",                 // 0x79
        "Triffix Entertainment",    // 0x7A
        "None",                     // 0x7B
        "Microprose",               // 0x7C
        "None",                     // 0x7D
        "None",                     // 0x7E
        "Kemco",                    // 0x7F
        "Misawa Entertainment",     // 0x80
        "None",                     // 0x81
        "None",                     // 0x82
        "Lozc",                     // 0x83
        "None",                     // 0x84
        "None",                     // 0x85
        "Tokuma Shoten",            // 0x86
        "None",                     // 0x87
        "None",                     // 0x88
        "None",                     // 0x89
        "None",                     // 0x8A
        "Bullet-Proof Software",    // 0x8B
        "Vic Tokai",                // 0x8C
        "None",                     // 0x8D
        "Ape",                      // 0x8E
        "I’Max",                    // 0x8F
        "None",                     // 0x90
        "Chunsoft Co.",             // 0x91
        "Video System",             // 0x92
        "Tsubaraya Productions",    // 0x93
        "None",                     // 0x94
        "Varie",                    // 0x95
        "Yonezawa/S’Pal",           // 0x96
        "Kemco",                    // 0x97
        "None",                     // 0x98
        "Arc",                      // 0x99
        "Nihon Bussan",             // 0x9A
        "Tecmo",                    // 0x9B
        "Imagineer",                // 0x9C
        "Banpresto",                // 0x9D
        "None",                     // 0x9E
        "Nova",                     // 0x9F
        "Nova",                     // 0xA0
        "Hori Electric",            // 0xA1
        "Bandai",                   // 0xA2
        "None",                     // 0xA3
        "Konami",                   // 0xA4
        "None",                     // 0xA5
        "Kawada",                   // 0xA6
        "Takara",                   // 0xA7
        "Technos Japan",            // 0xA8
        "None",                     // 0xA9
        "Broderbund",               // 0xAA
        "None",                     // 0xAB
        "Toei Animation",           // 0xAC
        "Toho",                     // 0xAD
        "None",                     // 0xAE
        "Namco",                    // 0xAF
        "Acclaim Entertainment",    // 0xB0
        "ASCII Corporation or Nexsoft", // 0xB1
        "Bandai",                   // 0xB2
        "None",                     // 0xB3
        "Square Enix",              // 0xB4
        "None",                     // 0x85
        "HAL Laboratory",           // 0xB6
        "SNK",                      // 0xB7
        "None",                     // 0xB8
        "Pony Canyon",              // 0xB9
        "Culture Brain",            // 0xBA
        "Sunsoft",                  // 0xBB
        "None",                     // 0xBC
        "Sony Imagesoft",           // 0xBD
        "None",                     // 0xBE
        "Sammy Corporation",        // 0xBF
        "Taito",                    // 0xC0
        "None",                     // 0xC1
        "Kemco",                    // 0xC2
        "Square",                   // 0xC3
        "Tokuma Shoten",            // 0xC4
        "Data East",                // 0xC5
        "Tonkinhouse",              // 0xC6
        "None",                     // 0xC7
        "Koei",                     // 0xC8
        "UFL",                      // 0xC9
        "Ultra",                    // 0xCA
        "Vap",                      // 0xCB
        "Use Corporation",          // 0xCC
        "Meldac",                   // 0xCD
        "Pony Canyon",              // 0xCE
        "Angel",                    // 0xCF
        "Taito",                    // 0xD0
        "Sofel",                    // 0xD1
        "Quest",                    // 0xD2
        "Sigma Enterprises",        // 0xD3
        "ASK Kodansha Co.",         // 0xD4
        "Naxat Soft",               // 0xD5
        "None",                     // 0xD6
        "Copya System",             // 0xD7
        "None",                     // 0xD8
        "Banpresto",                // 0xD9
        "Tomy",                     // 0xDA
        "LJN",                      // 0xDB
        "None",                     // 0xDC
        "NCS",                      // 0xDD
        "Human",                    // 0xDE
        "Altron",                   // 0xDF
        "Jaleco",                   // 0xE0
        "Towa Chiki",               // 0xE1
        "Yutaka",                   // 0xE2
        "Varie",                    // 0xE3
        "None",                     // 0xE4
        "Epcoh",                    // 0xE5
        "None",                     // 0xE6
        "Athena",                   // 0xE7
        "None",                     // 0xE8
        "Asmik Ace Entertainment",  // 0xE9
        "Natsume",                  // 0xEA
        "King Records",             // 0xEB
        "Atlus",                    // 0xEC
        "Epic/Sony Records",        // 0xED
        "IGS",                      // 0xEE
        "None",                     // 0xEF
        "A Wave",                   // 0xF0
        "None",                     // 0xF1
        "None",                     // 0xF2
        "Extreme Entertainment",    // 0xF3
        "None",                     // 0xF4
        "None",                     // 0xF5
        "None",                     // 0xF6
        "None",                     // 0xF7
        "None",                     // 0xF8
        "None",                     // 0xF9
        "None",                     // 0xFA
        "None",                     // 0xFB
        "None",                     // 0xFC
        "None",                     // 0xFD
        "None",                     // 0xFE
        "LJN"                       // 0xFF
    }; 
};

