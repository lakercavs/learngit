/*
 * Description: ck5a6.h - Define the system configuration, memory & IO base
 * address, flash size & address, interrupt resource for ck5a6 soc.
 *
 * Copyright (C) : 2008 Hangzhou C-SKY Microsystems Co.,LTD.
 * Author(s): Liu Bing (bing_liu@c-sky.com)
 * Contributors: Liu Bing
 * Date:  2010-06-26
 * Modify by liu jirang  on 2012-09-11
 */

#ifndef __INCLUDE_CK5A6_H
#define __INCLUDE_CK5A6_H

#include "config.h"

/************************************************
 * perpheral module baseaddress and irq number
 ***********************************************/
/**** off-chip Nor FLASH ****/
#define FLASH_START					(volatile CK_UINT32 *)(0x00000000+PERI_BASE)
#define FLASH_END					(volatile CK_UINT32 *)(0x00800000+PERI_BASE)

/** off-chip SDRAM **/
#define MMC_SCONR					(volatile CK_UINT32 *)(0x08000000+PERI_BASE)

/**** AHB ****/
/**** AHB arbiter ****/
#define CK_AHBBUS_BASE				(volatile CK_UINT32 *)(0x19000000+PERI_BASE)

/** MMC Register **/
#define MMC_Register				(volatile CK_UINT32 *)(0x10001000+PERI_BASE)

/****** POWM  *************/
#define CK_POWM_ADDRBASE			(volatile CK_UINT32 *)(0x10002000+PERI_BASE)

/** Define DMA control base address **/
#define CK_DMAC_CONTROL_BASEADDR	(volatile CK_UINT32 *)(0x10003000+PERI_BASE)

/****** USBD  *************/
#define CK_USBD_ADDRBASE			(volatile CK_UINT32 *)(0x10005000+PERI_BASE)

/**** MAC *******/
#define CKMAC_BASEADDR				(volatile CK_UINT32 *)(0x10006000+PERI_BASE) /* mac base address */

/****** NFC  *************/
#define CK_NFC_ADDRBASE				(volatile CK_UINT32 *)(0x10008000+PERI_BASE)

/****** USBH  *************/
#define CK_USBH_ADDRBASE			(volatile CK_UINT32 *)(0x1000B000+PERI_BASE)

/****** SDHC  *************/
#define CK_SDHC_ADDRBASE			(volatile CK_UINT32 *)(0x1000C000+PERI_BASE)


/**** APB ****/
/***** Intc ******/
#define CK_INTC_BASEADDRESS			(0x19000000+PERI_BASE)

/****** SPI *************/
#define CK_SPI_ADDRBASE				(0x19010000+PERI_BASE)

/***** Uart *******/
#define CK_UART_ADDRBASE0			(volatile CK_UINT32 *)(0x19020000+PERI_BASE)
#define CK_UART_ADDRBASE1			(volatile CK_UINT32 *)(0x19026000+PERI_BASE)

/**** Timer ****/
#define  CK_TIMER0_BASSADDR			(volatile CK_UINT32 *)(0x19030000+PERI_BASE)
#define  CK_TIMER1_BASSADDR			(volatile CK_UINT32 *)(0x19030020+PERI_BASE)
#define  CK_TIMER2_BASSADDR			(volatile CK_UINT32 *)(0x19030040+PERI_BASE)
#define  CK_TIMER3_BASSADDR			(volatile CK_UINT32 *)(0x19030060+PERI_BASE)

/***** GPIO *****/
#define PCK_GPIO_Control			(volatile CK_UINT32 *)(0x19040000+PERI_BASE)
#define GPIOA_BASE                  (volatile CK_UINT32 *)(0x19040000+PERI_BASE)
#define GPIOB_BASE                  (volatile CK_UINT32 *)(0x1904000C+PERI_BASE)
#define GPIOC_BASE                  (volatile CK_UINT32 *)(0x19040018+PERI_BASE)
#define GPIOD_BASE                  (volatile CK_UINT32 *)(0x19040024+PERI_BASE)
#define GPIO_BASE                   (volatile CK_UINT32 *)(0x19040030+PERI_BASE)

/****** I2C ******/
#define APB_I2C_BASEADDR            (volatile CK_UINT32 *)(0x19050000+PERI_BASE)

/****** RTC*************/
#define CK_RTC_ADDRBASE				(volatile CK_UINT32 *)(0x19060000+PERI_BASE)

/****** WDT *************/
#define CK_WDT_ADDRBASE				(volatile CK_UINT32 *)(0x19070000+PERI_BASE)
#define CK_WDT_ADDRBASE_1           (volatile CK_UINT32 *)(0x190700E4+PERI_BASE)

/****** DMA ******/
#define APB_DMA_BASEADDR            (volatile CK_UINT32 *)(0x19800000+PERI_BASE)
#define APB_DMA_CH(x)               (volatile CK_UINT32 *)(0x19800000+PERI_BASE + ((x) << 8))

/****** PCIE *****/
#define CK_PCIE_EP_ADDRBASE 		(volatile CK_UINT32 *)(0x1A000000+PERI_BASE)

#define CMDQ_INTC_BASE         (0x19000000+0x7F0000+PERI_BASE)
#define PCM_MEM_BASE0         ((volatile CK_UINT32 *)(0x8000000+PERI_BASE))
#define PCM_MEM_BASE1         ((volatile CK_UINT32 *)(0xC0000000+PERI_BASE))
#define CMDQ_INTC_MTA(n)      ((volatile CK_UINT32 *)(CMDQ_INTC_BASE+0x4000+(n<<2)))
#define CMDQ_INTC_MASK        ((volatile CK_UINT32 *)(CMDQ_INTC_BASE+0x4020))
#define CMDQ_INTC_STATUS      ((volatile CK_UINT32 *)(CMDQ_INTC_BASE+0x4024))
#define PCM_MAP_ADDR_HI       ((volatile CK_UINT32 *)(0x1908020c+PERI_BASE))
#define PCM_MAP_ADDR_LOW      ((volatile CK_UINT32 *)(0x19080208+PERI_BASE))
#define PCM_MAP_ADDR1G_HI     ((volatile CK_UINT32 *)(0x19080214+PERI_BASE))
#define PCM_MAP_ADDR1G_LOW    ((volatile CK_UINT32 *)(0x19080210+PERI_BASE))
#define MSI_INT_NUM   (0)

#define phy2virt(addr)  ((addr)+PERI_BASE)

/*
 * define irq number of perpheral modules
 */
#define  CK_INTC_UART0          0
#define  CK_INTC_SPI            1
#define  CK_INTC_TIM0           2
#define  CK_INTC_TIM1           3
#define  CK_INTC_TIM2           4
#define  CK_INTC_TIM3           5
#define  CK_INTC_DMAC           6
#define  CK_INTC_PCIECMDQ0      7
#define  CK_INTC_PCIECMDQ1      8
#define  CK_INTC_PCIECMDQ2      9
#define  CK_INTC_PCIECMDQ3      10
#define  CK_INTC_PCIECMDQ4      11
#define  CK_INTC_PCIECMDQ5      12
#define  CK_INTC_PCIECMDQ6      13
#define  CK_INTC_PCIECMDQ7      14
#define  CK_INTC_GPIOA          15
#define  CK_INTC_IIC            16
#define  CK_INTC_RTC            17
#define  CK_INTC_Watchdog       18
#define  CK_INTC_CMDRESQ        19
#define  CK_INTC_RP_CACHE       20
#define  CK_INTC_ATL            21


#define SAVE_PCM_MAP_REG() do{\
	extern CK_UINT64 gSavedAddrMapReg;\
   gSavedAddrMapReg = *(volatile CK_UINT64 *)PCM_MAP_ADDR_LOW;\
}while(0)

#define RESTORE_PCM_MAP_REG() do{\
	extern CK_UINT64 gSavedAddrMapReg;\
  *(volatile CK_UINT64 *)PCM_MAP_ADDR_LOW =  gSavedAddrMapReg;   \
}while(0)
/* aligned 256M */
#define MAP_PCM_ADDR1(hiaddr, lowaddr, mask32) \
do{\
    *PCM_MAP_ADDR_LOW = (hiaddr);\
    *PCM_MAP_ADDR_HI = (lowaddr) & (mask32);\
}while(0)


#define MAP_PCM_ADDR(addr64, mask32) \
do{\
    *PCM_MAP_ADDR_LOW = ((CK_UINT64)addr64) >> 32;\
    *PCM_MAP_ADDR_HI = ((CK_UINT64)addr64 & 0x00000000ffffffff) & (mask32);\
}while(0)

#define MAP_PCM_ADDR_1G_MASK  (0x3fffffff)
#define MAP_PCM_ADDR_1G(addr64, mask32) \
do{\
    *PCM_MAP_ADDR1G_LOW = ((CK_UINT64)addr64) >> 32;\
    *PCM_MAP_ADDR1G_HI = ((CK_UINT64)addr64 & 0x00000000ffffffff) & (mask32);\
}while(0)


#endif /* __INCLUDE_CK5A6_H */
