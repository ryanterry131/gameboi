//
//  dataio.c
//  gameboi
//
//  Created by Ryan Terry on 8/26/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#include <string.h>

#include "dataio.h"

void read8(u8* destination, const byte* src, int offset)
{
    *destination = src[offset];
}

void read16(u16* destination, const byte* src, int offset)
{
    memcpy(destination, src + offset, sizeof(*destination));
}

void read32(u32* destination, const byte* src, int offset)
{
    memcpy(destination, src + offset, sizeof(*destination));
}

void readChars(char* destination, const byte* src, int offset, int bytes)
{
    memcpy(destination, src + offset, bytes);
}

void readBytes(byte* destination, const byte* src, int offset, int bytes)
{
    memcpy(destination, src + offset, bytes);
}