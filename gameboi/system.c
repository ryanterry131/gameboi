//
//  system.c
//  gameboi
//
//  Created by Ryan Terry on 8/25/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#include "system.h"
#include "rom.h"
#include "core.h"
#include "audio.h"
#include "gpu.h"
#include "databus.h"

#include <stdlib.h> // for malloc
#include <string.h> // for memcpy
#include <stdio.h> // for printf and FILE

struct gb_system* gameboy;

bool gb_system_boot()
{
    gameboy = (struct gb_system*) malloc(sizeof(struct gb_system));
    gameboy->cpu = (struct gb_cpu*) malloc(sizeof(struct gb_cpu));
    gameboy->stopped = false;
    
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
        return false;
    }
    
    gb_system_swap_bank(gameboy->rom_bank0, 0);
    gb_system_swap_bank(gameboy->rom_bank1, 1);
    
    if(!gb_system_load_map_bootrom(0x0000, 0x256))
    {
        printf("Internal bootrom not yet implemented! Aborting...\n");
        //gb_system_internal_bootstrap();
        return false;
    }
    
    gb_core_initialize();
    gb_audio_initialize();
    gb_gpu_initialize();
    
    return true;
}

/*
 * Main loop for the console, ticks the CPU, rendering, etc.
*/
void gb_system_loop()
{
    cpu_execute(gameboy->cpu->reg_PC);

    gb_service_interrupts();
    gb_tick_delayed_interrupts();
}

/*
 * ---------------- INTERRUPTS ----------------
*/

void gb_tick_delayed_interrupts()
{
    // TODO
}

void gb_service_interrupts()
{
    if(gameboy->cpu->IME)
    {
        u16 int_vector_addr = 0x0040;
        for(u8 mask = 0b00000001; mask <= 0b00010000; mask <<= 1)
        {
            if((gb_get_IF() & mask) != 0 && (gb_get_IE() & mask) != 0)
            {
                // disable global interrupts and the current interrupt
                gameboy->cpu->IME = false;
                gb_set_IF(mask, false);
                // push the existing PC onto the stack and replace it with the interrupt vector
                gameboy->cpu->reg_SP -= 2;
                databus_write16(gameboy->cpu->reg_SP, gameboy->cpu->reg_PC);
                gameboy->cpu->reg_PC = int_vector_addr;
                // this process consumes 5 cycles (two of which are stalled)
                gameboy->cpu->cycles += 5;
                return;
            }
            
            int_vector_addr += 0x0008;
        }
    }
}

u8 gb_get_IF()
{
    return databus_read8(INTERRUPT_REQUEST_ADDR);
}
u8 gb_get_IE()
{
    return databus_read8(INTERRUPT_ENABLED_ADDR);
}
void gb_set_IF(u8 interrupt_mask, bool value)
{
    databus_write8(INTERRUPT_REQUEST_ADDR,
        ((databus_read8(INTERRUPT_REQUEST_ADDR) & ~interrupt_mask) | (value? interrupt_mask : 0b00000000)));
}


/*
 * Validate the 16-bit global rom checksum located at 0x14E in the rom header.
 * Done by adding all bytes in the rom except the 2 byte checksum.
 * Interestingly, real GameBoys don't actually validate this, so this checksum
 * goes unused. We'll do it anyway because we're well behaved.
*/
bool gb_system_validate_rom_checksum()
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
    checksum = databus_swap_endianess(checksum);
    printf("Generated checksum:%#04x\n", checksum);
    return checksum == gameboy->current_rom->header.globalChecksum;
}

void gb_system_swap_bank(u8* bank_ptr, int bank)
{
    memcpy(bank_ptr, gameboy->current_rom->rawBytes + (bank * GAMEBOY_ROM_BANK_SIZE), GAMEBOY_ROM_BANK_SIZE);
}

bool gb_system_load_map_bootrom(int map_addr, int bootrom_size)
{
    const char* bootrom_path = "/Users/ryan/Downloads/bootrom.bin";
    FILE* bootrom = fopen(bootrom_path, "rb");
    
    if(!bootrom)
    {
        printf("Bootrom image invalid!\n");
        return false;
    }
    
    fread(gameboy->memory_map + map_addr, bootrom_size, 1, bootrom);
    
    fclose(bootrom);
    return true;
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