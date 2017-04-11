/*
 * timer.h
 *
 *  Created on: 2016-11-1
 *      Author: i
 */

#ifndef TIMER_H_
#define TIMER_H_


#include "ck810.h"
#include "datatype.h"
#include "intc.h"
#include "types.h"

/*
 * define number of the timer interrupt
 */
#define  CK_TIMER_IRQ0   CK_INTC_TIM0
#define  CK_TIMER_IRQ1   CK_INTC_TIM1
#define  CK_TIMER_IRQ2   CK_INTC_TIM2
#define  CK_TIMER_IRQ3   CK_INTC_TIM3

typedef struct CKS_TIMER
{
    Bit32u    TxLoadCount_L;     /*  Timer Value to be loaded into Timer */
    Bit32u    TxLoadCount_H;     /*  Timer Value to be loaded into Timer */
    Bit32u    TxCurrentValue_L;  /*  Timer Current Value of Timer */
    Bit32u    TxCurrentValue_H;  /*  Timer Current Value of Timer */
    Bit32u    TxControl;         /* Timer Control Register for Timer */
    Bit32u    TxIntStatus;       /* Timer0 Contains the interrupt status for Timer*/
    Bit32u    TxEOI;	         /* Timer Clears the interrupt from Timer */
} CKStruct_TIMER,* PCKPStruct_TIMER;

typedef struct {
        Bit32u id;                    /* the number of timer */
        PCKPStruct_TIMER  addr;       /* the base-address of timer */
        Bit32u irq;                   /* the interruput mumber of timer */
        BOOL bopened;                 /* indicate whether have been opened or not */
        Bit64u timeout;               /* the set time (us) */
        CKStruct_IRQHandler irqhandler;

} CKStruct_TimerInfo, * PCKStruct_TimerInfo;

/*
 *  define the bits for TxControl
 */
#define CK_TIMER_TXCONTROL_ENABLE      (1UL << 0)
#define CK_TIMER_TXCONTROL_MODE        (1UL << 1)
#define CK_TIMER_TXCONTROL_INTMASK     (1UL << 2)



#define  PCK_TIMER_CONTROL  ((PCKStruct_TIMER_CON)CK_TIMER_CONTROL_BASSADDR)


typedef struct CKS_TIMER_CON
{
    Bit32u    IntStatus;	    /*  Contains the interrupt status of all
	                             *  timers in the component.
	                             */
    Bit32u    EOI;		    /*  Returns all zeroes (0) and clears all
	                             *  active interrupts.
	                             */
    Bit32u    RawIntStatus;	    /*  Contains the unmasked interrupt status
	                             *  of all timers in the component.
	                             */

} CKStruct_TIMER_CON,* PCKStruct_TIMER_CON;

/*
 * enum the timer
 */
typedef enum
{
    TIMER0,
    TIMER1,
    TIMER2,
    TIMER3,
    TIMER4,
} CKEnum_Timer_Device;


/*
 * intilize the timer driver.
 */
void CK_Timer_Init(void);


/*
 * open the timer, register the interrupt.
 *
 * timerid: the mumber of the corresponding timer port;
 * handler: the interrupt service function of the corresponding timer;
 */
Bit32u CK_Timer_Open ( CKEnum_Timer_Device timerid,IN void(*handler)(Bit32u irqid));

/*
 * close the timer, free interrupt.
 *
 * timerid: the mumber of the corresponding timer port;
 */
Bit32u CK_Timer_Close ( CKEnum_Timer_Device timerid );

/*
 * start the corresponding timer
 *
 * timerid: the mumber of the corresponding timer port;
 * timeout: the set time
 */
Bit32u CK_Timer_Start ( CKEnum_Timer_Device timerid, IN Bit64u timeout);

/*
 * stop a designated timer
 *
 * timerid: the mumber of the corresponding timer port;
 * stop_val: the count value when the timer stops
 */
Bit64u CK_Timer_Stop ( CKEnum_Timer_Device timerid );

/*
 * clear a timer interrupt
 * by reading its End of Interrupt register(EOI)
 */
void CK_Timer_ClearIrqFlag ( CKEnum_Timer_Device timerid );

/*
 * read the current value of the timer
 *
 * timerid: the mumber of the corresponding timer;
 * current_val: the current count-value
 */
Bit64u CK_Timer_CurrentValue ( CKEnum_Timer_Device timerid );
void CK_Timer_Test();

#endif /* TIMER_H_ */
