/*
 * wdt_test.c
 *
 *  Created on: 2016-9-23
 *  Author: wxy
 */

#include "wdt.h"
#include "io.h"
#include "misc.h"

Bit32u wdttestflag=0;
void CK_WDT_Handler(Bit32u irqid)
{
	Bit32u temp,i;
	ITStatus_TypeDef state;

	state=WDT_GetITStatus();
	printf(" IT state:%x \n", state);
	temp=WDT_GetCNTValue();
	printf(" Counter Value is:%x \n", temp);

	WDT_ClearFlag();
	WDT_CNT_RStart();

	for(i=0;i<=4;i++)
	{
		temp=WDT_GetCNTValue();
		printf(" Counter Value is:%x \n", temp);
	}

	wdttestflag=1;
}

/*
 * GER_IT mode,set TOP_INIT and TOP
 *
 */
void WDT_Test1(void )
{
	wdttestflag=0;
	Bit32u temp,i;
	printf (" wdt test begins \n");
	WDT_Deactive();
	temp = reg_readl(WDT,WDT_TORR);
	printf(" TORR_1:%x \n", temp);
	WDT_Configuation(GER_IT,PCLK_CYC_2,10,10);
	WDT_CNT_RStart();
	temp = reg_readl(WDT,WDT_TORR);
	printf(" TORR_2:%x \n", temp);

	WDT_IT_Register();
	//printf("INTC Regis state is:%x \n", temp);

	WDT_Start();
	//temp = WDT->WDT_CR;
	//printf("CR_4:%x \n", temp);


	for(i=0;i<=4;i++)
	//for(;;)
	{
		temp=WDT_GetCNTValue();
		printf(" Counter Value is:%x \n", temp);
	}

	while(!wdttestflag);
	//CK_INTC_FreeIrq(CK_WDT_IRQ);
	printf(" wdt test success \n");
}

/*
 * GER_RST mode,set TOP_INIT and TOP
 *
 */
void WDT_Test2(void )
{
	Bit32u temp,i;
	//temp = WDT->WDT_CR;
	//printf("CR_1:%x \n", temp);
	WDT_Deactive();
	//temp = WDT->WDT_CR;
	//printf("CR_2:%x \n", temp);

	//temp = WDT->WDT_TORR;
	//printf("TORR_1:%x \n", temp);
	WDT_Configuation(GER_RST,PCLK_CYC_8,0,0);
	temp = WDT->WDT_CR;
	//printf("CR_3:%x \n", temp);
	temp = WDT->WDT_TORR;
	//printf("TORR_2:%x \n", temp);

	//printf("INTC Regis state is:%x \n", temp);

	WDT_Start();
	temp = WDT->WDT_CR;
	printf("CR_4:%x \n", temp);

	for(i=0;i<=4;i++)
	{
		temp=WDT_GetCNTValue();
		printf("Counter Value is:%x \n", temp);
	}

	while(1);

}

