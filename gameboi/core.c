//
//  core.c
//  gameboi
//
//  Created by Ryan Terry on 8/25/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#include "dataio.h"
#include "system.h"
#include "core.h"

static const char* instructions[] =
#include "instructions.h"

void cpu_execute(u16 address)
{
    u8 instr = cpu_read8(address);
    u8 cycles_add = 0;
    u8 pc_add = 0;
    
    printf("INSTRUCTION: (%s) at %#04x\n", instructions[instr], address);
    
    switch(instr)
    {
        case 0x00: // NOP
            cycles_add = 4;
            pc_add = 1;
            break;
        case 0x01: // LD BC, d16
            cycles_add = 16;
            pc_add = 3;
        case 0xC3: // JP
            cycles_add = 16;
            gameboy->cpu->reg_PC = cpu_read16(address + 1, FALSE);
            break;
        default:
            printf("CPU: Operand %#02x at %#04x not implemented! Aborting...\n", instr, address);
            gb_system_stopped = TRUE;
            return;
    }
    
    gameboy->cpu->cycles += cycles_add;
    gameboy->cpu->reg_PC += pc_add;
}

u8 cpu_read8(u16 address)
{
    return gameboy->memory_map[address];
}
u16 cpu_read16(u16 address, BOOL little_endian)
{
    u16 data = *((u16*) (gameboy->memory_map + address));
    if(little_endian)
    {
        data = ((data << 8) | (data >> 8));
    }
    
    return data;
}

void gb_core_initialize()
{
    
}
