//
//  dataio.h
//  gameboi
//
//  Created by Ryan Terry on 8/26/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#ifndef dataio_h
#define dataio_h

#include <stdio.h>

#include "integers.h"

void read8(u8* destination, const byte* src, int offset);
void read16(u16* destination, const byte* src, int offset);
void read32(u32* destination, const byte* src, int offset);
void readChars(char* destination, const byte* src, int offset, int bytes);
void readBytes(byte* destination, const byte* src, int offset, int bytes);

#endif /* dataio_h */
