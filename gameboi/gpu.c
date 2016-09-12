//
//  gpu.c
//  gameboi
//
//  Created by Ryan Terry on 8/25/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#include "gpu.h"
#include "dotmatrix.h"
#include "databus.h"
#include "system.h"

#include <stdio.h> // for printf
#include <stdlib.h> // for free
#include <math.h> // for floor


enum gpu_mode
{
    GPU_MODE_HBLANK,
    GPU_MODE_VBLANK,
    GPU_MODE_OAM,
    GPU_MODE_TRANSFER
};


void gpu_initialize(struct gb_gpu* gpu)
{
    {
        // initialize special registers
        databus_write8(LCDC_ADDR, 0x00);
        databus_write8(STAT_ADDR, 0x02);
        databus_write8(SCY_ADDR, 0x00);
        databus_write8(SCX_ADDR, 0x00);
        databus_write8(LY_ADDR, 0x00);
    }
}

void gpu_teardown(struct gb_gpu* gpu)
{
    free(gpu);
}

void gpu_tick(struct gb_gpu* gpu, int lastCycles)
{
    if(lcdc_get_bit(LCDC_DISPLAY_ENABLED))
    {
        gpu->display_mode_cycles += lastCycles;
        switch(gpu_get_mode())
        {
            case 0x00: // H-Blank
            {
                if(gpu->display_mode_cycles >= 204)
                {
                    // carry overflown cycles into next display mode
                    gpu->display_mode_cycles %= 204;
                    
                    if(gpu_nextline(gpu) == 144)
                    {
                        // enter v-blank after 144 lines
                        gpu_set_mode(GPU_MODE_VBLANK);
                        gb_request_interrupt(INTERRUPT_VBLANK_MASK);
                        if((databus_read8(STAT_ADDR) & 0b00010000) != 0)
                        {
                            gb_request_interrupt(INTERRUPT_STAT_MASK);
                        }
                    }
                    else
                    {
                        // no v-blank yet; jump to the next line instead.
                        gpu_set_mode(GPU_MODE_OAM);
                        if((databus_read8(STAT_ADDR) & 0b00100000) != 0)
                        {
                            gb_request_interrupt(INTERRUPT_STAT_MASK);
                        }
                    }
                }
                break;
            }
            case 0x01: // V-Blank
            {
                // checks if we've passed a multiple of 456 (scanline cycle)
                if((floor(gpu->display_mode_cycles / 456) + 144) > databus_read8(LY_ADDR))
                {
                    if(gpu_nextline(gpu) > 153)
                    {
                        lcd_refresh(gameboy->lcd);
                        // reset from line 0
                        databus_write8(LY_ADDR, 0x00);
                        gpu->display_mode_cycles %= 4560;
                        gpu_set_mode(GPU_MODE_OAM);
                        if((databus_read8(STAT_ADDR) & 0b00100000) != 0)
                        {
                            gb_request_interrupt(INTERRUPT_STAT_MASK);
                        }
                    }
                }
                break;
            }
            case 0x02: // OAM access
            {
                if(gpu->display_mode_cycles >= 80)
                {
                    gpu->display_mode_cycles %= 80;
                    gpu_set_mode(GPU_MODE_TRANSFER);
                }
                break;
            }
            case 0x03: // Data transfer to LCD driver
            {
                if(gpu->display_mode_cycles >= 172)
                {
                    gpu->display_mode_cycles %= 172;
                    gpu_set_mode(GPU_MODE_HBLANK);
                    if((databus_read8(STAT_ADDR) & 0b00001000) != 0)
                    {
                        gb_request_interrupt(INTERRUPT_STAT_MASK);
                    }
                }
                break;
            }
        }
    }
}

u8 gpu_nextline(struct gb_gpu* gpu)
{
    u8 nextLine = databus_read8(LY_ADDR) + 1;
    databus_write8(LY_ADDR, nextLine);
    gpu_compare_ly();
    return nextLine;
}

void gpu_compare_ly()
{
    if(databus_read8(LY_ADDR) == databus_read8(LYC_ADDR))
    {
        databus_write8(STAT_ADDR, (databus_read8(STAT_ADDR) & ~0x04) | (0x04));
        if((databus_read8(STAT_ADDR) & 0b01000000) != 0)
        {
            gb_request_interrupt(INTERRUPT_STAT_MASK);
        }
    }
    else
    {
        databus_write8(STAT_ADDR, databus_read8(STAT_ADDR) & ~0x04);
    }
}

bool lcdc_get_bit(u8 mask)
{
    return (databus_read8(LCDC_ADDR) & mask) != 0;
}
void lcdc_set_bit(u8 mask, u8 shift, bool value)
{
    databus_write8(LCDC_ADDR, ((value << shift) | (databus_read8(LCDC_ADDR) & ~mask)));
    if(mask == 7 && !value)
    {
        // disabling display resets LY
        databus_write8(LY_ADDR, 0x00);
    }
}

u8 gpu_get_mode()
{
    return databus_read8(STAT_ADDR) & 0x03;
}
void gpu_set_mode(u8 mode)
{
    databus_write8(STAT_ADDR, (databus_read8(STAT_ADDR) & 0b11111100) | mode);
}
