/*
 * rtc.c
 *
 *  Created on: 2016-9-21
 *  Author: wxy
 */

#include "rtc.h"
#include "io.h"
#include "intc.h"
#include "misc.h"

static RTC_MODE_TypeDef Mode;

void RTC_Deactive(void)
{
	/*disable rtc*/
	RTC_IT_CMD(DISABLE);
	RTC_EN_CMD(DISABLE);
	RTC_Wrap_CMD(DISABLE);
}

/*config RTC mode,matchvalue,loadvalue,ITmusk state*/
void RTC_Configuation(RTC_MODE_TypeDef RTCMode,Bit32u MatchValue,Bit32u LoadValue)
{
	RTC_SetMTCHValue(MatchValue);
	RTC_SetLoadValue(LoadValue);
	Mode=RTCMode;
}

/* config intc,register rtc isq  */
Bit32u RTC_IT_Register(void)
{
	/* clear interrupt flag */
    RTC_ClearITFlag();
    /* register rtc isq  */
    CK_INTC_RequestIrq(CK_RTC_IRQ, CK_RTC_Handler);
    /* set rtc IT state  */
    return 0;
}



/*Enable interrupt(if needed),Enable the RTC*/
Bit32u RTC_Start(void)
{
	if(Mode==RTC_WRAP_MODE)
	{
		RTC_Wrap_CMD(ENABLE);
		RTC_EN_CMD(ENABLE);
		return 0;
	}

	else if(Mode==RTC_CNTEN_MODE)
    {
		RTC_Wrap_CMD(DISABLE);
        RTC_EN_CMD(ENABLE);
        return 0;
    }
	else
		return 1;
}

void RTC_Stop(void)
{
	RTC_EN_CMD(DISABLE);
	RTC_Wrap_CMD(DISABLE);
}

/*init rtc with default */
void RTC_Init(void)
{
	RTC_Configuation(RTC_CNTEN_MODE,10000,0);
	RTC_IT_Register();
    RTC_ITMask_CMD(RTC_IT_UMUSK);
    RTC_IT_CMD(ENABLE);
	RTC_Start();
}

/*get the value of the counter,read RTC_CCVR*/
Bit32u RTC_GetCNTValue(void)
{
  Bit32u tmp = 0;
  //tmp = RTC->RTC_CCVR;
  tmp = reg_readl(RTC,RTC_CCVR);
  return  tmp ;
}

/*set the match value,write RTC_CMR*/
void RTC_SetMTCHValue(Bit32u MatchValue)
{
  //RTC->RTC_CMR = MatchValue;
  reg_direct_writel(MatchValue,RTC,RTC_CMR);
}

/*set the loaded value,write RTC_CLR*/
void RTC_SetLoadValue(Bit32u LoadValue)
{
  //RTC->RTC_CLR = LoadValue;
  reg_direct_writel(LoadValue,RTC,RTC_CLR);
}

/*set the RTC wrap enabled or disabled,set the rtc_wen bit of the CCR*/
void RTC_Wrap_CMD( State_TypeDef NewState)
{
	if (NewState == ENABLE)
	{
		//RTC->RTC_CCR |= RTC_WEN;
		reg_writel(RTC_WEN,RTC,RTC_CCR);
	}
	else
	{
		//RTC->RTC_CCR &= ~RTC_WEN;
		reg_clearl(RTC_WEN,RTC,RTC_CCR);
	}
}

/*set the RTC counter enabled or disabled,set the rtc_en bit of the CCR*/
void RTC_EN_CMD( State_TypeDef NewState)
{
	if (NewState == ENABLE)
	{
		//RTC->RTC_CCR |= RTC_EN;
		reg_writel(RTC_EN,RTC,RTC_CCR);
	}
	else
	{
		//RTC->RTC_CCR &= ~RTC_EN;
		reg_clearl(RTC_EN,RTC,RTC_CCR);
	}
}

/*set the RTC interrupt masked or unmasked,set the rtc_mask bit of the CCR*/
void RTC_ITMask_CMD( RTC_ITMusk_TypeDef ITMaskState)
{
	if (ITMaskState == RTC_IT_MUSK)
	{
		//RTC->RTC_CCR |= RTC_MASK;
		reg_writel(RTC_MASK,RTC,RTC_CCR);
	}
	else
	{
		//RTC->RTC_CCR &= ~RTC_MASK;
		reg_clearl(RTC_MASK,RTC,RTC_CCR);
	}
}

/*set the RTC interrupt enabled or disabled,set the rtc_ien bit of the CCR*/
void RTC_IT_CMD( State_TypeDef NewState)
{
	if (NewState == ENABLE)
	{
		//RTC->RTC_CCR |= RTC_IEN;
		reg_writel(RTC_IEN,RTC,RTC_CCR);
	}
	else
	{
		//RTC->RTC_CCR &= ~RTC_IEN;
		reg_clearl(RTC_IEN,RTC,RTC_CCR);
	}
}

/*get the interrupt status,read RTC_stat*/
ITStatus_TypeDef RTC_GetITStat(void)
{
	ITStatus_TypeDef bitstatus = INACTIVE;
	//if(((RTC->RTC_STAT) & RTC_ITSTAT)==INACTIVE)
	if((reg_readl(RTC,RTC_STAT) & RTC_ITSTAT)==INACTIVE)
	{
		bitstatus = INACTIVE;
	}
	else
	{
		bitstatus = ACTIVE;               /* Interrupt is active */
	}
	return  bitstatus;                    /* Return the SPI_FLAG status */
}

/*get the interrupt raw status,read RTC_rstat*/
ITStatus_TypeDef RTC_GetITRStat(void)
{
	ITStatus_TypeDef bitstatus = INACTIVE;
	//if ((RTC->RTC_RSTAT & RTC_ITRSTAT) == (Bit32u) INACTIVE)
	if ((reg_readl(RTC,RTC_RSTAT) & RTC_ITRSTAT) == (Bit32u) INACTIVE)
	{
		bitstatus = INACTIVE;             /* Interrupt is inactive */
	}
	else
	{
		bitstatus = ACTIVE;               /* Interrupt is active */
	}
	return  bitstatus;                    /* Return the status */
}

/*clear the it flag,read RTC_EOI*/
void RTC_ClearITFlag(void)
{
	Bit32u tmp;
	//tmp=RTC->RTC_EOI;
	tmp = reg_readl(RTC,RTC_EOI);
}

/*get the comp version,RTC_COMP_VERSION*/
Bit32u RTC_GetCompVer(void)
{
  Bit32u tmp = 0;
  //tmp = RTC->RTC_COMP_VERSION;
  tmp = reg_readl(RTC,RTC_COMP_VERSION);
  return  tmp ;
}
