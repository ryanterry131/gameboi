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
#include "dotmatrix.h"
#include "databus.h"

#include <stdlib.h> // for malloc
#include <string.h> // for memcpy
#include <stdio.h> // for printf and FILE

struct gb_system* gameboy;

bool gb_system_boot()
{
    gameboy = (struct gb_system*) malloc(sizeof(struct gb_system));
    gameboy->cpu = (struct gb_cpu*) malloc(sizeof(struct gb_cpu));
    gameboy->gpu = (struct gb_gpu*) malloc(sizeof(struct gb_gpu));
    gameboy->lcd = (struct gb_dotmatrix*) malloc(sizeof(struct gb_dotmatrix));

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
    if(!rom_load(&gameboy->current_rom, path) || !gb_system_validate_rom_checksum())
    {
        printf("Failed to load rom!\n");
        return false;
    }
    
    gb_system_map_bank(gameboy->rom_bank0, 0);
    gb_system_map_bank(gameboy->rom_bank1, 1);
    
    gameboy->in_bootrom = true;
    if(!gb_system_load_map_bootrom(0x0000, 0x100))
    {
        printf("Internal bootrom not yet implemented! Aborting...\n");
        //gb_system_internal_bootstrap();
        return false;
    }
    
    {
        // initial values for special registers
        gameboy->IME = false;
        databus_write8(DIV_TIMER_ADDR, 0x0000);
        databus_write8(TIMA_TIMER_ADDR, 0x0000);
        databus_write8(TIMA_MODULO_ADDR, 0x0000);
        databus_write8(TIMA_CONTROL_ADDR, 0x0000);
        databus_write8(INTERRUPT_REQUEST_ADDR, 0x0000);
        databus_write8(INTERRUPT_ENABLED_ADDR, 0x0000);
    }
    
    cpu_initialize(gameboy->cpu);
    gpu_initialize(gameboy->gpu);
    lcd_initialize(gameboy->lcd);
    audio_initialize();
    
    return true;
}

void gb_system_shutdown()
{
    audio_teardown();
    lcd_teardown(gameboy->lcd);
    gpu_teardown(gameboy->gpu);
    cpu_teardown(gameboy->cpu);
    rom_close(gameboy->current_rom);
    
    free(gameboy->memory_map);
    free(gameboy);
}

/*
 * Main loop for the console, ticks the CPU, rendering, etc.
*/
void gb_system_loop()
{
    int lastCycles = cpu_execute(gameboy->cpu, gameboy->cpu->reg_PC);
    if(lastCycles == 0)
    {
        // an exception occurred executing the instruction; bail
        gameboy->stopped = true;
        return;
    }

    gb_service_interrupts();
    gb_tick_delayed_interrupts();
    
    lcd_tick(gameboy->lcd);
    gpu_tick(gameboy->gpu, lastCycles);
    
    gb_tick_timers(lastCycles);
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
    if(gameboy->IME && !gameboy->in_bootrom)
    {
        u16 int_vector_addr = 0x0040;
        // scan through each possible interrupt and service if the bit is set
        for(u8 mask = 0b00000001; mask <= 0b00010000; mask <<= 1)
        {
            if((gb_get_IF() & mask) != 0 && (gb_get_IE() & mask) != 0)
            {
                printf("PROCESSING INTERRUPT: %#02x\n", mask);
                // disable global interrupts and the current interrupt
                gameboy->IME = false;
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
void gb_set_IE(u8 interrupt_mask, bool value)
{
    databus_write8(INTERRUPT_ENABLED_ADDR,
        ((databus_read8(INTERRUPT_ENABLED_ADDR) & ~interrupt_mask) | (value? interrupt_mask : 0b00000000)));
}
void gb_request_interrupt(u8 interrupt_mask)
{
    if(!gameboy->in_bootrom)
    {
        gameboy->IME = true; // global enable interrupts
        gb_set_IF(interrupt_mask, true); // set the interrupt flag
        gb_set_IE(interrupt_mask, true); // enable the interrupt
    }
}

/*
 * ---------------- TIMERS ----------------
 */

void gb_tick_timers(int lastCycles)
{
    // cycles before next DIV update
    static int div_cycles = 0;
    if((div_cycles += lastCycles) >= DIV_TIMER_RATE)
    {
        // carry overflown cycles into the next loop
        div_cycles %= DIV_TIMER_RATE;
        u8 div_timer = databus_read8(DIV_TIMER_ADDR);
        databus_write8(DIV_TIMER_ADDR, ++div_timer);
        flag_set_carry(div_timer == 0x00); // timer has just overflown
    }
    
    // cycles before next TIMA update
    static int tima_cycles = 0;
    if(tima_enabled() && ((tima_cycles += lastCycles) >= tima_get_rate()))
    {
        // carry overflown cycles into the next loop
        tima_cycles %= tima_get_rate();
        u8 tima_next = databus_read8(TIMA_TIMER_ADDR) + 1;
        // timer will overflow
        if(tima_next == 0x00)
        {
            gb_request_interrupt(INTERRUPT_TIMER_MASK);
            databus_write8(TIMA_TIMER_ADDR, databus_read8(TIMA_MODULO_ADDR));
        }
        else
            databus_write8(TIMA_TIMER_ADDR, tima_next);
    }
}

void tima_set_enabled(bool enabled)
{
    databus_write8(TIMA_CONTROL_ADDR,
        ((enabled << 2) | (databus_read8(TIMA_CONTROL_ADDR) & ~0x04)));
}
bool tima_enabled()
{
    return (databus_read8(TIMA_CONTROL_ADDR) & 0x4) != 0;
}
int tima_get_rate()
{
    switch(databus_read8(TIMA_CONTROL_ADDR) & 0x03)
    {
        case 0x00:
            return 1024;
        case 0x01:
            return 16;
        case 0x02:
            return 64;
        case 0x03:
        default:
            return 256;
    }
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

void gb_system_map_bank(u8* bank_ptr, int bank)
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

void gb_execute_dma_transfer(u8 offset)
{
    if(offset < 0x00)
    {
        offset = 0x00;
    }
    if(offset > 0x5F)
    {
        offset = 0x5F;
    }
    u16 src = 0x8000 + (offset * 0x100);
    memcpy(gameboy->memory_map + 0xFE00, gameboy->memory_map + src, 160);
}

/*
 * Callback for data bus writes for addresses that have special
 * effects when written to (for example writing to DIV resets it)
*/
bool gb_write_callback(u16 address, u16 value)
{
    switch(address)
    {
        case DIV_TIMER_ADDR:
            gameboy->memory_map[address] = 0x00;
            break;
        case DMA_ADDR:
            gb_execute_dma_transfer((u8)value);
            gameboy->memory_map[address] = value;
            break;
        default:
            return false;
    }
    return true;
}
