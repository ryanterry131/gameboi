//
//  core.h
//  gameboi
//
//  Created by Ryan Terry on 8/25/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#ifndef core_h
#define core_h

#define FLAG_NO_MODIFY -1

#include "integers.h"

struct gb_cpu
{
    u16 reg_AF,
        reg_BC,
        reg_DE,
        reg_HL;
    
    u16 reg_SP,
        reg_PC;

    int cycles;
};

void cpu_initialize(struct gb_cpu*);

int cpu_execute(struct gb_cpu*, u16 address);
bool cpu_execute_extended_instruction(struct gb_cpu*, u16 address, u8* cycles_add, u8* pc_add);

// logic
u8 cpu_logic_rl_into(u8 value);
u8 cpu_logic_rr_into(u8 value);
u8 cpu_logic_rl_through(u8 value);
u8 cpu_logic_rr_through(u8 value);
// registers
u8 reg_get_high(u16* reg);
u8 reg_get_low(u16* reg);
void reg_set_high(u16* reg, u8 v);
void reg_set_low(u16* reg, u8 v);
// flags
void cpu_set_flags(signed char z, signed char n, signed char h, signed char c);
void flag_set_zero(bool set);
bool flag_get_zero();
void flag_set_negative(bool set);
bool flag_get_negative();
void flag_set_halfcarry(bool set);
bool flag_get_halfcarry();
void flag_set_carry(bool set);
bool flag_get_carry();

#endif /* core_h */
