//
//  databus.h
//  gameboi
//
//  Created by Ryan Terry on 8/26/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#ifndef databus_h
#define databus_h

#include "integers.h"

u8 databus_read8(u16 address);
u16 databus_read16(u16 address);
void databus_write8(u16 address, u8 value);
void databus_write16(u16 address, u16 value);
u16 databus_swap_endianess(u16 value);

#endif /* databus_h */
