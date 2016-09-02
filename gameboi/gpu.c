//
//  gpu.c
//  gameboi
//
//  Created by Ryan Terry on 8/25/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#include "gpu.h"
#include "databus.h"

#include <stdio.h> // for printf
#include <math.h> // for floor

void gpu_initialize(struct gb_gpu* gpu)
{
    gpu->display_mode = 0x02;
    databus_write8(LCDC_Y_ADDR, 0x00);
}

void gpu_tick(struct gb_gpu* gpu, int lastCycles)
{
    if(gpu_display_enabled())
    {
        gpu->display_mode_cycles += lastCycles;
        switch(gpu->display_mode)
        {
            case 0x00: // H-Blank
                // TODO: this code has an error in that it runs h-blank prior to v-blank at the start of v-blank (too many cycles taken up in v-blank)
                if(gpu->display_mode_cycles >= 204)
                {
                    // carry overflown cycles into next display mode
                    gpu->display_mode_cycles %= 204;
                    
                    if(gpu_nextline(gpu) >= 144)
                    {
                        // enter v-blank after 144 lines
                        gpu->display_mode = 0x01;
                    }
                    else
                    {
                        // no v-blank yet; jump to the next line instead.
                        gpu->display_mode = 0x02;
                    }
                }
                break;
            case 0x01: // V-Blank
                // checks if we've passed a multiple of 456 (scanline cycle)
                if((floor(gpu->display_mode_cycles / 456) + 144) > databus_read8(LCDC_Y_ADDR))
                {
                    if(gpu_nextline(gpu) > 153)
                    {
                        // reset from line 0
                        databus_write8(LCDC_Y_ADDR, 0x00);
                        gpu->display_mode_cycles %= 4560;
                        gpu->display_mode = 0x02;
                    }
                }
                break;
            case 0x02: // OAM access
                if(gpu->display_mode_cycles >= 80)
                {
                    gpu->display_mode_cycles %= 80;
                    gpu->display_mode = 0x03;
                }
                break;
            case 0x03: // Data transfer to LCD driver
                if(gpu->display_mode_cycles >= 172)
                {
                    gpu->display_mode_cycles %= 172;
                    gpu->display_mode = 0x00;
                }
                break;
        }
    }
}

u8 gpu_nextline(struct gb_gpu* gpu)
{
    u8 nextLine = databus_read8(LCDC_Y_ADDR) + 1;
    databus_write8(LCDC_Y_ADDR, nextLine);
    return nextLine;
}

bool gpu_display_enabled()
{
    return (databus_read8(LCDC_ADDR) & LCDC_ENABLED_MASK) != 0;
}
