//
//  core.h
//  gameboi
//
//  Created by Ryan Terry on 8/25/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#ifndef core_h
#define core_h

#include <stdio.h>

#include "integers.h"

struct z80
{
    u16 reg_AF,
        reg_BC,
        reg_DE,
        reg_HL;
    
    u16 reg_SP,
        reg_PC;
    
    int cycles;
};

void cpu_execute(u16 address);

u8 cpu_read8(u16 address);
u16 cpu_read16(u16, BOOL);

void gb_core_initialize();

#endif /* core_h */
