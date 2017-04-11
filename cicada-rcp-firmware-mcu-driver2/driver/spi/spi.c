#include "types.h"
#include "ck810.h"
#include "spi.h"
#include "misc.h"

//#define SSI ((volatile SSI_t *)CK_SPI_ADDRBASE)
#define BAUD_DIV 4

#define SWAP32(data) ((unsigned int)(   \
            (((data)&0xff) << 24)  \
            | (((data)&0xff00) << 8)  \
            | (((data)&0xff0000) >> 8)  \
            | (((data)&0xff000000) >> 24)  \
            ))

struct ssi_t* SSI = (volatile struct ssi_t *)CK_SPI_ADDRBASE;

#define min(a,b) ((a) > (b)?(b):(a))

#define RESET_CMD(_ssi)  do { \
    while(((_ssi)->sr & 0x1) == 0x01); \
    (_ssi)->ser = 0; \
}while(0)
	
// RESET_CMD must be called,before call SEND_CMD
#define SEND_CMD(_ssi) do { \
    while(((_ssi)->sr & 0x1) == 0x01); \
    (_ssi)->ser = 1; \
    while(((_ssi)->sr & 0x1) == 0x01); \
}while(0)

#define disable_chip(_ssi) do {  \
        RESET_CMD(_ssi); \
        (_ssi)->ssienr = 0; \
    } while (0) 

#define enable_chip(_ssi) (_ssi)->ssienr = 1

void flash_reset()
{
    disable_chip(SSI);
    SSI->spi_ctrlr0 = 0
            | 0b00 <<  0  // trans_type=addr,data in the mode specified byCTRLR0.SPI_FRF
            | 0b10 <<  8  // 8bit cmd
            | 0 << 11     // wait 0cycle
            ;

    SSI->ctrlr0 =  0
        | (0<<4)  /*motolar spi*/
        | (0b01<<8) /*TMOD:tx+rx:00 01<--todev 10<--tohost*/
        | (0<<11) 
        | ((7) << 16) /*8bit data  */
        | (0b00 << 21) /*frf:normal format*/
        ;
    enable_chip(SSI);

    // write enable
    SSI->dr[0] = 0x66;
    SEND_CMD(SSI);

    // send cmd
    RESET_CMD(SSI);
    SSI->dr[0] = 0x99;
    SEND_CMD(SSI);
}

void write_enable()
{
    SSI->ssienr = 0x0;//设置前禁止ssI
    SSI->baudr = BAUD_DIV;
    SSI->ctrlr0 =  0
        | (0<<4)  /*motolar spi*/
        | (0b01<<8) /*tx+rx:00 01<--todev 10<--tohost*/
        | (0<<11) 
        | 0b00 << 21 /*stand spi mode*/
        | (7 << 16) /* */
        ;    
    SSI->dmacr = 0;
    SSI->imr = 0;
    SSI->ser = 0x1;
    SSI->ctrlr1 = 0;
    SSI->ssienr = 0x1;

    // write enable 
    SSI->dr[0] = 0x06;
    while((SSI->sr & 0x1) == 0x01);
}

unsigned char flash_reg_write8(unsigned char cmd, unsigned char value)
{
    // 0x05
    SSI->ssienr = 0x0;//设置前禁止ssI

#if 1
    SSI->spi_ctrlr0 = 0b00 << 0  // trans_type=addr,data in stand mode
        | 2 <<  8 // 8bit cmd
        | 0 << 11  // wait 0cycle
        ;
#endif

    write_enable();

    SSI->ssienr = 0x0;//设置前禁止ssI
    SSI->baudr = BAUD_DIV;
    SSI->ctrlr0 =  0
        | (0<<4)  /*motolar spi*/
        | (0b01<<8) /*tx+rx:00 01<--todev 10<--tohost*/
        | (0<<11) 
        | 0b00 << 21 /*stand spi mode*/
        | (15 << 16) /* */
        ;    
    SSI->dmacr = 0;
    SSI->imr = 0;
    SSI->ser = 0x1;
    SSI->ctrlr1 = 0;
    SSI->ssienr = 0x1;

    // write enable 
    SSI->dr[0] = cmd << 8|value;
    while((SSI->sr & 0x1) == 0x01);
	
	return 0;
}

static int flash_read(unsigned int flashaddr, void*buf, int readbytes)
{
    int frms_peer_loop,i;
    int *ptr = (int*)buf;
    int MAX_FRMS_ONETIME = 16;
    int cntof4byte = readbytes/sizeof(int);
    int less4byte = readbytes%4;
    int frm_cnt = 0;
    int data;

    SSI->ssienr = 0x0;//设置前禁止ssI
    SSI->baudr = BAUD_DIV;
    SSI->ctrlr0 =  0
        | (0<<4)  /*motolar spi*/
        | (0b11<<8) /*tx+rx:00, 01<--todev, 10<--tohost,11<-eeprom read*/
        | 0b00 << 21 /*stand spi mode*/
        | (31 << 16) /* read 32bit onetime(rd) 32bit one frame */
        ;    
    SSI->dmacr = 0;
    SSI->imr = 0;
    SSI->ser = 0x1;
    SSI->ssienr = 0x1;


    while(frm_cnt < cntof4byte){
        // set regs
        frms_peer_loop = min(cntof4byte-frm_cnt, MAX_FRMS_ONETIME);
        SSI->ssienr = 0x0;//
        SSI->ctrlr1 = frms_peer_loop-1;
        SSI->ssienr = 0x1;//

        // send read cmd
        SSI->dr[0] = (0x03 << 24) | flashaddr;
        while((SSI->sr & 0x1) == 0x01);

        // read data
        for (i = 0; i < frms_peer_loop; i++) {
            while((SSI->sr & (1<<3)) == 0x0);// receive fifo is empty
            while((SSI->sr & (1<<0)) != 0x0);// data is tranfer
            data = SSI->dr[0];
            *ptr = SWAP32(data);
            ptr++;
        }


        flashaddr += sizeof(int)*frms_peer_loop;
        frm_cnt+=frms_peer_loop;
    }

    // less than 4byte(32bit)
    if(less4byte>0){
        SSI->ssienr = 0x0;//设置前禁止ssI
        SSI->ctrlr1 = 0;
        SSI->ssienr = 0x1;//

        SSI->dr[0] = (0x03 << 24) | flashaddr;
        while((SSI->sr & 0x1) == 0x01);

        while((SSI->sr & (1<<3)) == 0x0);// receive fifo is empty
        while((SSI->sr & (1<<0)) != 0x0);// data is tranfer
        data = SSI->dr[0];
        data = SWAP32(data);

        for (i = 0; i < less4byte; i++) {
            ((unsigned char*)ptr)[i] =  (data >> (i*8)) & 0xff;
        }
    }

    return readbytes;
}
static unsigned int read_id()
{
    unsigned int data;
    unsigned int id = 0;

    SSI->ssienr = 0x0;//设置前禁止ssI

    SSI->spi_ctrlr0 = 0b00 << 0  // trans_type=addr,data in stand mode
        | 2 <<  8 // 8bit cmd
        | 0 << 11  // wait 0cycle
        ;

    SSI->baudr = BAUD_DIV;
    SSI->ctrlr0 =  0
        | (0<<4)  /*motolar spi*/
        | (0b11<<8) /*tx+rx:00 01<--todev 10<--tohost*/
        | (0<<11) 
        | 0b00 << 21 /*stand spi mode*/
        | (7 << 16) /* */
        ;
    SSI->dmacr = 0;
    SSI->imr = 0;
    SSI->ser = 0x1;
    SSI->ctrlr1 = 3;
    SSI->ssienr = 0x1;

    SSI->dr[0] = 0x9e;
    while((SSI->sr & 0x1) == 0x01);

    data = SSI->dr[0]; 
    id = data & 0xff;
    data = SSI->dr[0]; 
    id = (id << 8) | (data & 0xff);
    data = SSI->dr[0]; 
    id = (id << 8) | (data & 0xff);

    return id;
}

int spi_read(void *buff, unsigned long addr, unsigned long size)
{
    int read_bytes;
	
    if (addr < 0x1000000)
		flash_reg_write8(0xc5, 0);
	else
	{
		flash_reg_write8(0xc5, 1);
		addr -= 0x1000000;
	}
	
	read_bytes = flash_read(addr, buff, size);
		
	return read_bytes;
}

int spi_write(void *buff, unsigned long addr, unsigned long size)
{
	/*
	Bit32u i;
	Bit32u *dest_addr = (Bit32u *)addr;
	for(i=0; i< size; i++)
		*dest_addr++ = *buff++;
	*/
	return size;
}

int spi_flash_check(void)
{
    unsigned int id = read_id();

    if (id == 0x20bb19)
    {
        return 0;
    }
    else
    {
        printf("SPI flash check failed \n");
        printf("read id is %x \n",id);
        return -1;
    }

    return 0;
}

int spi_init(void)
{
    printf("Initialize SPI flash ...\r\n");    
    return 0;
}
