/*
 * timer.c
 *
 *  Created on: 2016-11-1
 *  Author: wxy
 */

#include "ck810.h"
#include "datatype.h"
#include "timer.h"
#include "intc.h"
#include "include/io.h"

static CKStruct_TimerInfo CK_Timer_Table[] =
{
   {0, (PCKPStruct_TIMER)CK_TIMER0_BASSADDR, CK_TIMER_IRQ0, FALSE, 0},
   {1, (PCKPStruct_TIMER)CK_TIMER1_BASSADDR, CK_TIMER_IRQ1, FALSE, 0},
   {2, (PCKPStruct_TIMER)CK_TIMER2_BASSADDR, CK_TIMER_IRQ2, FALSE, 0},
   {3, (PCKPStruct_TIMER)CK_TIMER3_BASSADDR, CK_TIMER_IRQ3, FALSE, 0}
};

#define TIMERID_MAX  (sizeof(CK_Timer_Table) / sizeof(CKStruct_TimerInfo) - 1)


/*
 * Make all the timers in the idle state;
 * this function should be called before
 * INTC module working;
 */

void CK_Deactive_TimerModule()
{
    int i;
    PCKPStruct_TIMER ptimer;
    for( i = 0; i < TIMERID_MAX; i++ )
    {
        ptimer = CK_Timer_Table[i].addr;
		/* stop the corresponding timer */
		//ptimer->TxControl &= ~CK_TIMER_TXCONTROL_ENABLE;
        reg_clearl(CK_TIMER_TXCONTROL_ENABLE,ptimer,TxControl);
		/* Disable interrupt. */
		//ptimer->TxControl |= CK_TIMER_TXCONTROL_INTMASK;
        reg_writel(CK_TIMER_TXCONTROL_INTMASK,ptimer,TxControl);
    }
}

/*
 * intilize the timer driver.
 */
void CK_Timer_Init(void)
{
   CK_Deactive_TimerModule();
}


/*
 * open the timer, register the interrupt.
 *
 * timerid: the mumber of the corresponding timer port;
 * handler: the interrupt service function of the corresponding timer;
 * bfast: indicate whether the fast interrupt ornot;
 * bopened: indicate the state whether be opened ornot
 */
Bit32u CK_Timer_Open ( CKEnum_Timer_Device timerid,
		         IN void(*handler)(Bit32u irqid))
{
   PCKStruct_TimerInfo info;
   PCKPStruct_TIMER ptimer;

   if ((timerid < 0) || (timerid > TIMERID_MAX))
   {
	return FAILURE;
   }
   info = &(CK_Timer_Table[timerid]);
   ptimer = info->addr ;
   if(info->bopened)
   {
	return FAILURE;
   }
   /* intilize irqhandler */
   if (NULL != handler)
   {
     CK_INTC_RequestIrq(info->irq, handler);
   }
   info->bopened = TRUE;
   /* Enable Timer interrupt. */
  // ptimer->TxControl &= ~(CK_TIMER_TXCONTROL_INTMASK);
   reg_clearl(CK_TIMER_TXCONTROL_INTMASK,ptimer,TxControl);
  return SUCCESS;
}

/*
 * close the timer, free interrupt.
 *
 * timerid: the mumber of the corresponding timer port;
 * bopened: indicate the state whether be opened ornot
 */
Bit32u CK_Timer_Close(CKEnum_Timer_Device timerid)
{
    PCKStruct_TimerInfo info;
	PCKPStruct_TIMER ptimer;

    if ((timerid < 0) || (timerid > TIMERID_MAX))
    {
	return FAILURE;
    }
    info = &(CK_Timer_Table[timerid]);
    ptimer = info->addr;
    if(!(info->bopened))
    {
	return FAILURE;
    }

   /* stop the corresponding timer */
   //ptimer->TxControl &= ~CK_TIMER_TXCONTROL_ENABLE;
    reg_clearl(CK_TIMER_TXCONTROL_ENABLE,ptimer,TxControl);
   /* Disable interrupt. */
  // ptimer->TxControl |= CK_TIMER_TXCONTROL_INTMASK;
    reg_writel(CK_TIMER_TXCONTROL_INTMASK,ptimer,TxControl);
   /*clear the backcall function*/
   CK_INTC_FreeIrq(info->irq);
   info->bopened = FALSE;
   return SUCCESS;
}



/*
 * start the corresponding timer
 *
 * timerid: the mumber of the corresponding timer port;
 * timeout: the set time (uS)
 */
Bit32u CK_Timer_Start(CKEnum_Timer_Device timerid,IN Bit64u timeout)
{
   Bit64u load;
   PCKStruct_TimerInfo info;
   PCKPStruct_TIMER ptimer;

   if ((timerid < 0) || (timerid > TIMERID_MAX))
   {
	return FAILURE;
   }

   info = &(CK_Timer_Table[timerid]);
   ptimer = info->addr;
   if(!(info->bopened))
   {
	return FAILURE;
   }

   load = (Bit64u)((APB_DEFAULT_FREQ / 1000000) * timeout);
     /*  load time(us)  */

   //ptimer->TxLoadCount_H = load>>32;
   reg_direct_writel(load>>32,ptimer,TxLoadCount_H);
   //printf("load_h is %d \n",(Bit32u)(load>>32));
   //ptimer->TxLoadCount_L = load;
   reg_direct_writel(load,ptimer,TxLoadCount_L);
   //printf("load_l is %d \n",(Bit32u)load);
   info->timeout = timeout;
   /*in user-defined running mode*/
   //ptimer->TxControl |= CK_TIMER_TXCONTROL_MODE;
   reg_writel(CK_TIMER_TXCONTROL_MODE,ptimer,TxControl);
   /* enable the corresponding timer */
   //ptimer->TxControl |= CK_TIMER_TXCONTROL_ENABLE;
   reg_writel(CK_TIMER_TXCONTROL_ENABLE,ptimer,TxControl);
   return SUCCESS;
}

/*
 * stop a designated timer
 *
 * timerid: the mumber of the corresponding timer port;
 * stop_val: the count value when the timer stops
 */
Bit64u CK_Timer_Stop(CKEnum_Timer_Device timerid)
{
   Bit64u stop_val,stop_val_H=0;
   PCKStruct_TimerInfo info;
   PCKPStruct_TIMER ptimer;

   /* if the timer does not open,return failure */
   info = &(CK_Timer_Table[timerid]);
   if(!(info->bopened))
   {
	return FAILURE;
   }

   ptimer = info->addr;
   /* disable the timer*/
   //ptimer->TxControl &= ~CK_TIMER_TXCONTROL_ENABLE;
   reg_clearl(CK_TIMER_TXCONTROL_ENABLE,ptimer,TxControl);
   //stop_val = ptimer->TxCurrentValue_L;
   //stop_val_H = ptimer->TxCurrentValue_H;
   stop_val = reg_readl(ptimer,TxCurrentValue_L);
   stop_val_H = reg_readl(ptimer,TxCurrentValue_H);
   stop_val = stop_val + (stop_val_H<<32);
   return stop_val;
}

/*
 * clear a timer interrupt
 * by reading its End of Interrupt register(EOI)
 */
void  CK_Timer_ClearIrqFlag(CKEnum_Timer_Device timerid)
{
   PCKStruct_TimerInfo info;
   PCKPStruct_TIMER ptimer;

   info = &(CK_Timer_Table[timerid]);
   ptimer = info->addr ;
   //*((volatile Bit32u *)(&(ptimer->TxEOI)));
   reg_readl(ptimer,TxEOI);
}

/*
 * read the current value of the timer
 *
 * timerid: the mumber of the corresponding timer;
 * current_val: the current count-value
 */
Bit64u CK_Timer_CurrentValue(CKEnum_Timer_Device timerid)
{
   Bit64u current_val,current_val_H=0;
   PCKStruct_TimerInfo info;
   PCKPStruct_TIMER ptimer;

   info = &(CK_Timer_Table[timerid]);
   ptimer = info->addr;
   //current_val = ptimer->TxCurrentValue_L;
   //current_val_H = ptimer->TxCurrentValue_H;
   current_val=reg_readl(ptimer,TxCurrentValue_L);
   current_val_H=reg_readl(ptimer,TxCurrentValue_H);
   current_val = current_val+(current_val_H<<32);
   return current_val ;
}



