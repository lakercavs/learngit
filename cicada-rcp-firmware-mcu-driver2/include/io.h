#ifndef __IO_H__
#define __IO_H__

#include "types.h"

static inline Bit8u readb(const volatile void *addr)
{
    return *(const volatile Bit8u *)addr;
}

static inline void writeb(Bit8u value, volatile void *addr)
{
    *(volatile Bit8u *)addr = value;
}

static inline Bit16u readw(const volatile void *addr)
{
    return *(const volatile Bit16u *)addr;
}

static inline void writew(Bit16u value, volatile void *addr)
{
    *(volatile Bit16u *)addr = value;
}

static inline Bit32u readl(const volatile void *addr)
{
    return *(const volatile Bit32u *)addr;
}

static inline void writel(Bit32u value, volatile void *addr)
{
    *(volatile Bit32u *)addr = value;
}

static inline void dw_write(Bit32u val, void* addr)
{
	Bit32u value;
	value =readl(addr);
	value |= val;
	writel(value, addr);
}

static inline void dw_clear(Bit32u val, void* addr)
{
	Bit32u value;
	value =readl(addr);
	value &= ~val;
	writel(value, addr);
}

#define reg_readl(dw, name)                  readl((void *)&(dw->name))
#define reg_writel(val, dw, name)            dw_write((val), (void *)&(dw->name))
#define reg_clearl(val, dw, name)            dw_clear((val), (void *)&(dw->name))
#define reg_direct_writel(val, dw, name)     writel((val), (void *)&(dw->name))


/*
static inline void hexdump(void *_data, Bit32u size){
    const Bit8u *data = (const Bit8u *)_data;
    Bit32u offset = 0, i;
    printf("0x%x\n", _data);
    while (offset < size) {
        printf("0x%x  ", offset);
        Bit32u n = size - offset;
        if (n > 16) {
            n = 16; 
        }   

        for (i = 0; i < 16; ++i) {
            if (i == 8) {
                printf(" ");
            }   

            if (offset + i < size) {
                printf("%x ", data[offset + i]);
            } else {
                printf("   ");            }   
        }   
        printf(" ");

        for (i = 0; i < n; ++i) {
            if (isprint(data[offset + i])) {
                printf("%c", data[offset + i]);
            } else {
                printf(".");
            }   
        }   
        printf("\n");
        offset += 16; 
    } 
}
*/

#endif /*__IO_H__*/
