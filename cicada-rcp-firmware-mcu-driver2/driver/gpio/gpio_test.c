/*
 * gpio_test.c
 *
 *  Created on: 2016-10-28
 *  Author: wxy
 */
#include "misc.h"
#include "intc.h"
#include "gpio.h"
#include "misc.h"

Bit32u gpiotestflag=0;
void CK_GPIO_Handler(Bit32u irqid)
{
	ITStatus_TypeDef temp=GPIO_Get_ITStat(GPIO_Pin_1);
	if(temp)
	{
		GPIO_Pin_Set(GPIOA,GPIO_Pin_0);
	}
	else
	{
		GPIO_Pin_Reset(GPIOA,GPIO_Pin_0);
	}
	printf(" IT state :%x \n", temp);
	GPIO_Clear_ITSTAT(GPIO_Pin_1);
	gpiotestflag=1;
}
/*no interrupt*/
void GPIO_test1(void)
{
	Bit32u temp=0;
	GPIO_Set_Port_Source(GPIOA,SOFTWARE);
	GPIO_Set_Port_Source(GPIOB,SOFTWARE);
	GPIO_Set_Port_Source(GPIOC,SOFTWARE);
	GPIO_Set_Port_Source(GPIOD,SOFTWARE);

	GPIO_Set_Pin_Dir(GPIOA,GPIO_Pin_0,OUTPUT);
	GPIO_Set_Pin_Dir(GPIOA,GPIO_Pin_1,INPUT);
	GPIO_Set_Port_Dir(GPIOB,OUTPUT);
	GPIO_Set_Port_Dir(GPIOC,OUTPUT);
	GPIO_Set_Port_Dir(GPIOD,OUTPUT);

	GPIO_Pin_Reset(GPIOA,GPIO_Pin_0);
	GPIO_Pin_Set(GPIOB,GPIO_Pin_0);
	GPIO_Pin_Reset(GPIOC,GPIO_Pin_0);
	GPIO_Pin_Set(GPIOD,GPIO_Pin_0);
	temp=GPIOA_Get_Ext_Data(GPIO_Pin_1);
	printf("Pin1 of PortA :%x \n", temp);
	while(1);
}

void GPIO_input_set(void)
{
	GPIO_Set_Port_Source(GPIOA,SOFTWARE);
	GPIO_Set_Port_Source(GPIOB,SOFTWARE);
	GPIO_Set_Port_Source(GPIOC,SOFTWARE);
	GPIO_Set_Port_Source(GPIOD,SOFTWARE);

	GPIO_Set_Pin_Dir(GPIOA,GPIO_Pin_0,INPUT);
	GPIO_Set_Pin_Dir(GPIOA,GPIO_Pin_1,INPUT);
	GPIO_Set_Port_Dir(GPIOB,INPUT);
	GPIO_Set_Port_Dir(GPIOC,INPUT);
	GPIO_Set_Port_Dir(GPIOD,INPUT);

}

/*interrupt*/
void GPIO_test2(void)
{
	gpiotestflag=0;
	printf (" gpio test begins\n");
	GPIO_Set_Port_Source(GPIOA,SOFTWARE);
	GPIO_Set_Port_Source(GPIOB,SOFTWARE);
	GPIO_Set_Port_Source(GPIOC,SOFTWARE);
	GPIO_Set_Port_Source(GPIOD,SOFTWARE);

	GPIO_Set_Pin_Dir(GPIOA,GPIO_Pin_0,OUTPUT);
	GPIO_Set_Pin_Dir(GPIOA,GPIO_Pin_1,INPUT);
	GPIO_Set_Port_Dir(GPIOB,OUTPUT);
	GPIO_Set_Port_Dir(GPIOC,OUTPUT);
	GPIO_Set_Port_Dir(GPIOD,OUTPUT);

	GPIO_Pin_Reset(GPIOA,GPIO_Pin_0);
	GPIO_Pin_Set(GPIOB,GPIO_Pin_0);
	GPIO_Pin_Reset(GPIOC,GPIO_Pin_0);
	GPIO_Pin_Set(GPIOD,GPIO_Pin_0);

	GPIO_Pin_Set(GPIOA,GPIO_Pin_0);
	GPIO_Pin_Reset(GPIOB,GPIO_Pin_0);
	GPIO_Pin_Set(GPIOC,GPIO_Pin_0);
	GPIO_Pin_Reset(GPIOD,GPIO_Pin_0);

	GPIOA_IT_Config(GPIO_Pin_1,GPIO_IT_UMASK,GPIO_IT_Edge,GPIO_IT_Low_Falling);
	GPIOA_IT_Register();
	GPIO_IT_ENABLE(GPIO_Pin_1);

	GPIO_Pin_Reset(GPIOC,GPIO_Pin_0);

	while(!gpiotestflag);
	printf (" gpio test success\n");


}
