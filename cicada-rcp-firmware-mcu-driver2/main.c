/*
 * main.c - main function Modulation.
 *
 * Copyright (C): 2008~2010 Hangzhou C-SKY Microsystem Co.,LTD.
 * Author: Liu Bing  (bing_liu@c-sky.com)
 * Contributor: Liu Bing
 * Date: 2010-6-28
 *
 */


#include "datatype.h"
#include "ck810.h"
#ifdef CONFIG_CPU_HAS_FPU
#include "fpu.h"
#endif
#include "uart.h"
#include "misc.h"
#include "rtc.h"
#include "dma.h"
#include "gpio.h"
#include "i2c.h"
#include "intc.h"
#include "timer.h"
#include "wdt.h"
#include "pci_tlp.h"

CK_Uart_Device consoleuart = CONFIG_TERMINAL_UART;
extern void CK_Exception_Init (void);
extern void CK_INTC_Init(void);
extern void CK_UART_Test();
extern volatile unsigned int reset_status;

/*
 * initialize the device registered
 */
static void CK_Drivers_Init(void)
{
	CK_Uart_DriverInit();
    CK_INTC_Init();
}


static void CK_Console_CallBack(CK_INT8 error)
{
  if(error==CK_Uart_CTRL_C)
  {
    CK_UART_ClearRxBuffer(consoleuart);
  }
}

static void CK_Console_Init()
{
  CK_Uart_Open(consoleuart,CK_Console_CallBack);
}

void IP_Test(void)
{

	printf ("Step1:Test rtc\n");
	RTC_Test2();

	printf ("Step2:Test timer\n");
	CK_Timer_Test();

	printf ("Step3:Test dma signal \n");
	test_dma_singal_allchan(1);

	printf ("Step4:Test dma lli \n");
	test_dma_lli_allchan(1);

	//printf ("Step5:Test IIC \n");   // not available for Simulation
	//I2C_Test_For_EEPROM();

	printf ("Step6:Test gpio\n");
	GPIO_test2();

	printf ("Step7:Test wdt\n");
	WDT_Test1();

}

/*
 * the main function of Uart demo project
 */
int main ( void )
{
#if 1
	if (reset_status == 0x12345678)
    {
        /* @todo warm reset */
    }
    else
    {
        /* @todo cold reset */
        reset_status = 0x12345678;
    }
#endif

#ifdef CONFIG_CPU_HAS_FPU
	init_fpu();
#endif

    CK_Drivers_Init();
    CK_Exception_Init();
    CK_Console_Init();
    GPIO_input_set();   //GPIO set input all

    printf("Begin to test IP\n");
	//pci_altera_rc_test();
    //CK_UART_Test();
	//RTC_Test2();
	CK_Timer_Test();
    //test_dma_singal_allchan(8);
    //test_dma_lli_allchan(8);
	//I2C_Test_For_EEPROM(); //not available for Simulation
    GPIO_test2();
    WDT_Test1();
	//pci_altera_rc_test();
    CK_UART_Debug();

    //while(1);
    return 0;
}
