#include "replaycache.h"

Bit32u ReplayCache_Flush()
{
	*(Bit32u *)(REPLAY_CACHE_REG_BASE + REPLAY_CACHE_FLUSH_OFFSET) = 1;
    while(*(Bit32u *)(REPLAY_CACHE_REG_BASE + REPLAY_CACHE_FLUSH_OFFSET));
    return 0;
}

Bit32u ReplayCache_ByteValid_Check()
{
     return *(Bit32u *)(REPLAY_CACHE_REG_BASE + REPLAY_CACHE_BV_OFFSET);
}

Bit32u ReplayCache_SRAM_Init_Check()
{
	return *(Bit32u *)(REPLAY_CACHE_REG_BASE + REPLAY_CACHE_SRAM_INIT_OFFSET);
}
