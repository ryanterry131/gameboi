//
//  integers.h
//  gameboi
//
//  Created by Ryan Terry on 8/26/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#ifndef integers_h
#define integers_h

#include <stdint.h>

#define FLAG_ZERO_MASK          0b10000000
#define FLAG_ZERO_SHIFT         0x00000007
#define FLAG_NEGATIVE_MASK      0b01000000
#define FLAG_NEGATIVE_SHIFT     0x00000006
#define FLAG_HALFCARRY_MASK     0b00100000
#define FLAG_HALFCARRY_SHIFT    0x00000005
#define FLAG_CARRY_MASK         0b00010000
#define FLAG_CARRY_SHIFT        0x00000004

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint8_t byte;

#define FALSE 0
#define TRUE 1
typedef byte BOOL;

#endif /* integers_h */
