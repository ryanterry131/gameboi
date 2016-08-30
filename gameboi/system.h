//
//  system.h
//  gameboi
//
//  Created by Ryan Terry on 8/25/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#ifndef system_h
#define system_h

#define GAMEBOY_MEMORY_SIZE 0x10000
#define GAMEBOY_ROM_BANK_SIZE 0x4000

#include <stdio.h>

struct z80;
struct gb_rom;
#include "integers.h"

extern BOOL gb_system_stopped;

struct gb_system
{
    struct z80* cpu;
    struct gb_rom* current_rom;
    
    u8* memory_map;
    u8* rom_bank0;      // 0x0000
    u8* rom_bank1;      // 0x4000
    u8* char_data;      // 0x8000
    u8* extram;         // 0xA000
    u8* wram;           // 0xC000
    u8* oam;            // 0xFE00
    u8* regs;           // 0xFF00
    u8* stack;          // 0xFF80
};

extern struct gb_system* gameboy;

BOOL gb_system_boot();
void gb_system_loop();

BOOL gb_system_validate_rom_checksum();
void gb_system_swap_bank(u8* bank_ptr, int bank);
BOOL gb_system_load_map_bootrom(int, int);

void gb_system_shutdown();

#endif /* system_h */
