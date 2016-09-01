//
//  system.h
//  gameboi
//
//  Created by Ryan Terry on 8/25/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#ifndef system_h
#define system_h

#define GAMEBOY_MEMORY_SIZE         0x10000
#define GAMEBOY_ROM_BANK_SIZE       0x4000

#define DIV_TIMER_RATE              0x100

#define DIV_TIMER_ADDR              0xFF04
#define TIMA_TIMER_ADDR             0xFF05
#define TIMA_MODULO_ADDR            0xFF06
#define TIMA_CONTROL_ADDR           0xFF07
#define INTERRUPT_REQUEST_ADDR      0xFF0F
#define INTERRUPT_ENABLED_ADDR      0xFFFF

#define INTERRUPT_VBLANK_MASK       0b00000001
#define INTERRUPT_LCDC_MASK         0b00000010
#define INTERRUPT_TIMER_MASK        0b00000100
#define INTERRUPT_SERIAL_MASK       0b00001000
#define INTERRUPT_CONTROLLER_MASK   0b00010000

struct gb_cpu;
struct gb_rom;
#include "integers.h"

struct gb_system
{
    struct gb_cpu* cpu;
    struct gb_rom* current_rom;
    
    bool IME; // interrupt master enable
    bool stopped;
    
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

bool gb_system_boot();
void gb_system_loop();

void gb_tick_delayed_interrupts();
void gb_service_interrupts();
u8 gb_get_IF();
u8 gb_get_IE();
void gb_set_IF(u8 interrupt_mask, bool value);

void gb_tick_timers(int lastCycles);
bool tima_enabled();
int tima_get_rate();

bool gb_system_validate_rom_checksum();
void gb_system_swap_bank(u8* bank_ptr, int bank);
bool gb_system_load_map_bootrom(int map_addr, int bootrom_size);

bool gb_specialwrite(u16 address, u16 value);

void gb_system_shutdown();

#endif /* system_h */
