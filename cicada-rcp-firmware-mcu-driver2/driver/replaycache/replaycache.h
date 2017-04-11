#ifndef __REPLAY_CACHE_H__
#define __REPLAY_CACHE_H__

#include "types.h"

#define REPLAY_CACHE_REG_BASE          0x197E0000

#define REPLAY_CACHE_FLUSH_OFFSET      0x00
#define REPLAY_CACHE_BV_OFFSET         0x04
#define REPLAY_CACHE_SRAM_INIT_OFFSET  0x08

Bit32u ReplayCache_Flush();

Bit32u ReplayCache_ByteValid_Check();

Bit32u ReplayCache_SRAM_Init_Check();

#endif
