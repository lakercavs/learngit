/*
 * gpio.h
 *
 *  Created on: 2016-10-28
 *  Author: wxy
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "io.h"
#include "ck810.h"
#include "datatype.h"
#include "intc.h"

#define  CK_GPIOA_IRQ   CK_INTC_GPIOA
typedef struct{
	Bit32u gpio_swportx_dr; 				/* 0x00 Port X Data Register */
	Bit32u gpio_swportx_ddr; 				/* 0x04 Port X Data Direction Register */
	Bit32u gpio_swportx_ctl; 				/* 0x08 Port X Data Source */
}GPIO_PortReg_TypeDef;

typedef struct{
	Bit32u gpio_inten; 				        /* 0x30 Interrupt enable register */
	Bit32u gpio_intmask; 				    /* 0x34 Interrupt mask register */
	Bit32u gpio_inttype_level; 				/* 0x38 Interrupt level register */
	Bit32u gpio_int_polarity; 				/* 0x3C Interrupt polarity register */
	Bit32u gpio_intstatus; 				    /* 0x40 Interrupt status of Port A */
	Bit32u gpio_raw_intstatus; 				/* 0x44 Raw interrupt status of Port A */
	Bit32u gpio_debounce; 				    /* 0x48 Debounce enable register */
	Bit32u gpio_porta_eoi; 				    /* 0x4C Port A clear interrupt register */
	Bit32u gpio_ext_porta; 				    /* 0x50 Port A external port register */
	Bit32u gpio_ext_portb; 				    /* 0x54 Port B external port register */
	Bit32u gpio_ext_portc; 				    /* 0x58 Port C external port register */
	Bit32u gpio_ext_portd; 				    /* 0x5C Port D external port register */
	Bit32u gpio_ls_sync; 				    /* 0x60 Level-sensitive synchronization enable register */
	Bit32u gpio_id_code; 				    /* 0x64 ID code register */
	Bit32u gpio_int_bothedge; 				/* 0x68 Interrupt both edge type */
	Bit32u gpio_ver_id_code; 				/* 0x6C Component Version register */
	Bit32u gpio_config_reg2; 				/* 0x70 Configuration Register 2 */
	Bit32u gpio_config_reg1; 				/* 0x74 Configuration Register 1 */
}GPIO_CommonReg_TypeDef;

#define GPIOA                ((GPIO_PortReg_TypeDef *) GPIOA_BASE)
#define GPIOB                ((GPIO_PortReg_TypeDef *) GPIOB_BASE)
#define GPIOC                ((GPIO_PortReg_TypeDef *) GPIOC_BASE)
#define GPIOD                ((GPIO_PortReg_TypeDef *) GPIOD_BASE)
#define GPIO                 ((GPIO_CommonReg_TypeDef *) GPIO_BASE)

typedef enum{
	INPUT = 0,
	OUTPUT = 1
}GPIO_Direction_TypeDef;

typedef enum{
	HARDWARE = 1,
	SOFTWARE = 0
}GPIO_Source_TypeDef;

typedef enum
{
  GPIO_IT_MASK = 1,
  GPIO_IT_UMASK = 0
}GPIO_ITMask_TypeDef;

typedef enum
{
  GPIO_IT_Edge = 1,
  GPIO_IT_Level = 0
}GPIO_ITType_TypeDef;

typedef enum
{
  GPIO_IT_Low_Falling = 0,
  GPIO_IT_High_Rising = 1,
  GPIO_IT_Bothedge=2
}GPIO_ITPolarity_TypeDef;

#define	GPIO_Pin_0  (Bit32u)0x0001
#define	GPIO_Pin_1  (Bit32u)0x0002
#define	GPIO_Pin_2  (Bit32u)0x0004
#define	GPIO_Pin_3  (Bit32u)0x0008
#define	GPIO_Pin_4  (Bit32u)0x0010
#define	GPIO_Pin_5  (Bit32u)0x0020
#define	GPIO_Pin_6  (Bit32u)0x0040
#define	GPIO_Pin_7  (Bit32u)0x0080
#define	GPIO_Pin_8  (Bit32u)0x0100
#define	GPIO_Pin_9  (Bit32u)0x0200
#define	GPIO_Pin_10  (Bit32u)0x0400
#define	GPIO_Pin_11  (Bit32u)0x0800
#define	GPIO_Pin_12  (Bit32u)0x1000
#define	GPIO_Pin_13  (Bit32u)0x2000
#define	GPIO_Pin_14  (Bit32u)0x4000
#define	GPIO_Pin_15  (Bit32u)0x8000
#define	GPIO_Pin_All  (Bit32u)0xFFFF

/***gpio_config_reg1***/
#if 0
#define INTERRUPT_BOTH_EDGE_TYPE     (1<<21)         /**/
#define ENCODED_ID_WIDTH(x)          (x<<16)         /**/
#define GPIO_ID                      (1<<15)         /**/
#define ADD_ENCODED_PARAMS           (1<<14)         /**/
#define DEBOUNCE                     (1<<13)         /**/
#define PORTA_INTR                   (1<<12)         /**/
#define HW_PORTD                     (1<<11)         /**/
#define HW_PORTC                     (1<<10)         /**/
#define HW_PORTB                     (1<<9)          /**/
#define HW_PORTA                     (1<<8)          /**/
#define PORTD_SINGLE_CTL             (1<<7)          /**/
#define PORTC_SINGLE_CTL             (1<<6)          /**/
#define PORTB_SINGLE_CTL             (1<<5)          /**/
#define PORTA_SINGLE_CTL             (1<<4)          /**/
#define NUM_PORTS(x)                 (x<<2)          /**/
#define APB_DATA_WIDTH(x)            (x)             /**/

/***gpio_config_reg2***/
#define ENCODED_ID_PWIDTH_D(x)       (x<<15)         /**/
#define ENCODED_ID_PWIDTH_C(x)       (x<<10)         /**/
#define ENCODED_ID_PWIDTH_B(x)       (x<<5)          /**/
#define ENCODED_ID_PWIDTH_A(x)       (x)             /**/
#endif

typedef enum
{
	INTERRUPT_BOTH_EDGE_TYPE,
	ENCODED_ID_WIDTH ,
	GPIO_ID,
	ADD_ENCODED_PARAMS,
	DEBOUNCE,
	PORTA_INTR,
	HW_PORTD,
	HW_PORTC,
	HW_PORTB,
	HW_PORTA,
	PORTD_SINGLE_CTL,
	PORTC_SINGLE_CTL,
	PORTB_SINGLE_CTL,
	PORTA_SINGLE_CTL,
	NUM_PORTS,
	APB_DATA_WIDTH,
	ENCODED_ID_PWIDTH_D,
	ENCODED_ID_PWIDTH_C,
	ENCODED_ID_PWIDTH_B,
	ENCODED_ID_PWIDTH_A
}GPIO_ParamterTypeDef;

/*function 1_1: write data to portX*/
Bit32u GPIO_Port_Data_Write(GPIO_PortReg_TypeDef* GPIOx,Bit32u Data);
/*function 1_2: write 1 to bit [n] of portX
 * GPIOx=GPIOA/GPIOB/GPIOC/GPIOD
 * Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Pin_Set(GPIO_PortReg_TypeDef* GPIOx,Bit32u Pin);
/*function 1_3: write 0 to bit [n] of portX
 *GPIOx=GPIOA/GPIOB/GPIOC/GPIOD
 * Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Pin_Reset(GPIO_PortReg_TypeDef* GPIOx,Bit32u Pin);
/*function 1_4: read data of portX
 *GPIOx=GPIOA/GPIOB/GPIOC/GPIOD
 *!!The value read back is equal to the last value written to this register.
 * */
Bit32u GPIO_Get_PORT_Inter_Data(GPIO_PortReg_TypeDef* GPIOx);
/*function 1_5: read data of pin[n] of portX
 *GPIOx=GPIOA/GPIOB/GPIOC/GPIOD
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 *!!The value read back is equal to the last value written to this register
 * */
Bit32u GPIO_Get_Pin_Inter_Data(GPIO_PortReg_TypeDef* GPIOx,Bit32u Pin);
/*function 2_1: set direction of portX
 *GPIOx=GPIOA/GPIOB/GPIOC/GPIOD
  * */
Bit32u GPIO_Set_Port_Dir(GPIO_PortReg_TypeDef* GPIOx,GPIO_Direction_TypeDef Direction);
/*function 2_2: set direction of pin[n] of portX
 *GPIOx=GPIOA/GPIOB/GPIOC/GPIOD
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
Bit32u GPIO_Set_Pin_Dir(GPIO_PortReg_TypeDef* GPIOx,Bit32u Pin,GPIO_Direction_TypeDef Direction);
/*function 3_1: set data source of portX
 *GPIOx=GPIOA/GPIOB/GPIOC/GPIOD
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
Bit32u GPIO_Set_Port_Source(GPIO_PortReg_TypeDef* GPIOx,GPIO_Source_TypeDef Source);
/*function 3_2: set data source of pin[n] of portX
 *GPIOx=GPIOA/GPIOB/GPIOC/GPIOD
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
Bit32u GPIO_Set_Pin_Source(GPIO_PortReg_TypeDef* GPIOx,Bit32u Pin,GPIO_Source_TypeDef Source);
/*function 4_1: set IT enabled or disabed of pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
Bit32u GPIO_IT_CMD(Bit32u Pin,State_TypeDef Newstate);
/*function 4_2: set IT enabled pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_IT_ENABLE(Bit32u Pin);
/*function 4_3: set IT disenabled pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_IT_DISABLE(Bit32u Pin);
/*function 5_1: set IT mask or umask of pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
Bit32u GPIO_IT_Musk_CMD(Bit32u Pin,GPIO_ITMask_TypeDef MaskState);
/*function 5_2: set IT mask pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_IT_Mask(Bit32u Pin);
/*function 5_3: set IT umask pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_IT_Umask(Bit32u Pin);
/*function 6_1: set IT type of pin[n] of portA,level sentive or edge sentive
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
Bit32u GPIO_Set_IT_Type(Bit32u Pin,GPIO_ITType_TypeDef ITtype);
/*function 6_2: set IT type of pin[n] of portA,level sentive
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Set_IT_Type_lev(Bit32u Pin);
/*function 6_3: set IT type of pin[n] of portA,edge sentive
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Set_IT_Type_Edge(Bit32u Pin);
/*function 7_1: set IT type of pin[n] of portA,low active/falling edge or high active/rising edge
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
Bit32u GPIO_Set_IT_Polarity(Bit32u Pin,GPIO_ITPolarity_TypeDef Polarity);
/*function 7_2: set IT type of pin[n] of portA,low active/falling edge
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Set_IT_Polar_LOW(Bit32u Pin);
/*function 7_3: set IT type of pin[n] of portA,high active/rising edge
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Set_IT_Polar_HIGH(Bit32u Pin);
/*function 18_1: set IT type of pin[n] of portA,both edge
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Set_IT_Polar_BothEdge(Bit32u Pin);
/*function 8_1: get IT status of pin[n] of portA,ACTIVE or INACTIVE
 *Pin=GPIO_Pin_n
 * */
ITStatus_TypeDef GPIO_Get_ITStat(Bit32u Pin);
/*function 9_1: get raw IT status of pin[n] of portA,ACTIVE or INACTIVE
 *Pin=GPIO_Pin_n
 * */
ITStatus_TypeDef GPIO_Get_RITStat(Bit32u Pin);
/*function 10_1: set Debounce enable or disable of pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
Bit32u GPIO_Debounce_CMD(Bit32u Pin,State_TypeDef Newstate);
/*function 10_2: set Debounce enable of pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Debounce_ENABLE(Bit32u Pin);
/*function 10_3: set Debounce disable of pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Debounce_DISABLE(Bit32u Pin);
/*function 11_1: clear IT flag of pin[n] of portA
 *Pin=GPIO_Pin_n or GPIO_Pin_a | GPIO_Pin_b | GPIO_Pin_c .....
 * */
void GPIO_Clear_ITSTAT(Bit32u Pin);
/*function 12_1: get external value of pin[n] of portA
 *Pin=GPIO_Pin_n
 * */
Bit32u GPIOA_Get_Ext_Data(Bit32u Pin);
/*function 13_1: get external value of pin[n] of portB
 *Pin=GPIO_Pin_n
 * */
Bit32u GPIOB_Get_Ext_Data(Bit32u Pin);
/*function 14_1: get external value of pin[n] of portC
 *Pin=GPIO_Pin_n
 * */
Bit32u GPIOC_Get_Ext_Data(Bit32u Pin);
/*function 15_1: get external value of pin[n] of portD
 *Pin=GPIO_Pin_n
 * */
Bit32u GPIOD_Get_Ext_Data(Bit32u Pin);
/*function 16_1: set IT of pin[n] of portA Synchronize to pclk_intr or not*/
Bit32u GPIOA_IT_Sync_CMD(State_TypeDef Newstate);
/*function 17_1: get gpio_id_code */
Bit32u GPIO_Get_ID_Code(void);
/*function 19_1: get gpio comp version */
Bit32u GPIO_Get_Comp_Ver(void);
/*function 20_1: get par of config_1 or config_2 */
Bit32u GPIO_Get_Config(GPIO_ParamterTypeDef Paramter);
/*config function1*/
void GPIOB_Config(Bit32u Pin,GPIO_Source_TypeDef Source,GPIO_Direction_TypeDef Direction);
/*config function2*/
void GPIOC_Config(Bit32u Pin,GPIO_Source_TypeDef Source,GPIO_Direction_TypeDef Direction);
/*config function3*/
void GPIOD_Config(Bit32u Pin,GPIO_Source_TypeDef Source,GPIO_Direction_TypeDef Direction);
/*config function4*/
void GPIOA_Normal_Config(Bit32u Pin,GPIO_Source_TypeDef Source,GPIO_Direction_TypeDef Direction);
/*config function34*/
void GPIOA_IT_Config(Bit32u Pin,GPIO_ITMask_TypeDef MaskState,GPIO_ITType_TypeDef ITtype,GPIO_ITPolarity_TypeDef Polarity);
Bit32u GPIOA_IT_Register(void);
void CK_GPIO_Handler(Bit32u irqid);

void GPIO_test1(void);
void GPIO_test2(void);
void GPIO_input_set(void);


#endif /* GPIO_H_ */
