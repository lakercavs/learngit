#ifndef __DW_SSI_H__
#define __DW_SSI_H__

#include "io.h"

struct 
 ssi_t{
	unsigned int ctrlr0;
    unsigned int ctrlr1;
	unsigned int ssienr;
	unsigned int mwcr;
	unsigned int ser;
	unsigned int baudr;
	unsigned int txftlr;
	unsigned int rxftlr;
	unsigned int txflr;
	unsigned int rxflr;
	unsigned int sr;
	unsigned int imr;
	unsigned int isr;
	unsigned int risr;
	unsigned int txoicr;
	unsigned int rxoicr;
	unsigned int rxuicr;
	unsigned int msticr;
	unsigned int icr;
	unsigned int dmacr;
	unsigned int dmatdlr;
	unsigned int dmardlr;
	unsigned int idr;
	unsigned int ssi_comp_version;
	unsigned int dr[36]; 
	unsigned int rx_sample_dly;
	unsigned int spi_ctrlr0;
} ;

int spi_read(void *buff, unsigned long addr, unsigned long size);
int spi_write(void *buff, unsigned long addr, unsigned long size);
int spi_flash_check(void);
int spi_init(void);
Bit32u spi_test(void);
#endif /* __DW_SSI_H__ */
