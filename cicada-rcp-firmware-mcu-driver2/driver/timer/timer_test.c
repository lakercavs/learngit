/*
 * timer_test.c
 *
 *  Created on: 2016-11-1
 *      Author: i
 */

#include "ck810.h"
#include "timer.h"
#include "misc.h"


#define CK_TIMER_TEST_TIME       1000000  /* 1s,less than 32bits */
//#define CK_TIMER_TEST_TIME       210000000  /* 210s,more than 32 bits */
#define TIMERID_MAX  4

Bit8u timer_test;
Bit32u t_timerid;


/*
 * Callback function for timer interrupt, set timer_flag.
 */
static void CK_TIMER_Handler(Bit32u irqid)
{
  CK_Timer_ClearIrqFlag(t_timerid);
  timer_test = 0;
}

/*
 * main function of timer test program.
 */
void CK_Timer_Test()
{
  Bit64u currentvalue;
  Bit64u lastvalue;
  Bit32u i;

  printf("Timer test begin \n");
  for(i = 0; i < TIMERID_MAX; i++)
  {
    t_timerid = i;
    printf(" Testing Timer%d \n ", t_timerid);
    timer_test = 1;
    {
      CK_Timer_Open(t_timerid,CK_TIMER_Handler);
    }

    CK_Timer_Start(t_timerid, CK_TIMER_TEST_TIME);
    currentvalue = CK_Timer_CurrentValue(t_timerid);
    /* if currentvalue > lastvalue, it means that it's time out
        but there is no interrupt generate. */
    while(timer_test)
    {
      lastvalue = currentvalue;
      currentvalue = CK_Timer_CurrentValue(t_timerid);
      if(currentvalue > lastvalue)
      {
        break;
      }
    }
    CK_Timer_Stop(t_timerid);
    CK_Timer_Close(t_timerid);
    if(timer_test == 0)
        printf("Testing Timer%d success\n", t_timerid);
    else
        printf("Testing Timer%d failure\n", t_timerid);
  }
}








