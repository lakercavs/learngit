/*
 * wdt.h
 *
 *  Created on: 2016-9-21
 *  Author: wxy
 */

#ifndef WDT_H_
#define WDT_H_

#include "io.h"
#include "ck810.h"
#include "datatype.h"
#include "types.h"
#include "intc.h"

typedef struct{
	volatile Bit32u WDT_CR;		/* 0x00 control Register */
	volatile Bit32u WDT_TORR;		/* 0x04 timeout range Register */
	volatile Bit32u WDT_CCVR;		/* 0x08 Counter LoadCurrent counter value Register */
	volatile Bit32u WDT_CRR;		/* 0x0c Counter restart register */
	volatile Bit32u WDT_STAT;		/* 0x10 Interrupt Status Register */
	volatile Bit32u WDT_EOI;		/* 0x14 Interrupt clear register */
}WDT_RegTypeDef;

typedef struct{
	volatile Bit32u WDT_COMP_PARAMS_5;		/* 0xe4 Upper limit of Timeout Period parameters */
	volatile Bit32u WDT_COMP_PARAMS_4;		/* 0xe8 Upper limit of Timeout Period parameters */
	volatile Bit32u WDT_COMP_PARAMS_3;		/* 0xec Upper limit of Timeout Period parameters */
	volatile Bit32u WDT_COMP_PARAMS_2;		/* 0xf0 Upper limit of Timeout Period parameters */
	volatile Bit32u WDT_COMP_PARAMS_1;		/* 0xf4 Upper limit of Timeout Period parameters */
}WDTCOMP_RegTypeDef;

#define WDT            ((WDT_RegTypeDef *)CK_WDT_ADDRBASE)                   //define WDT reg struct,and get the WDT addrbase
#define WDTCOMP        ((WDTCOMP_RegTypeDef *) CK_WDT_ADDRBASE_1)       //define WDT reg struct,and get the WDT addrbase
#define CK_WDT_IRQ     CK_INTC_Watchdog

/*0x00 control Register CR*/
/*response length enum*/
typedef enum{
  PCLK_CYC_2=0x00,
  PCLK_CYC_4=0x04,
  PCLK_CYC_8=0x08,
  PCLK_CYC_16=0x0C,
  PCLK_CYC_32=0x10,
  PCLK_CYC_64=0x14,
  PCLK_CYC_128=0x18,
  PCLK_CYC_256=0x1c
}WDT_PluLenTypeDef;

/*response mode enum*/
typedef enum{
	 GER_RST=0x00,
	 GER_IT= 0x02
	}WDT_RMODTypeDef;

/*WDT enabled */
#define WDT_ENA  0x01

/*0x04 Timeout Range Register TOPR*/
#define TOP_INT(x)	 ((x) << 4)
#define TOP(x)	     ((x) << 0)

#define WDT_ITSTAT       0x01

#define WDT_HC_RPL       0x10    /*Hard Code Reset PLuse Length,bit 4 of WDT_COMP_PARAMS_1*/
#define WDT_HC_RMOD      0x08    /*Configures the output response mode to be hard coded.,bit 3 of WDT_COMP_PARAMS_1*/
#define WDT_ALWAYS_EN    0x01    /*the WDT is always enabled,bit 0 of WDT_COMP_PARAMS_1*/
#define WDT_DUAL_TOP     0x04    /*double timeout period is enabled,bit 2 of WDT_COMP_PARAMS_1*/
#define WDT_HC_TOP       0x20    /*Hard code timeout period range selection,bit 5 of WDT_COMP_PARAMS_1*/

typedef struct{
	WDT_RMODTypeDef ResponseMode;
	WDT_PluLenTypeDef PluseLength;
	Bit32u TOP_INIT_Value;
	Bit32u TOP_Value;
}WDT_InitTypeDef;

void wdt_bit_write(Bit32u val, void* addr);
void wdt_bit_clear(Bit32u val, void* addr);
/*set the Reset pulse length,write RPL bits of WDT_CR when WDT_HC_RPL!=1 */
Bit32u WDT_SetRPLen(WDT_PluLenTypeDef PluseLen);
/*set the Response Mode,write RMOD bits of WDT_CR when WDT_HC_RMOD=0 */
Bit32u WDT_SetRepMode(WDT_RMODTypeDef ResponseMode);
/*set the WDT enabled or disabled,write WDT_EN bits of WDT_CR when WDT_ALWAYS_EN = 0*/
Bit32u WDT_CMD(State_TypeDef NewState);
/*set the Timeout period for initialization.,write TOP_INIT bits of WDT_TOPR when WDT_ALWAYS_EN,WDT_HC_TOP=0;WDT_DUAL_TOP=1*/
Bit32u WDT_SetTOP_INIT(Bit32u TOP_INITVal );
/*set the Timeout period ,write TOP bits of WDT_TOPR when WDT_HC_TOP= 0*/
Bit32u WDT_SetTOP(Bit32u TOPVal );
/*get the value of the counter,read WDT_CCVR*/
Bit32u WDT_GetCNTValue(void);
/*restart the counter,clear the interrupt,feed the dog ,write 0x76 into WDT_CRR*/
void WDT_CNT_RStart(void );
/*get the interrupt status of the WDT read WDT_STAT*/
ITStatus_TypeDef WDT_GetITStatus(void);
/*just clear IT flagstatus but not restart the WDT ,read WDT_EOI*/
void WDT_ClearFlag(void);
Bit32u WDT_Deactive(void);
/*set the ResponseMode,PluseLength,TOP_INIT_Value,TOP_Value*/
void WDT_Configuation(WDT_RMODTypeDef ResponseMode,WDT_PluLenTypeDef PluseLength,Bit32u TOP_INIT_Value,Bit32u TOP_Value);
/* register wdt isq  */
Bit32u WDT_IT_Register(void);
Bit32u WDT_Start(void);
void WDT_INIT(void);
void CK_WDT_Handler(Bit32u irqid);
void WDT_Test1(void );
void WDT_Test2(void );





#endif /* WDT_H_ */
