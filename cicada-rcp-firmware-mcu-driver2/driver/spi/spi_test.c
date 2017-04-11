#include "spi.h"
#include "io.h"

Bit32u spi_test(void)
{
	Bit32u b[100]={0};
	Bit32u i = 0;
	Bit32u addr = 0;
	spi_init();
	spi_flash_check();
	spi_read(b, addr, 100);
	for(i=0;i<100;i++)
	{
		printf("Read data:%x \n",b[i]);
	}
	return 0;
}
