//
//  gpu.h
//  gameboi
//
//  Created by Ryan Terry on 8/25/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#ifndef gpu_h
#define gpu_h

#define LCDC_BG_DISPLAY         0b00000001
#define LCDC_OBJ_DISPLAY        0b00000010
#define LCDC_OBJ_BLOCK_SIZE     0b00000100
#define LCDC_BG_READ_ADDR       0b00001000
#define LCDC_BG_DATA_ADDR       0b00010000
#define LCDC_WINDOWING          0b00100000
#define LCDC_WINDOWING_AREA     0b01000000
#define LCDC_DISPLAY_ENABLED    0b10000000

#define LCDC_ADDR               0xFF40 // lcd controller
#define STAT_ADDR               0xFF41 // lcd status
#define SCY_ADDR                0xFF42 // display y-scroll
#define SCX_ADDR                0xFF43 // display x-scroll
#define LY_ADDR                 0xFF44 // scanline
#define LYC_ADDR                0xFF45 // scanline compare
#define DMA_ADDR                0xFF46 // DMA transfer offset
#define BGP_ADDR                0xFF47 // BG palette
#define OBP0_ADDR               0xFF48 // OBJ 1 palette
#define OBP1_ADDR               0xFF49 // OBJ 2 palette
#define WY_ADDR                 0xFF4A // window Y coord
#define WX_ADDR                 0xFF4B // window X coord

#include "integers.h"


struct gb_gpu
{
    int display_mode_cycles;
};

void gpu_initialize(struct gb_gpu* gpu);
void gpu_teardown(struct gb_gpu* gpu);

void gpu_tick(struct gb_gpu* gpu, int lastCycles);
u8 gpu_nextline(struct gb_gpu* gpu);

bool lcdc_get_bit(u8 mask);
void lcdc_set_bit(u8 mask, u8 shift, bool value);
u8 gpu_get_mode();
void gpu_set_mode(u8 mode);
void gpu_compare_ly();

#endif /* gpu_h */
