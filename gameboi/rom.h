//
//  rom.h
//  gameboi
//
//  Created by Ryan Terry on 8/25/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#ifndef rom_h
#define rom_h

#include "integers.h"

struct rom_header
{
    // gameboy rom header begins at $0100
    
    // entry point, usually "NOP    JP $0150"
    u32 entryCode;
    // gameboy checksums this at boot so no 3rd party logos are used
    byte nintendoLogo[0x30];
    /*
     16 character game title
     TODO: on GBC this is 11, then 4 byte manufacterer code and 1 byte gbc flag
     */
    char gameTitle[0x10];
    // licensee code for new games (developed after the SGB)
    u16 newLicensee;
    // specifies whether or not this game uses SGB functions (0x03)
    u8 sgbFunction;
    // tells what kind of special hardware this cartridge uses
    u8 cartridgeType;
    // specifies the size of the ROM
    u8 ROMsize;
    // specifies the size of the external RAM (if any)
    u8 RAMsize;
    // 0 if this game is sold in Japan, 1 if sold elsewhere
    u8 international;
    // licensee code for old games (developed before the SGB)
    u8 oldLicensee;
    // version number
    u8 versionCode;
    /*
     8-bit checksum for header offsets $0134-$014C
     Checksum is calculated as follows:
     x=0:FOR i=0134h TO 014Ch:x=x-MEM[i]-1:NEXT
     Game crashes if this checksum is invalid
     */
    u8 headerChecksum;
    /*
     Checksum of the entire ROM, never tested by the gameboy
     Created by adding every byte of the ROM (except for these two bytes)
     */
    u16 globalChecksum;
};

struct gb_rom
{
    char* path;
    struct rom_header header;
    size_t rom_size;
    byte* rawBytes;
};

bool gb_rom_load(const char* path);
void gb_rom_print_header();
bool gb_rom_validate_checksum();

void gb_rom_read8(u8* destination, const byte* src, int offset);
void gb_rom_read16(u16* destination, const byte* src, int offset);
void gb_rom_read32(u32* destination, const byte* src, int offset);
void gb_rom_readChars(char* destination, const byte* src, int offset, int bytes);
void gb_rom_readBytes(byte* destination, const byte* src, int offset, int bytes);

#endif /* rom_h */
