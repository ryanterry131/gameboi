//
//  system.c
//  gameboi
//
//  Created by Ryan Terry on 8/25/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#include <stdlib.h>
#include <string.h>

#include "system.h"
#include "rom.h"
#include "core.h"
#include "audio.h"
#include "renderer.h"

struct gb_system* gameboy;
BOOL gb_system_stopped = FALSE;

BOOL gb_system_boot()
{
    gameboy = (struct gb_system*) malloc(sizeof(struct gb_system));
    gameboy->cpu = (struct z80*) malloc(sizeof(struct z80));
    
    {
        // setup memory pages
        gameboy->memory_map = (u8*) malloc(GAMEBOY_MEMORY_SIZE);
        gameboy->rom_bank0 = gameboy->memory_map + 0x0000;
        gameboy->rom_bank1 = gameboy->memory_map + 0x4000; // switchable bank
        gameboy->char_data = gameboy->memory_map + 0x8000;
        gameboy->extram = gameboy->memory_map + 0xA000;
        gameboy->wram = gameboy->memory_map + 0xC000;
        gameboy->oam = gameboy->memory_map + 0xFE00;
        gameboy->regs = gameboy->memory_map + 0xFF00;
        gameboy->stack = gameboy->memory_map + 0xFF80;
    }

    const char* path = "/Users/ryan/Downloads/blue.gb";
    if(!gb_rom_load(path) || !gb_system_validate_rom_checksum())
    {
        printf("Failed to load rom!\n");
        return FALSE;
    }
    
    gb_system_swap_bank(gameboy->rom_bank0, 0);
    gb_system_swap_bank(gameboy->rom_bank1, 1);
    
    if(!gb_system_load_map_bootrom(0x0000, 0x256))
    {
        printf("Internal bootrom not yet implemented! Aborting...\n");
        //gb_system_internal_bootstrap();
        return FALSE;
    }
    
    gameboy->cpu->reg_PC = 0x0000;
    
    gb_core_initialize();
    gb_audio_initialize();
    gb_renderer_initialize();
    
    return TRUE;
}

/*
 * Main loop for the console, ticks the CPU, rendering, etc.
*/
void gb_system_loop()
{
    cpu_execute(gameboy->cpu->reg_PC);
}

/*
 * Validate the 16-bit global rom checksum located at 0x14E in the rom header.
 * Done by adding all bytes in the rom except the 2 byte checksum.
 * Interestingly, real GameBoys don't actually validate this, so this checksum
 * goes unused. We'll do it anyway because we're well behaved.
*/
BOOL gb_system_validate_rom_checksum()
{
    u16 checksum = 0;
    for(int i = 0; i < gameboy->current_rom->rom_size; i++)
    {
        if(i == 0x14E || i == 0x14F)
        {
            continue;
        }
        checksum += gameboy->current_rom->rawBytes[i];
    }
    
    // swap endianess
    checksum = ((checksum << 8) | (checksum >> 8));
    printf("Generated checksum:%#04x\n", checksum);
    return checksum == gameboy->current_rom->header.globalChecksum;
}

void gb_system_swap_bank(u8* bank_ptr, int bank)
{
    memcpy(bank_ptr, gameboy->current_rom->rawBytes + (bank * GAMEBOY_ROM_BANK_SIZE), GAMEBOY_ROM_BANK_SIZE);
}

BOOL gb_system_load_map_bootrom(int map_addr, int bootrom_size)
{
    const char* bootrom_path = "/Users/ryan/Downloads/bootrom.bin";
    FILE* bootrom = fopen(bootrom_path, "rb");
    
    if(!bootrom)
    {
        printf("Bootrom image invalid!\n");
        return FALSE;
    }
    
    fread(gameboy->memory_map + map_addr, bootrom_size, 1, bootrom);
    
    fclose(bootrom);
    return TRUE;
}

void gb_system_shutdown()
{
    printf("Shutting down...\n");

    free(gameboy->cpu);
    free(gameboy->current_rom);
    free(gameboy->memory_map);
    free(gameboy);
    
    //gb_core_shutdown();
    //gb_audio_shutdown();
    //gb_renderer_shutdown();
}