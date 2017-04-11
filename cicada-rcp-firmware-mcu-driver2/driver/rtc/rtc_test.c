/*
 * rtc_test.c
 *
 *  Created on: 2016-9-23
 *  Author: wxy
 */
#include "rtc.h"
#include "io.h"
#include "misc.h"

Bit32u rtctestflag=0;
/*User modify*/
void CK_RTC_Handler(Bit32u irqid)
{
	Bit32u i,temp;
	ITStatus_TypeDef state;
	state=RTC_GetITRStat();
	printf("IT raw state:%x \n", state);
	state=RTC_GetITStat();
	printf("IT state:%x \n", state);
	for(i=0;i<5;i++)
	{
		temp=RTC_GetCNTValue();
		printf("Counter Value:%x \n", temp);

	}
	RTC_ClearITFlag();
	rtctestflag=1;
}


/*rtc_en mode ,interrupt musked and disabled,set load value and match value
 *
 */
void RTC_Test1(void )
{
	Bit32u temp,i;
	temp = RTC->RTC_CCR;
	printf("CCR_1 :%x \n", temp);
	RTC_Deactive();
	temp = RTC->RTC_CCR;
	printf("CCR_2 :%x \n", temp);

	temp = RTC->RTC_CMR;
	printf("Match_1 :%x \n", temp);
	temp = RTC->RTC_CLR;
	printf("Load_1 :%x \n", temp);
	RTC_Configuation(RTC_CNTEN_MODE,11,10);
	temp = RTC->RTC_CMR;
	printf("Match_2 :%x \n", temp);
	temp = RTC->RTC_CLR;
	printf("Load_2 :%x \n", temp);
	temp = RTC->RTC_CCR;
	//printf("CCR_3 :%x \n", temp);

	temp=RTC_IT_Register();
    RTC_ITMask_CMD(RTC_IT_UMUSK);
    RTC_IT_CMD(ENABLE);
	printf("irq regis state:%x \n", temp);

	RTC_Start();
	temp = RTC->RTC_CCR;
	printf("CCR_4:%x \n", temp);

	temp=RTC_GetCompVer();
	printf("CompVersion:%x \n", temp);

	for(i=0;i<=4;i++)
	{
		temp=RTC_GetCNTValue();
		printf("Counter Value:%x \n", temp);
		if(i==4)
		{
			RTC_EN_CMD(DISABLE);
		}
	}
}

/*rtc_en mode ,interrupt umusked and enabled,set load value and match value
 *
 */
void RTC_Test2(void )
{
	rtctestflag=0;
	Bit32u i,temp;
	printf(" rtc test begins \n");
	RTC_Configuation(RTC_CNTEN_MODE,4000000,1);
	//temp = RTC->RTC_CMR;
	temp = reg_readl(RTC,RTC_CMR);
	printf(" Match value :%x \n", temp);
	//temp = RTC->RTC_CLR;
	temp = RTC_GetCNTValue();
	printf(" Load value :%x \n", temp);
	//temp = RTC->RTC_CCR;

	temp=RTC_IT_Register();
    RTC_ITMask_CMD(RTC_IT_UMUSK);
    RTC_IT_CMD(ENABLE);

	RTC_Start();

	for(i=0;i<=4;i++)
	{
		temp=RTC_GetCNTValue();
		printf(" Counter Value:%x \n", temp);
	}

	while (!rtctestflag);
	CK_INTC_FreeIrq(CK_RTC_IRQ);
	RTC_ITMask_CMD(RTC_IT_MUSK);
	printf (" rtc test success\n");
}

/*wrap mode ,interrupt musked and disabled,set load value and match value,enable rtc_en and wrap_en
 *
 */
void RTC_Test3_1(void )
{
	Bit32u temp,i;
	temp = RTC->RTC_CCR;
	printf("CCR_1 :%x \n", temp);
	RTC_Deactive();
	temp = RTC->RTC_CCR;
	printf("CCR_2 :%x \n", temp);

	temp = RTC->RTC_CMR;
	printf("Match_1 :%x \n", temp);
	temp = RTC->RTC_CLR;
	printf("Load_1 :%x \n", temp);
	RTC_Configuation(RTC_WRAP_MODE,50,40);
	temp = RTC->RTC_CMR;
	printf("Match_2 :%x \n", temp);
	temp = RTC->RTC_CLR;
	printf("Load_2 :%x \n", temp);
	temp = RTC->RTC_CCR;
	printf("CCR_3 :%x \n", temp);

	temp=RTC_IT_Register();
    RTC_ITMask_CMD(RTC_IT_MUSK);
    RTC_IT_CMD(ENABLE);
	printf("irq regis state:%x \n", temp);

	RTC_Start();
	//RTC_EN_CMD(DISABLE);
	//RTC_Wrap_CMD(ENABLE);
	temp = RTC->RTC_CCR;
	printf("CCR_4:%x \n", temp);

	for(i=0;i<=4;i++)
	{
		temp=RTC_GetCNTValue();
		printf("Counter Value:%x \n", temp);
	}
}

/*wrap mode ,interrupt musked and disabled,set load value and match value,just enable wrap_en,disable rtc_en
 *
 */
void RTC_Test3_2(void )
{
	Bit32u temp,i;
	temp = RTC->RTC_CCR;
	printf("CCR_1 :%x \n", temp);
	RTC_Deactive();
	temp = RTC->RTC_CCR;
	printf("CCR_2 :%x \n", temp);

	temp = RTC->RTC_CMR;
	printf("Match_1 :%x \n", temp);
	temp = RTC->RTC_CLR;
	printf("Load_1 :%x \n", temp);
	RTC_Configuation(RTC_WRAP_MODE,50,40);
	temp = RTC->RTC_CMR;
	printf("Match_2 :%x \n", temp);
	temp = RTC->RTC_CLR;
	printf("Load_2 :%x \n", temp);
	temp = RTC->RTC_CCR;
	printf("CCR_3 :%x \n", temp);

	temp=RTC_IT_Register();
    RTC_ITMask_CMD(RTC_IT_MUSK);
    RTC_IT_CMD(ENABLE);
	printf("irq regis state:%x \n", temp);

	RTC_Wrap_CMD(ENABLE);
	temp = RTC->RTC_CCR;
	printf("CCR_4:%x \n", temp);

	for(i=0;i<=4;i++)
	{
		temp=RTC_GetCNTValue();
		printf("Counter Value:%x \n", temp);
	}
}

/*wrap mode ,interrupt umusked and enabled,set load value and match value
 *
 */
void RTC_Test4(void )
{
	Bit32u temp,i;
	temp = RTC->RTC_CCR;
	printf("CCR_1 :%x \n", temp);
	RTC_Deactive();
	temp = RTC->RTC_CCR;
	printf("CCR_2 :%x \n", temp);

	temp = RTC->RTC_CMR;
	printf("Match_1 :%x \n", temp);
	temp = RTC->RTC_CLR;
	printf("Load_1 :%x \n", temp);
	RTC_Configuation(RTC_WRAP_MODE,50,40);
	temp = RTC->RTC_CMR;
	printf("Match_2 :%x \n", temp);
	temp = RTC->RTC_CLR;
	printf("Load_2 :%x \n", temp);
	temp = RTC->RTC_CCR;
	printf("CCR_3 :%x \n", temp);

	temp=RTC_IT_Register();
    RTC_ITMask_CMD(RTC_IT_UMUSK);
    RTC_IT_CMD(ENABLE);
	printf("irq regis state:%x \n", temp);

	RTC_Wrap_CMD(ENABLE);
	temp = RTC->RTC_CCR;
	printf("CCR_4:%x \n", temp);

	for(i=0;i<=4;i++)
	{
		temp=RTC_GetCNTValue();
		printf("Counter Value:%x \n", temp);
	}
	while (1);
}

