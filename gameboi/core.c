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

#include <string.h>

static const char* instructions[] =
#include "instructions.h"


/*
 * ================= CORE =================
*/

void gb_core_initialize()
{
    gameboy->cpu->IME = FALSE;
}


/*
 * ================= CPU =================
*/

void cpu_execute(u16 address)
{
    u8 instr = cpu_read8(address);
    u8 cycles_add = 0;
    u8 pc_add = 0;
    
    struct gb_cpu* cpu = gameboy->cpu;
    
    printf("INSTRUCTION: (%s) at %#04x\n", instructions[instr], address);

    switch(instr)
    {
        case 0x00: // NOP
            cycles_add = 4;
            pc_add = 1;
            cpu_set_flags(FLAG_NO_MODIFY, FLAG_NO_MODIFY, FLAG_NO_MODIFY, FLAG_NO_MODIFY);
            break;
        case 0x01: // LD BC,d16
            cycles_add = 16;
            pc_add = 3;
            cpu->reg_BC = cpu_read16(address + 1);
            break;
        case 0x02: // LD (BC),A
            cycles_add = 8;
            pc_add = 1;
            cpu_write8(cpu->reg_BC, reg_get_high(&cpu->reg_AF));
            break;
        case 0x03: // INC BC
            cycles_add = 8;
            pc_add = 1;
            cpu->reg_BC++;
            break;
        case 0x04: // INC B
            cycles_add = 4;
            pc_add = 1;
            reg_set_high(&cpu->reg_BC, reg_get_high(&cpu->reg_BC) + 1);
            break;
        case 0x05: // DEC B
            cycles_add = 4;
            pc_add = 1;
            u8 sub_b = reg_get_high(&cpu->reg_BC) - 1;
            reg_set_high(&cpu->reg_BC, sub_b);
            // TODO: Modify half-carry flag properly
            cpu_set_flags(sub_b == 0, TRUE, FLAG_NO_MODIFY, FLAG_NO_MODIFY);
            break;
        case 0x06: // LD B,d8
            cycles_add = 8;
            pc_add = 2;
            reg_set_high(&cpu->reg_BC, cpu_read8(address + 1));
            break;
        case 0x07: // RLCA
            break;
        case 0x08: // LD (d16),SP
            cycles_add = 20;
            pc_add = 3;
            cpu_write16(cpu_read16(address + 1), cpu->reg_SP);
            break;
        case 0x0C: // INC C
            cycles_add = 4;
            pc_add = 1;
            reg_set_low(&cpu->reg_BC, reg_get_low(&cpu->reg_BC) + 1);
            break;
        case 0x0D: // DEC C
            cycles_add = 4;
            pc_add = 1;
            u8 sub_c = reg_get_low(&cpu->reg_BC) - 1;
            reg_set_low(&cpu->reg_BC, sub_c);
            // TODO: Modify half-carry flag properly
            cpu_set_flags(sub_c == 0, TRUE, FLAG_NO_MODIFY, FLAG_NO_MODIFY);
            break;
        case 0x0E: // LD C,d8
            cycles_add = 8;
            pc_add = 2;
            reg_set_low(&cpu->reg_BC, cpu_read8(address + 1));
            break;
        case 0x11: // LD DE,d16
            cycles_add = 12;
            pc_add = 3;
            cpu->reg_DE = cpu_read16(address + 1);
            break;
        case 0x13: // INC DE
            cycles_add = 8;
            pc_add = 1;
            cpu->reg_DE++;
            break;
        case 0x17: // RLA
            cycles_add = 4;
            pc_add = 1;
            reg_set_high(&cpu->reg_AF, cpu_logic_rl_through(reg_get_high(&cpu->reg_AF)));
            break;
        case 0x18: // JR r8
            cycles_add = 12;
            pc_add = 2;
            cpu->reg_PC += (signed char)(cpu_read8(address + 1));
            break;
        case 0x1A: // LD A,(DE)
            cycles_add = 8;
            pc_add = 1;
            reg_set_high(&cpu->reg_AF, cpu_read8(cpu->reg_DE));
            break;
        case 0x1E: // LD E,d8
            cycles_add = 8;
            pc_add = 2;
            reg_set_low(&cpu->reg_DE, cpu_read8(address + 1));
            break;
        case 0x20: // JR NZ,r8
            cycles_add = 8;
            pc_add = 2;
            if(!flag_get_zero())
            {
                cycles_add = 12;
                cpu->reg_PC += (signed char)(cpu_read8(address + 1));
            }
            break;
        case 0x21: // LD HL,d16
            cycles_add = 12;
            pc_add = 3;
            cpu->reg_HL = cpu_read16(address + 1);
            break;
        case 0x22: // LD (HL+),A
            cycles_add = 8;
            pc_add = 1;
            cpu_write8(cpu->reg_HL, reg_get_high(&cpu->reg_AF));
            cpu->reg_HL++;
            break;
        case 0x23: // INC HL
            cycles_add = 8;
            pc_add = 1;
            cpu->reg_HL++;
            break;
        case 0x28: // JR Z,r8
            cycles_add = 8;
            pc_add = 2;
            if(flag_get_zero())
            {
                cycles_add = 12;
                cpu->reg_PC += (signed char)(cpu_read8(address + 1));
            }
            break;
        case 0x2E: // LD L,d8
            cycles_add = 8;
            pc_add = 2;
            reg_set_low(&cpu->reg_HL, cpu_read8(address + 1));
            break;
        case 0x31: // LD SP,d16
            cycles_add = 12;
            pc_add = 3;
            cpu->reg_SP = cpu_read16(address + 1);
            break;
        case 0x32: // LD (HL-),A
            cycles_add = 8;
            pc_add = 1;
            cpu_write8(cpu->reg_HL, reg_get_high(&cpu->reg_AF));
            cpu->reg_HL--;
            break;
        case 0x3D: // DEC A
            cycles_add = 4;
            pc_add = 1;
            u8 sub_a = reg_get_high(&cpu->reg_AF) - 1;
            reg_set_high(&cpu->reg_AF, sub_a);
            // TODO: Modify half-carry flag properly
            cpu_set_flags(sub_a == 0, TRUE, FLAG_NO_MODIFY, FLAG_NO_MODIFY);
            break;
        case 0x3E: // LD A,d8
            cycles_add = 8;
            pc_add = 2;
            reg_set_high(&cpu->reg_AF, cpu_read8(address + 1));
            break;
        case 0x4F: // LD C,A
            cycles_add = 4;
            pc_add = 1;
            reg_set_low(&cpu->reg_BC, reg_get_high(&cpu->reg_AF));
            break;
        case 0x57: // LD D,A
            cycles_add = 4;
            pc_add = 1;
            reg_set_high(&cpu->reg_DE, reg_get_high(&cpu->reg_AF));
            break;
        case 0x67: // LD H,A
            cycles_add = 4;
            pc_add = 1;
            reg_set_high(&cpu->reg_HL, reg_get_high(&cpu->reg_AF));
            break;
        case 0x77: // LD (HL),A
            cycles_add = 8;
            pc_add = 1;
            cpu_write8(cpu->reg_HL, reg_get_high(&cpu->reg_AF));
            break;
        case 0x7B: // LD A,E
            cycles_add = 4;
            pc_add = 1;
            reg_set_high(&cpu->reg_AF, reg_get_low(&cpu->reg_DE));
            break;
        case 0xAF: // XOR A
            cycles_add = 4;
            pc_add = 1;
            reg_set_high(&cpu->reg_AF, 0);
            break;
        case 0xC1: // POP BC
            cycles_add = 12;
            pc_add = 1;
            cpu->reg_BC = cpu_read16(cpu->reg_SP);
            cpu->reg_SP += 2;
            break;
        case 0xC3: // JP d16
            cycles_add = 16;
            cpu->reg_PC = cpu_read16(address + 1);
            break;
        case 0xC5: // PUSH BC
            cycles_add = 16;
            pc_add = 1;
            cpu->reg_SP -= 2;
            cpu_write16(cpu->reg_SP, cpu->reg_BC);
            break;
        case 0xC9: // RET
            cycles_add = 16;
            cpu->reg_PC = cpu_read16(cpu->reg_SP);
            cpu->reg_SP += 2;
            break;
        case 0xCB: // PREFIX CB
            if(!cpu_execute_extended_instruction(address + 1, &cycles_add, &pc_add))
            {
                gameboy->stopped = TRUE;
                return;
            }
            break;
        case 0xCD: // CALL a16
            cycles_add = 24;
            cpu->reg_SP -= 2;
            cpu_write16(cpu->reg_SP, cpu->reg_PC + 3);
            cpu->reg_PC = cpu_read16(address + 1);
            break;
        case 0xD9: // RETI
            cycles_add = 16;
            pc_add = 1;
            cpu->reg_PC = cpu_read16(cpu->reg_SP);
            cpu->reg_SP += 2;
            cpu->IME = TRUE;
            break;
        case 0xE0: // LD ($FF00+a8),A
            cycles_add = 12;
            pc_add = 2;
            cpu_write8(0xFF00 + cpu_read8(address + 1), reg_get_high(&cpu->reg_AF));
            break;
        case 0xE2: // LD ($FF00+C),A
            cycles_add = 8;
            pc_add = 1;
            cpu_write8(0xFF00 + reg_get_low(&cpu->reg_BC), reg_get_high(&cpu->reg_AF));
            break;
        case 0xEA: // LD (a16),A
            cycles_add = 16;
            pc_add = 3;
            cpu_write8(cpu_read16(address + 1), reg_get_high(&cpu->reg_AF));
            break;
        case 0xF0: // LD A,($FF00+a8)
            cycles_add = 12;
            pc_add = 2;
            reg_set_high(&cpu->reg_AF, cpu_read8(0xFF00 + cpu_read8(address + 1)));
            break;
        case 0xF3: // DI
            cycles_add = 4;
            pc_add = 1;
            cpu->IME = FALSE;
            break;
        case 0xFB: // EI
            cycles_add = 4;
            pc_add = 1;
            cpu->IME = TRUE;
            break;
        case 0xFE: // CP d8
            cycles_add = 4;
            pc_add = 2;
            signed char result = reg_get_high(&cpu->reg_AF) - cpu_read8(address + 1);
            // TODO: Modify half-carry and carry flags properly
            cpu_set_flags(result == 0, TRUE, FLAG_NO_MODIFY, FLAG_NO_MODIFY);
            break;
        default:
            printf("CPU: Operand %#02x at %#04x not implemented! Aborting...\n", instr, address);
            gameboy->stopped = TRUE;
            return;
    }
    
    cpu->cycles += cycles_add;
    cpu->reg_PC += pc_add;
}

BOOL cpu_execute_extended_instruction(u16 address, u8* cycles_add, u8* pc_add)
{
    u8 instr = cpu_read8(address);
    
    struct gb_cpu* cpu = gameboy->cpu;
    
    printf("EXTENDED INSTRUCTION: (%#02x) at %#04x\n", /*instructions[0x00 + 0x100]*/instr, address);
    
    switch(instr)
    {
        case 0x11: // RL C
            *cycles_add = 8;
            *pc_add = 2;
            reg_set_low(&cpu->reg_BC, cpu_logic_rl_through(reg_get_low(&cpu->reg_BC)));
            break;
        case 0x7C: // BIT 7,H
            *cycles_add = 8;
            *pc_add = 2;
            flag_set_zero(!(reg_get_high(&cpu->reg_HL) & 0b10000000));
            break;
        default:
            printf("CPU: Extended Operand %#02x at %#04x not implemented! Aborting...\n", instr, address);
            return FALSE;
    }
    
    return TRUE;
}

/*
 * All 16-bit reads/writes are in little endian
*/
u8 cpu_read8(u16 address)
{
    return gameboy->memory_map[address];
}
u16 cpu_read16(u16 address)
{
    return *((u16*) (gameboy->memory_map + address));
}
void cpu_write8(u16 address, u8 value)
{
    gameboy->memory_map[address] = value;
}
void cpu_write16(u16 address, u16 value)
{
    *((u16*) (gameboy->memory_map + address)) = value;
}

u16 cpu_swap_endianess(u16 value)
{
    return ((value << 8) | (value >> 8));
}

/*
 * ================= LOGIC =================
 */

/*
 * Rotate into the carry flag
*/
u8 cpu_logic_rl_into(u8 value)
{
    BOOL carry = FALSE;
    if((value & 0b10000000) != 0)
    {
        carry = TRUE;
    }
    
    value <<= 1;
    
    value |= carry;
    flag_set_carry(carry);
    
    return value;
}
u8 cpu_logic_rr_into(u8 value)
{
    BOOL carry = FALSE;
    if((value & 0b00000001) != 0)
    {
        carry = TRUE;
    }
    
    value >>= 1;
    
    value |= (carry << 7);
    flag_set_carry(carry);
    
    return value;
}
/*
 * Rotate through the carry flag
*/
u8 cpu_logic_rl_through(u8 value)
{
    BOOL carry = FALSE;
    if((value & 0b10000000) != 0)
    {
        carry = TRUE;
    }
    
    value <<= 1;
    
    value |= flag_get_carry();
    flag_set_carry(carry);
    
    return value;
}
u8 cpu_logic_rr_through(u8 value)
{
    BOOL carry = FALSE;
    if((value & 0b00000001) != 0)
    {
        carry = TRUE;
    }
    
    value >>= 1;
    
    value |= (flag_get_carry() << 7);
    flag_set_carry(carry);
    
    return value;
}



/*
 * ================= REGISTERS =================
*/

u8 reg_get_high(u16* reg)
{
    return (*reg >> 8) & 0xFF;
}
void reg_set_high(u16* reg, u8 v)
{
    *reg = ((v << 8) | (*reg & 0xFF));
}
u8 reg_get_low(u16* reg)
{
    return *reg & 0xFF;
}
void reg_set_low(u16* reg, u8 v)
{
    *reg = ((*reg & 0xFF00) | v);
}

/*
 * ================= FLAGS =================
 */

void cpu_set_flags(signed char z, signed char n, signed char h, signed char c)
{
    if(z != FLAG_NO_MODIFY)
        flag_set_zero(z);
    if(n != FLAG_NO_MODIFY)
        flag_set_negative(n);
    if(h != FLAG_NO_MODIFY)
        flag_set_halfcarry(h);
    if(c != FLAG_NO_MODIFY)
        flag_set_carry(c);
}
void flag_set_zero(BOOL set)
{
    reg_set_low(&gameboy->cpu->reg_AF, ((set << FLAG_ZERO_SHIFT) | (reg_get_low(&gameboy->cpu->reg_AF) & ~FLAG_ZERO_MASK)));
}
BOOL flag_get_zero()
{
    return (reg_get_low(&gameboy->cpu->reg_AF) & FLAG_ZERO_MASK) != 0;
}

void flag_set_negative(BOOL set)
{
    reg_set_low(&gameboy->cpu->reg_AF, ((set << FLAG_NEGATIVE_SHIFT) | (reg_get_low(&gameboy->cpu->reg_AF) & ~FLAG_NEGATIVE_MASK)));
}
BOOL flag_get_negative()
{
    return (reg_get_low(&gameboy->cpu->reg_AF) & FLAG_NEGATIVE_MASK) != 0;
}

void flag_set_halfcarry(BOOL set)
{
    reg_set_low(&gameboy->cpu->reg_AF, ((set << FLAG_HALFCARRY_SHIFT) | (reg_get_low(&gameboy->cpu->reg_AF) & ~FLAG_HALFCARRY_MASK)));
}
BOOL flag_get_halfcarry()
{
    return (reg_get_low(&gameboy->cpu->reg_AF) & FLAG_HALFCARRY_MASK) != 0;
}

void flag_set_carry(BOOL set)
{
    reg_set_low(&gameboy->cpu->reg_AF, ((set << FLAG_CARRY_SHIFT) | (reg_get_low(&gameboy->cpu->reg_AF) & ~FLAG_CARRY_MASK)));
}
BOOL flag_get_carry()
{
    return (reg_get_low(&gameboy->cpu->reg_AF) & FLAG_CARRY_MASK) != 0;
}