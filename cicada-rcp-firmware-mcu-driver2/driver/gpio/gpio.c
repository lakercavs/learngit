/*
 * gpio.c
 *
 *  Created on: 2016-10-28
 *  Author: wxy
 */
#include <stdio.h>
#include <ctype.h>
#include "include/io.h"
#include "gpio.h"

/*function 1_1: write data to portX*/
Bit32u GPIO_Port_Data_Write(GPIO_PortReg_TypeDef* GPIOx,Bit32u Data)
{
	//GPIOx->gpio_swportx_dr=Data;
	reg_direct_writel(Data,GPIOx,gpio_swportx_dr);
	return Data;
}

/*function 1_2: write 1 to bit [n] of portX
 * GPIOx=GPIOA/GPIOB/GPIOC/GPIOD
 * Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Pin_Set(GPIO_PortReg_TypeDef* GPIOx,Bit32u Pin)
{
	//GPIOx->gpio_swportx_dr |= Pin;
	reg_writel(Pin,GPIOx,gpio_swportx_dr);
}

/*function 1_3: write 0 to bit [n] of portX
 *GPIOx=GPIOA/GPIOB/GPIOC/GPIOD
 * Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Pin_Reset(GPIO_PortReg_TypeDef* GPIOx,Bit32u Pin)
{
	//GPIOx->gpio_swportx_dr &= ~Pin;
	reg_clearl(Pin,GPIOx,gpio_swportx_dr);
}

/*function 1_4: read data of portX
 *GPIOx=GPIOA/GPIOB/GPIOC/GPIOD
 *!!The value read back is equal to the last value written to this register.
 * */
Bit32u GPIO_Get_PORT_Inter_Data(GPIO_PortReg_TypeDef* GPIOx)
{
	Bit32u temp = 0;
	//temp = GPIOx->gpio_swportx_dr;
	temp = reg_readl(GPIOx,gpio_swportx_dr);
	return temp;
}

/*function 1_5: read data of pin[n] of portX
 *GPIOx=GPIOA/GPIOB/GPIOC/GPIOD
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 *!!The value read back is equal to the last value written to this register
 * */
Bit32u GPIO_Get_Pin_Inter_Data(GPIO_PortReg_TypeDef* GPIOx,Bit32u Pin)
{
	Bit32u temp = 0;
	//temp = GPIOx->gpio_swportx_dr & Pin;
	temp = reg_readl(GPIOx,gpio_swportx_dr) & Pin;
	return temp;
}

/*function 2_1: set direction of portX
 *GPIOx=GPIOA/GPIOB/GPIOC/GPIOD
  * */
Bit32u GPIO_Set_Port_Dir(GPIO_PortReg_TypeDef* GPIOx,GPIO_Direction_TypeDef Direction)
{
	if(Direction==OUTPUT)
	{
		//GPIOx->gpio_swportx_ddr = 0xFFFF;
		reg_direct_writel(0xFFFF,GPIOx,gpio_swportx_ddr);
		return 0;
	}
	else if(Direction==INPUT)
	{
		//GPIOx->gpio_swportx_ddr = 0;
		reg_clearl(0xFFFF,GPIOx,gpio_swportx_ddr);
		return 0;
	}
	return 1;
}

/*function 2_2: set direction of pin[n] of portX
 *GPIOx=GPIOA/GPIOB/GPIOC/GPIOD
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
Bit32u GPIO_Set_Pin_Dir(GPIO_PortReg_TypeDef* GPIOx,Bit32u Pin,GPIO_Direction_TypeDef Direction)
{
	if(Direction==OUTPUT)
	{
		//GPIOx->gpio_swportx_ddr |= Pin;
		reg_writel(Pin,GPIOx,gpio_swportx_ddr);
		return 0;
	}
	else if(Direction==INPUT)
	{
		//GPIOx->gpio_swportx_ddr &= ~Pin;
		reg_clearl(Pin,GPIOx,gpio_swportx_ddr);
		return 0;
	}
	return 1;
}

/*function 3_1: set data source of portX
 *GPIOx=GPIOA/GPIOB/GPIOC/GPIOD
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
Bit32u GPIO_Set_Port_Source(GPIO_PortReg_TypeDef* GPIOx,GPIO_Source_TypeDef Source)
{
	if(Source==HARDWARE)
	{
		//GPIOx->gpio_swportx_ctl = 0xFFFF;
		reg_direct_writel(0xFFFF,GPIOx,gpio_swportx_ctl);
		return 0;
	}
	else if(Source==SOFTWARE)
	{
		//GPIOx->gpio_swportx_ctl = 0;
		reg_clearl(0xFFFF,GPIOx,gpio_swportx_ctl);
		return 0;
	}
	return 1;
}

/*function 3_2: set data source of pin[n] of portX
 *GPIOx=GPIOA/GPIOB/GPIOC/GPIOD
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
Bit32u GPIO_Set_Pin_Source(GPIO_PortReg_TypeDef* GPIOx,Bit32u Pin,GPIO_Source_TypeDef Source)
{
	if(Source==HARDWARE)
	{
		//GPIOx->gpio_swportx_ctl |= Pin;
		reg_writel(Pin,GPIOx,gpio_swportx_ctl);
		return 0;
	}
	else if(Source==SOFTWARE)
	{
		//GPIOx->gpio_swportx_ctl &= ~Pin;
		reg_clearl(Pin,GPIOx,gpio_swportx_ctl);
		return 0;
	}
	return 1;
}

/*function 4_1: set IT enabled or disabed of pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
Bit32u GPIO_IT_CMD(Bit32u Pin,State_TypeDef Newstate)
{
	if(Newstate==ENABLE)
	{
		//GPIO->gpio_inten |= Pin;      // do not affect other pins
		reg_writel(Pin,GPIO,gpio_inten);
		return 0;
	}
	else if(Newstate==DISABLE)
	{
		//GPIO->gpio_inten &= ~Pin;    //do not affect other pins
		reg_clearl(Pin,GPIO,gpio_inten);
		return 0;
	}
	else
		return 1;
}

/*function 4_2: set IT enabled pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_IT_ENABLE(Bit32u Pin)
{
	//GPIO->gpio_inten |= Pin;      // do not affect other pins
	reg_writel(Pin,GPIO,gpio_inten);
}

/*function 4_3: set IT disenabled pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_IT_DISABLE(Bit32u Pin)
{
	//GPIO->gpio_inten &= ~Pin;      // do not affect other pins
	reg_clearl(Pin,GPIO,gpio_inten);
}

/*function 5_1: set IT mask or umask of pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
Bit32u GPIO_IT_Musk_CMD(Bit32u Pin,GPIO_ITMask_TypeDef MaskState)
{
	if(MaskState==GPIO_IT_MASK)
	{
		//GPIO->gpio_intmask |= Pin;      // do not affect other pins
		reg_writel(Pin,GPIO,gpio_intmask);
		return 0;
	}
	else if(MaskState==GPIO_IT_UMASK)
	{
		//GPIO->gpio_intmask &= ~Pin;     //do not affect other pins
		reg_clearl(Pin,GPIO,gpio_intmask);
		return 0;
	}
	else
		return 1;
}

/*function 5_2: set IT mask pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_IT_Mask(Bit32u Pin)
{
	//GPIO->gpio_intmask |= Pin;      // do not affect other pins
	reg_writel(Pin,GPIO,gpio_intmask);
}

/*function 5_3: set IT umask pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_IT_Umask(Bit32u Pin)
{
	//GPIO->gpio_intmask &= ~Pin;     //do not affect other pins
	reg_clearl(Pin,GPIO,gpio_intmask);
}

/*function 6_1: set IT type of pin[n] of portA,level sentive or edge sentive
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
Bit32u GPIO_Set_IT_Type(Bit32u Pin,GPIO_ITType_TypeDef ITtype)
{
	//GPIO->gpio_int_bothedge=0;                //bothedge should not be set
	reg_direct_writel(0,GPIO,gpio_int_bothedge);
	if(ITtype==GPIO_IT_Edge)
	{
		//GPIO->gpio_inttype_level |= Pin;      //do not affect other pins
		reg_writel(Pin,GPIO,gpio_inttype_level);
		return 0;
	}
	else if(ITtype==GPIO_IT_Level)
	{
		//GPIO->gpio_inttype_level &= ~Pin;     //do not affect other pins
		reg_clearl(Pin,GPIO,gpio_inttype_level);
		return 0;
	}
	else
		return 1;
}

/*function 6_2: set IT type of pin[n] of portA,level sentive
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Set_IT_Type_lev(Bit32u Pin)
{
	//GPIO->gpio_int_bothedge=0;            //bothedge should not be set
	//GPIO->gpio_inttype_level &= ~Pin;     //do not affect other pins
	reg_direct_writel(0,GPIO,gpio_int_bothedge);
	reg_writel(Pin,GPIO,gpio_inttype_level);
}

/*function 6_3: set IT type of pin[n] of portA,edge sentive
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Set_IT_Type_Edge(Bit32u Pin)
{
	//GPIO->gpio_int_bothedge=0;            //bothedge should not be set
	//GPIO->gpio_inttype_level |= Pin;      //do not affect other pins
	reg_direct_writel(0,GPIO,gpio_int_bothedge);
	reg_clearl(Pin,GPIO,gpio_inttype_level);
}

/*function 7_1: set IT type of pin[n] of portA,low active/falling edge or high active/rising edge
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
Bit32u GPIO_Set_IT_Polarity(Bit32u Pin,GPIO_ITPolarity_TypeDef Polarity)
{
	if(Polarity==GPIO_IT_High_Rising)
	{
		//GPIO->gpio_int_bothedge=0;           //bothedge should not be set
		//GPIO->gpio_int_polarity |= Pin;      //do not affect other pins
		reg_direct_writel(0,GPIO,gpio_int_bothedge);
		reg_writel(Pin,GPIO,gpio_int_polarity);
		return 0;
	}
	else if(Polarity==GPIO_IT_Low_Falling)
	{
		//GPIO->gpio_int_bothedge=0;           //bothedge should not be set
		//GPIO->gpio_int_polarity &= ~Pin;     //do not affect other pins
		reg_direct_writel(0,GPIO,gpio_int_bothedge);
		reg_clearl(Pin,GPIO,gpio_int_polarity);
		return 0;
	}
	else if(Polarity==GPIO_IT_Bothedge)
	{
		//GPIO->gpio_int_bothedge |= Pin;      //do not affect other pins
		reg_writel(Pin,GPIO,gpio_int_bothedge);
		return 0;
	}
	else
		return 1;
}

/*function 7_2: set IT type of pin[n] of portA,low active/falling edge
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Set_IT_Polar_LOW(Bit32u Pin)
{
	//GPIO->gpio_int_bothedge=0;            //bothedge should not be set
	//GPIO->gpio_int_polarity &= ~Pin;      //do not affect other pins
	reg_direct_writel(0,GPIO,gpio_int_bothedge);
	reg_clearl(Pin,GPIO,gpio_int_polarity);
}

/*function 7_3: set IT type of pin[n] of portA,high active/rising edge
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Set_IT_Polar_HIGH(Bit32u Pin)
{
	//GPIO->gpio_int_bothedge=0;           //bothedge should not be set
	//GPIO->gpio_int_polarity |= Pin;      //do not affect other pins
	reg_direct_writel(0,GPIO,gpio_int_bothedge);
	reg_writel(Pin,GPIO,gpio_int_polarity);
}

/*function 18_1: set IT type of pin[n] of portA,both edge
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Set_IT_Polar_BothEdge(Bit32u Pin)
{
	//GPIO->gpio_int_bothedge |= Pin;
	reg_writel(Pin,GPIO,gpio_int_bothedge);
}

/*function 8_1: get IT status of pin[n] of portA,ACTIVE or INACTIVE
 *Pin=GPIO_Pin_n
 * */
ITStatus_TypeDef GPIO_Get_ITStat(Bit32u Pin)
{
	ITStatus_TypeDef bitstatus=INACTIVE;
	//if (!(GPIO->gpio_intstatus & Pin))
	if (!(reg_readl(GPIO, gpio_intstatus) & Pin))

	{
		bitstatus = INACTIVE;             /* Interrupt is inactive */
	}
	else
	{
		bitstatus = ACTIVE;               /* Interrupt is active */
	}
	return bitstatus;
}

/*function 9_1: get raw IT status of pin[n] of portA,ACTIVE or INACTIVE
 *Pin=GPIO_Pin_n
 * */
ITStatus_TypeDef GPIO_Get_RITStat(Bit32u Pin)
{
	ITStatus_TypeDef bitstatus=INACTIVE;
	//if (!(GPIO->gpio_raw_intstatus & Pin))
	if (!(reg_readl(GPIO, gpio_raw_intstatus) & Pin))
	{
		bitstatus = INACTIVE;             /* Interrupt is inactive */
	}
	else
	{
		bitstatus = ACTIVE;               /* Interrupt is active */
	}
	return bitstatus;
}

/*function 10_1: set Debounce enable or disable of pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
Bit32u GPIO_Debounce_CMD(Bit32u Pin,State_TypeDef Newstate)
{
	if(Newstate==ENABLE)
	{
		//GPIO->gpio_debounce |= Pin;      // do not affect other pins
		reg_writel(Pin,GPIO,gpio_debounce);
		return 0;
	}
	else if(Newstate==DISABLE)
	{
		//GPIO->gpio_debounce &= ~Pin;      //do not affect other pins
		reg_clearl(Pin,GPIO,gpio_debounce);
		return 0;
	}
	else
		return 1;
}

/*function 10_2: set Debounce enable of pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Debounce_ENABLE(Bit32u Pin)
{
	//GPIO->gpio_debounce |= Pin;      // do not affect other pins
	reg_writel(Pin,GPIO,gpio_debounce);
}

/*function 10_3: set Debounce disable of pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Debounce_DISABLE(Bit32u Pin)
{
	//GPIO->gpio_debounce &= ~Pin;      // do not affect other pins
	reg_clearl(Pin,GPIO,gpio_debounce);
}

/*function 11_1: clear IT flag of pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Clear_ITSTAT(Bit32u Pin)
{
	//GPIO->gpio_porta_eoi |= Pin;       // do not affect other pins
	reg_direct_writel(Pin,GPIO,gpio_porta_eoi);  //gpio_porta_eoi write only
}

/*function 12_1: get external value of pin[n] of portA
 *Pin=GPIO_Pin_n
 * */
Bit32u GPIOA_Get_Ext_Data(Bit32u Pin)
{
	Bit32u temp;
	//temp = GPIO->gpio_ext_porta & Pin;
	temp = reg_readl(GPIO, gpio_ext_porta)& Pin;
	return temp;
}

/*function 13_1: get external value of pin[n] of portB
 *Pin=GPIO_Pin_n
 * */
Bit32u GPIOB_Get_Ext_Data(Bit32u Pin)
{
	Bit32u temp;
	//temp = GPIO->gpio_ext_portb & Pin;
	temp = reg_readl(GPIO, gpio_ext_portb)& Pin;
	return temp;
}

/*function 14_1: get external value of pin[n] of portC
 *Pin=GPIO_Pin_n
 * */
Bit32u GPIOC_Get_Ext_Data(Bit32u Pin)
{
	Bit32u temp;
	//temp = GPIO->gpio_ext_portc & Pin;
	temp = reg_readl(GPIO, gpio_ext_portc)& Pin;
	return temp;
}

/*function 15_1: get external value of pin[n] of portD
 *Pin=GPIO_Pin_n
 * */
Bit32u GPIOD_Get_Ext_Data(Bit32u Pin)
{
	Bit32u temp;
	//temp = GPIO->gpio_ext_portd & Pin;
	temp = reg_readl(GPIO, gpio_ext_portd)& Pin;
	return temp;
}

/*function 16_1: set IT of pin[n] of portA Synchronize to pclk_intr or not*/
Bit32u GPIOA_IT_Sync_CMD(State_TypeDef Newstate)
{
	if(Newstate==ENABLE)
	{
		//GPIO->gpio_ls_sync = 1;
		reg_direct_writel(1,GPIO,gpio_ls_sync);
		return 0;
	}
	else if(Newstate==DISABLE)
	{
		//GPIO->gpio_ls_sync =0;
		reg_direct_writel(0,GPIO,gpio_ls_sync);
		return 0;
	}
	else
		return 1;
}

/*function 17_1: get gpio_id_code */
Bit32u GPIO_Get_ID_Code(void)
{
	Bit32u temp;
	//temp = GPIO->gpio_id_code;
	temp =reg_readl(GPIO, gpio_id_code);
	return temp;
}

/*function 19_1: get gpio comp version */
Bit32u GPIO_Get_Comp_Ver(void)
{
	Bit32u temp;
	//temp = GPIO->gpio_ver_id_code;
	temp =reg_readl(GPIO, gpio_ver_id_code);
	return temp;
}

/*function 20_1: get par of config_1 or config_2 */
Bit32u GPIO_Get_Config(GPIO_ParamterTypeDef Paramter)
{
	Bit32u temp,temp1,temp2=0;
	//temp1= GPIO->gpio_config_reg1;
	//temp2= GPIO->gpio_config_reg2;
	temp1 =reg_readl(GPIO, gpio_config_reg1);
	temp2 =reg_readl(GPIO, gpio_config_reg2);
	switch(Paramter)
	{
	    /*config_1*/
		case INTERRUPT_BOTH_EDGE_TYPE:    temp= (temp1>>21) & 0x01;     break; /* bit[21] */
		case ENCODED_ID_WIDTH:            temp= (temp1>>16) & 0x1F;     break; /* bit[20:16] */
		case GPIO_ID:                     temp= (temp1>>15) & 0x01;     break; /* bit[15] */
		case ADD_ENCODED_PARAMS:          temp= (temp1>>14) & 0x01;     break; /* bit[14] */
		case DEBOUNCE:                    temp= (temp1>>13) & 0x01;     break; /* bit[13] */
		case PORTA_INTR:                  temp= (temp1>>12) & 0x01;     break; /* bit[12] */
		case HW_PORTD:                    temp= (temp1>>11) & 0x01;     break; /* bit[11] */
		case HW_PORTC:                    temp= (temp1>>10) & 0x01;     break; /* bit[10] */
		case HW_PORTB:                    temp= (temp1>>9) & 0x01;      break; /* bit[9] */
		case HW_PORTA:                    temp= (temp1>>8) & 0x01;      break; /* bit[8] */
		case PORTD_SINGLE_CTL:            temp= (temp1>>7) & 0x01;      break; /* bit[7] */
		case PORTC_SINGLE_CTL:            temp= (temp1>>6) & 0x01;      break; /* bit[6] */
		case PORTB_SINGLE_CTL:            temp= (temp1>>5) & 0x01;      break; /* bit[5] */
		case PORTA_SINGLE_CTL:            temp= (temp1>>4) & 0x01;      break; /* bit[4] */
		case NUM_PORTS:                   temp= (temp1>>2) & 0x03;      break; /* bit[3:2] */
		case APB_DATA_WIDTH:              temp= temp1 & 0x03;           break; /* bit[1:0] */
		/*config_2*/
		case ENCODED_ID_PWIDTH_D:         temp= (temp2>>15) & 0x1F;     break; /* bit[19:15] */
		case ENCODED_ID_PWIDTH_C:         temp= (temp2>>10) & 0x1F;     break; /* bit[14:10] */
		case ENCODED_ID_PWIDTH_B:         temp= (temp2>>5) & 0x1F;      break; /* bit[9:5] */
		case ENCODED_ID_PWIDTH_A:         temp= temp2 & 0x1F;           break; /* bit[4:0] */
		default:                          temp= 0xFFFFFFFF;             break;
	}
	return temp;
}

/*config function1*/
void GPIOB_Config(Bit32u Pin,GPIO_Source_TypeDef Source,GPIO_Direction_TypeDef Direction)
{
	GPIO_Set_Pin_Source(GPIOB,Pin,Source);
	GPIO_Set_Pin_Dir(GPIOB,Pin,Direction);
}

/*config function2*/
void GPIOC_Config(Bit32u Pin,GPIO_Source_TypeDef Source,GPIO_Direction_TypeDef Direction)
{
	GPIO_Set_Pin_Source(GPIOB,Pin,Source);
	GPIO_Set_Pin_Dir(GPIOB,Pin,Direction);
}

/*config function3*/
void GPIOD_Config(Bit32u Pin,GPIO_Source_TypeDef Source,GPIO_Direction_TypeDef Direction)
{
	GPIO_Set_Pin_Source(GPIOD,Pin,Source);
	GPIO_Set_Pin_Dir(GPIOD,Pin,Direction);
}

/*config function4*/
void GPIOA_Normal_Config(Bit32u Pin,GPIO_Source_TypeDef Source,GPIO_Direction_TypeDef Direction)
{
	GPIO_Set_Pin_Source(GPIOA,Pin,Source);
	GPIO_Set_Pin_Dir(GPIOA,Pin,Direction);
}

/*config function5*/
void GPIOA_IT_Config(Bit32u Pin,GPIO_ITMask_TypeDef MaskState,GPIO_ITType_TypeDef ITtype,GPIO_ITPolarity_TypeDef Polarity)
{
	GPIO_Set_IT_Type(Pin,ITtype);
	GPIO_Set_IT_Polarity(Pin,Polarity);
	GPIO_IT_Musk_CMD(Pin,MaskState);
}

Bit32u GPIOA_IT_Register(void)
{
	/* clear interrupt flag */
	GPIO_Clear_ITSTAT(GPIO_Pin_All);

    CK_INTC_RequestIrq(CK_GPIOA_IRQ, CK_GPIO_Handler);
    return 0;
}


