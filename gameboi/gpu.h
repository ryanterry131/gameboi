//
//  gpu.h
//  gameboi
//
//  Created by Ryan Terry on 8/25/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#ifndef gpu_h
#define gpu_h

#define LCDC_ENABLED_MASK   0b10000000

#define LCDC_ADDR           0xFF40
#define LCDC_Y_ADDR         0xFF44

#include "integers.h"

struct gb_gpu
{
    u8 display_mode;
    int display_mode_cycles;
};

void gpu_initialize(struct gb_gpu* gpu);
void gpu_tick(struct gb_gpu* gpu, int lastCycles);

u8 gpu_nextline(struct gb_gpu* gpu);
bool gpu_display_enabled();

#endif /* gpu_h */
