//
//  databus.c
//  gameboi
//
//  Created by Ryan Terry on 8/26/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#include "databus.h"
#include "system.h"

#include <string.h> // for memcpy

/*
 * All 16-bit reads/writes are in little endian
*/
u8 databus_read8(u16 address)
{
    return gameboy->memory_map[address];
}
u16 databus_read16(u16 address)
{
    return *((u16*) (gameboy->memory_map + address));
}
void databus_write8(u16 address, u8 value)
{
    if(!gb_specialwrite(address, value))
    {
        gameboy->memory_map[address] = value;
    }
}
void databus_write16(u16 address, u16 value)
{
    if(!gb_specialwrite(address, value))
    {
        *((u16*) (gameboy->memory_map + address)) = value;
    }
}

u16 databus_swap_endianess(u16 value)
{
    return ((value << 8) | (value >> 8));
}
