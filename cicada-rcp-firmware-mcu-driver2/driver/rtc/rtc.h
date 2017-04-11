/*
 * rtc.h
 *
 *  Created on: 2016-9-21
 *  Author: wxy
 */

#ifndef RTC_H_
#define RTC_H_

#include "io.h"
#include "ck810.h"
#include "datatype.h"
#include "intc.h"
#include "types.h"

#define RTC_VERSION  	0x3130302A			   /* should be modified */
#define RTC_CLK         62500000               /*APB CLK or external clock,should be modified*/
typedef struct{
	volatile Bit32u RTC_CCVR;		/* 0x00 Current Counter Value Register */
	volatile Bit32u RTC_CMR;		/* 0x04 Counter Match Register */
	volatile Bit32u RTC_CLR;		/* 0x08 Counter Load Register */
	volatile Bit32u RTC_CCR;		/* 0x0c Counter Control Register */
	volatile Bit32u RTC_STAT;		/* 0x10 Interrupt Status Register */
	volatile Bit32u RTC_RSTAT;		/* 0x14 Interrupt Raw Status Register */
	volatile Bit32u RTC_EOI;		/* 0x18 End of Interrupt Register */
	volatile Bit32u RTC_COMP_VERSION;/* 0x1C Component Version Register */
}RTC_RegTypeDef;

#define RTC    ((RTC_RegTypeDef *) CK_RTC_ADDRBASE)          //define rtc reg struct,and get the rtc addrbase

#define RTC_CMV(x)     x        /*  Match Counter Value  */
#define RTC_CLV(x)     x        /*  Counter Load Value  */
#define RTC_WEN     0x08        /*  Wrap enabled  */
#define RTC_EN      0x04        /*  Counter enabled  */
#define RTC_MASK    0x02        /*  Interrupt masked  */
#define RTC_IEN     0x01        /*  Interrupt enabled  */

#define RTC_ITSTAT      0x01        /*  Interrupt is active  */
#define RTC_ITRSTAT     0x01        /*  Interrupt is active  */
#define RTC_EOI_BIT     0x01        /*  By reading this location, the match interrupt is cleared.  */

#define  CK_RTC_IRQ   CK_INTC_RTC

typedef enum
{
  RTC_CNTEN_MODE = 1,
  RTC_WRAP_MODE = 2
}RTC_MODE_TypeDef;

typedef enum
{
  RTC_IT_MUSK = 1,
  RTC_IT_UMUSK = 0
}RTC_ITMusk_TypeDef;

typedef enum
{
  RTC_IT_ENABLE = 1,
  RTC_IT_DISABLE = 0
}RTC_IT_TypeDef;

typedef struct{
	Bit32u MatchValue;                   /*unit:ns*/
	Bit32u LoadValue;                    /*unit:ns*/
    RTC_MODE_TypeDef RTCMode;
    RTC_ITMusk_TypeDef ITMaskState;
    RTC_IT_TypeDef ITState;
}RTC_InitTypeDef;

void rtc_bit_write(Bit32u val, void* addr);
void rtc_bit_clear(Bit32u val, void* addr);
/*get the value of the counter,read RTC_CCVR*/
Bit32u RTC_GetCNTValue(void);
/*set the match value,write RTC_CMR*/
void RTC_SetMTCHValue(Bit32u MatchValue);
/*set the loaded value,write RTC_CLR*/
void RTC_SetLoadValue(Bit32u LoadValue);
/*set the RTC wrap enabled or disabled,set the rtc_wen bit of the CCR*/
void RTC_Wrap_CMD( State_TypeDef NewState);
/*set the RTC counter enabled or disabled,set the rtc_en bit of the CCR*/
void RTC_EN_CMD( State_TypeDef NewState);
/*set the RTC interrupt masked or unmasked,set the rtc_mask bit of the CCR*/
void RTC_ITMask_CMD( RTC_ITMusk_TypeDef ITMaskState);
/*set the RTC interrupt enabled or disabled,set the rtc_ien bit of the CCR*/
void RTC_IT_CMD( State_TypeDef NewState);
/*get the interrupt status,read RTC_stat*/
ITStatus_TypeDef RTC_GetITStat(void);
ITStatus_TypeDef RTC_GetITRStat(void);
/*clear the it flag,read RTC_EOI*/
void RTC_ClearITFlag(void);
void CK_RTC_Handler(Bit32u irqid);
/*get the comp version,RTC_COMP_VERSION*/
Bit32u RTC_GetCompVer(void);
void RTC_Deactive(void);
/*config RTC mode,matchvalue,loadvalue,ITmusk state*/
void RTC_Configuation(RTC_MODE_TypeDef RTCMode,Bit32u MatchValue,Bit32u LoadValue);
/* register rtc isq  */
Bit32u RTC_IT_Register(void);
/*Enable interrupt(if neeeded),Enable the RTC*/
Bit32u RTC_Start(void);
void RTC_Stop(void);
void RTC_Init(void);
void RTC_Test1(void );
void RTC_Test2(void );
void RTC_Test3_1(void );
void RTC_Test3_2(void );
void RTC_Test4(void );













#endif /* RTC_H_ */
