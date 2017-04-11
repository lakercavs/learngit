/*
 * wdt.c
 *
 *  Created on: 2016-9-21
 *  Author: wxy
 *  A/0
 */
#include "wdt.h"
#include "io.h"
#include "intc.h"
#include "misc.h"

static WDT_RMODTypeDef Mode;

Bit32u WDT_Deactive(void)
{
	WDT_CMD(DISABLE);
	return 0;
}

/*set the ResponseMode,PluseLength,TOP_INIT_Value,TOP_Value*/
void WDT_Configuation(WDT_RMODTypeDef ResponseMode,WDT_PluLenTypeDef PluseLength,Bit32u TOP_INIT_Value,Bit32u TOP_Value)
{
	WDT_SetRepMode(ResponseMode);
	WDT_SetRPLen(PluseLength);
	WDT_SetTOP_INIT(TOP_INIT_Value );
	WDT_SetTOP(TOP_Value);
}

/* register wdt isq  */
Bit32u WDT_IT_Register(void)
{
  	/* clear interrupt flag */
    WDT_ClearFlag();
	if(Mode==GER_IT)
	{
		printf ("Begin to irq\n");
        /* register wdt isq  */
        CK_INTC_RequestIrq(CK_WDT_IRQ, CK_WDT_Handler);
        return 0;
	}
	else
		return 1;
}

/*set WDT enabled,write WDT_EN bits of WDT_CR when WDT_ALWAYS_EN = 0*/
Bit32u WDT_Start(void)
{
	WDT_CMD(ENABLE);
	return 0;
}

void WDT_INIT(void)
{
	WDT_Deactive();
	WDT_Configuation(GER_IT,NULL,0,0);
	WDT_IT_Register();
	WDT_Start();
}

/*set the WDT enabled or disabled,write WDT_EN bits of WDT_CR when WDT_ALWAYS_EN = 0*/
Bit32u WDT_CMD(State_TypeDef NewState)
{
	//if (((WDTCOMP->WDT_COMP_PARAMS_1) & WDT_ALWAYS_EN)== 0)      // WDT_ALWAYS_EN=0,CR_WDT_EN is useful
	if ((reg_readl(WDTCOMP,WDT_COMP_PARAMS_1) & WDT_ALWAYS_EN)== 0)      // WDT_ALWAYS_EN=0,CR_WDT_EN is useful
	{
		if (NewState == ENABLE)
		{
			//WDT->WDT_CR |= WDT_ENA;
			reg_writel(WDT_ENA,WDT,WDT_CR);
		}
		else
		{
			//WDT->WDT_CR &= ~WDT_ENA;
			reg_clearl(WDT_ENA,WDT,WDT_CR);
		}
		return 0;
	}
	else
		return 1;
}

/*restart the counter,clear the interrupt,feed the dog ,write 0x76 into WDT_CRR*/
void WDT_CNT_RStart(void )
{
	//WDT->WDT_CRR=0x76;                              //write  WDT_CRR
	reg_direct_writel(0x76,WDT,WDT_CRR);
}


/*set the Reset pulse length,write RPL bits of WDT_CR when WDT_HC_RPL!=1 */
Bit32u WDT_SetRPLen(WDT_PluLenTypeDef PluseLen)
{
	//if ((WDTCOMP->WDT_COMP_PARAMS_1 & WDT_HC_RPL)== 0)     // WDT_HC_RPL=0,CR_RPL is useful
	if ((reg_readl(WDTCOMP,WDT_COMP_PARAMS_1) & WDT_HC_RPL)== 0)     // WDT_HC_RPL=0,CR_RPL is useful
	{
		Bit32u tmp;
		//tmp=WDT->WDT_CR;                                     //get original value of WDT_CR
		tmp=reg_readl(WDT,WDT_CR);
		tmp &= ~0x1C;                                        //clear RPL bits of WDT_CR
		tmp|= PluseLen;                                      //set RPL bits of WDT_CR
		//WDT->WDT_CR=tmp;                                     //write  WDT_CR
		reg_direct_writel(tmp,WDT,WDT_CR);
		return 0;
	}
	else
		return 1;
}

/*set the Response Mode,write RMOD bits of WDT_CR when WDT_HC_RMOD=0 */
Bit32u WDT_SetRepMode(WDT_RMODTypeDef RespMode)
{
	//if (((WDTCOMP->WDT_COMP_PARAMS_1) & WDT_HC_RMOD)== 0)     // WDT_HC_RMOD=0,CR_RMOD is useful
	if ((reg_readl(WDTCOMP,WDT_COMP_PARAMS_1) & WDT_HC_RMOD)== 0)     // WDT_HC_RMOD=0,CR_RMOD is useful
	{
		Bit32u tmp;
		//tmp=WDT->WDT_CR;                                      //get original value of WDT_CR
		tmp=reg_readl(WDT,WDT_CR);
		tmp&= ~0x03;                                          //clear RMOD bits of WDT_CR
		tmp|= RespMode;                                       //set RMOD bits of WDT_CR
		//WDT->WDT_CR=tmp;                                      //write  WDT_CR
		reg_direct_writel(tmp,WDT,WDT_CR);
		Mode=RespMode;
		return 0;
	}
	else
		return 1;
}

/*set the Timeout period for initialization.,write TOP_INIT bits of WDT_TOPR when WDT_ALWAYS_EN,WDT_HC_TOP=0;WDT_DUAL_TOP=1*/
Bit32u WDT_SetTOP_INIT(Bit32u TOP_INITVal )
{
	//if((((WDTCOMP->WDT_COMP_PARAMS_1) & WDT_ALWAYS_EN)== 0)&
	//   (((WDTCOMP->WDT_COMP_PARAMS_1) & WDT_HC_TOP)== 0)&
	//   (((WDTCOMP->WDT_COMP_PARAMS_1) & WDT_DUAL_TOP)!= 0))  /*WDT_ALWAYS_EN,WDT_HC_TOP=0;WDT_DUAL_TOP=1*/
	if(((reg_readl(WDTCOMP,WDT_COMP_PARAMS_1) & WDT_ALWAYS_EN)== 0)&
		((reg_readl(WDTCOMP,WDT_COMP_PARAMS_1) & WDT_HC_TOP)== 0)&
		((reg_readl(WDTCOMP,WDT_COMP_PARAMS_1) & WDT_DUAL_TOP)!= 0))  /*WDT_ALWAYS_EN,WDT_HC_TOP=0;WDT_DUAL_TOP=1*/
	{
		Bit32u tmp;
		//tmp=WDT->WDT_TORR;                              //get original value of WDT_TORR
		tmp=reg_readl(WDT,WDT_TORR);
		tmp&= ~0xF0;                                    //clear TOP_INIT bits of WDT_TORR
		tmp|= TOP_INT(TOP_INITVal);                     //set TOP_INIT bits of WDT_TORR
		//WDT->WDT_TORR=tmp;                              //write  WDT_TORR
		reg_direct_writel(tmp,WDT,WDT_TORR);
		return 0;
	}
	else
		return 1;
}

/*set the Timeout period ,write TOP bits of WDT_TOPR when WDT_HC_TOP= 0*/
Bit32u WDT_SetTOP(Bit32u TOPVal )
{
	//if(((WDTCOMP->WDT_COMP_PARAMS_1) & WDT_HC_TOP)== 0)
	if((reg_readl(WDTCOMP,WDT_COMP_PARAMS_1) & WDT_HC_TOP)== 0)
	{
		Bit32u tmp;
		//tmp=WDT->WDT_TORR;                              //get original value of WDT_TORR
		tmp=reg_readl(WDT,WDT_TORR);
		tmp&= ~0x0F;                                    //clear TOP bits of WDT_TORR
		tmp|= TOP(TOPVal);                              //set TOP bits of WDT_TORR
		//WDT->WDT_TORR=tmp;                              //write  WDT_TORR
		reg_direct_writel(tmp,WDT,WDT_TORR);
		return 0;
	}
	else
		return 1;
}

/*get the value of the counter,read WDT_CCVR*/
Bit32u WDT_GetCNTValue(void)
{
  Bit32u tmp = 0;
  //tmp = WDT->WDT_CCVR;
  tmp=reg_readl(WDT,WDT_CCVR);
  return  tmp ;
}

/*get the interrupt status of the WDT read WDT_STAT*/
ITStatus_TypeDef WDT_GetITStatus(void)
{
	FlagStatus_TypeDef bitstatus = INACTIVE;
	//if ((WDT->WDT_STAT & WDT_ITSTAT) != (Bit32u) INACTIVE)
	if ((reg_readl(WDT,WDT_STAT) & WDT_ITSTAT) != (Bit32u) INACTIVE)
	{
		bitstatus = ACTIVE;                 /* Interrupt is active */
	}
	else
	{
		bitstatus = INACTIVE;               /* Interrupt is inactive */
	}
	return  bitstatus;                      /* Return the ITstatus */
}

/*just clear IT flagstatus but not restart the WDT ,read WDT_EOI*/
void WDT_ClearFlag(void)
{
	//Bit32u tmp = WDT->WDT_EOI;
	Bit32u tmp=reg_readl(WDT,WDT_EOI);
}














