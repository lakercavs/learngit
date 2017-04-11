/*
  * Description: this file contains the functions support
  *              interrupts and exceptions
  * Copyright (C):  2008 C-SKY Microsystem  Ltd.
  * Author(s): Dongqi Hu  (dongqi_hu@c-sky.com)
  *            Jianyong jiang  (jianyong_jiang@c-sky.com)
  * Contributors: Chunqiang Li
  * Date:         2008-9-26
  */

#include "ck810.h"

#include "datatype.h"
#include "intc.h"
#include "ckintc.h"

#include "misc.h"
#include "cache.h"


/*
 * NR_IRQS: The number of member in irq_desc
 *          respective.It is equal to the number of priority levels.
 */
#define NR_IRQS  32

/*
 * irq_desc[NR_IRQS]: All the normal interrupt service routines should
 *                           been registered in this list.
 *
 */
static CKStruct_IRQHandler irq_desc[NR_IRQS*2];

/* PCK_INTC: The base address of interrupt controller registers */
volatile CKStruct_INTC *icrp = PCK_INTC;

static char *exception_names[] = 
{
    "RESET PC", "BUS ERROR", "ADDRESS ERROR", "ZERO DIVIDE",
    "ILLEGAL INSTRUCTION",  "PRIVILEGE VIOLATION",
    "TRACE", "BREAK POIOT ERROR", "FATAL ERROR" , "Idly4 ERROR",
    "" , "" ,  "UNASSIGNED RESERVED HAI", "UNASSIGNED RESERVED FP",  
    "" , "" , "TRAP #0" , "TRAP #1", "TRAP #2", "TRAP #3",  
    "UNASSIGNED RESERVED 20", "UNASSIGNED RESERVED 21", 
    "UNASSIGNED RESERVED 22", "UNASSIGNED RESERVED 23", 
    "UNASSIGNED RESERVED 24", "UNASSIGNED RESERVED 25", 
    "UNASSIGNED RESERVED 26", "UNASSIGNED RESERVED 27", 
    "UNASSIGNED RESERVED 28", "UNASSIGNED RESERVED 29", 
    "FPU Exception",  "SYSTEM DESCRIPTION POINT", 
};


/*
 * CKCORE_SAVED_CONTEXT -- Saved by a normal interrput or exception
 */
typedef struct
{       
    CK_UINT32 pc;
    CK_UINT32 psr;
    CK_UINT32 r1; 
    CK_UINT32 r2; 
    CK_UINT32 r3; 
    CK_UINT32 r4; 
    CK_UINT32 r5; 
    CK_UINT32 r6; 
    CK_UINT32 r7; 
    CK_UINT32 r8; 
    CK_UINT32 r9; 
    CK_UINT32 r10;
    CK_UINT32 r11;
    CK_UINT32 r12;
    CK_UINT32 r13;
    CK_UINT32 r14;
    CK_UINT32 r15;
} __attribute__ ((aligned, packed)) Ckcore_SavedRegisters;

extern void hw_vsr_autovec();
extern void hw_vsr_fiqautovec();
extern void hw_vsr_default();
extern void hw_vsr_privil();
extern void hw_vsr_alignment();
extern void hw_vsr_fpe();
extern void hw_vsr_trap();
extern void hw_vsr_inthandler();
extern void hw_vsr_fiqhandler();

// exception table
extern volatile unsigned int exception_status_addr;
extern volatile unsigned int fpe_status;
// VSR table
extern  volatile unsigned int ckcpu_vsr_table[256];

/*************************************************************************** 
This function initializes irq_desc[NR_IRQS], fast_irq_list[NR_IRQS]
and PR0-PR31.

***************************************************************************/ 
void CK_INTC_Init(void)
{
  int i;
  for (i = 0; i < NR_IRQS*2; i++)
  {
    irq_desc[i].irqid = 0xffffffff;
    irq_desc[i].handler = NULL;
  }

  /* initialize PR0-PR31, big endian */
  icrp->PR[0] = 0x00000000;
  icrp->PR[1] = 0x00000000;
  icrp->PR[2] = 0x08080808;
  icrp->PR[3] = 0x08080808;
  icrp->PR[4] = 0x10101010;
  icrp->PR[5] = 0x10101010;
  icrp->PR[6] = 0x18181818;
  icrp->PR[7] = 0x18181818;

  icrp->ICR_ISR |= ICR_AVE;
}

/******************************************************************
This function enables a priority level of normal interrupt.

priotity: A priority of normal interrupt which between 0 to 31.

*******************************************************************/
void CK_INTC_EnNormalIrq( IN CK_UINT32 irqid)
{ 
  CK_UINT32 psrbk;

  CK_CPU_EnterCritical(&psrbk);
  icrp->NIER |= (1 << irqid);
  CK_CPU_ExitCritical(psrbk);
}
 
/************************************************************************
This function disables a priority level of normal interrupt.

priotity: A priority of normal interrupt which between 0 to 31.

**********************************************************************/
void CK_INTC_DisNormalIrq(IN CK_UINT32 irqid)
{  
  CK_UINT32 psrbk;

  CK_CPU_EnterCritical(&psrbk);
  icrp->NIER &= ~(1 << irqid);
  CK_CPU_ExitCritical(psrbk);
}

/***********************************************************************
This function enables a priority level of fast interrupt.

priotity: A priority of fast interrupt which between 0 to 31.

***********************************************************************/
void CK_INTC_EnFastIrq(IN CK_UINT32 irqid)
{
  CK_UINT32 psrbk;

  CK_CPU_EnterCritical(&psrbk);
  icrp->FIER |= (1 << irqid);
  CK_CPU_ExitCritical(psrbk);
}

/***************************************************************************
This function disables a priority level of fast interrupt.

priotity: A priority of fast interrupt which between 0 to 31.

**************************************************************************/
void CK_INTC_DisFastIrq(IN CK_UINT32 irqid)
{
  CK_UINT32 psrbk;

  CK_CPU_EnterCritical(&psrbk);
  icrp->FIER &= ~(1 << irqid);
  CK_CPU_ExitCritical(psrbk);
}

/*****************************************************************************
This function enables normal interrupt masking.

primask: The priority level that would be masked, leading to mask those 
         priority levels below primask.

****************************************************************************/
void CK_INTC_MaskNormalIrq(IN CK_UINT32 primask)
{
  IN CK_UINT32 temp_icr;
  CK_UINT32 psrbk;

  temp_icr = icrp->ICR_ISR;

  /*
   * This function will be implemented When fast interrupt masking is disabled,
   * or return at once.
   */
  if((temp_icr &= 0x10000000) == 0x10000000)
  {
    return;
  }

  else
  {
    CK_CPU_EnterCritical(&psrbk);
    icrp->ICR_ISR &= 0xffe0ffff;
    icrp->ICR_ISR |= ((primask & 0x0000001f) << 16);
    icrp->ICR_ISR |= ICR_ME;
    CK_CPU_ExitCritical(psrbk);
  }
}

/******************************************************************
This function disables normal interrupt masking.

primask: The priority level that would be unmasked.

*********************************************************************/
void CK_INTC_UnMaskNormalIrq(void)
{
  CK_UINT32 psrbk;

  CK_CPU_EnterCritical(&psrbk);
  icrp->ICR_ISR &= ~ICR_ME;
  CK_CPU_ExitCritical(psrbk);
}

/************************************************************************
This function enables fast interrupt masking.

primask: The priority level that would be masked, leading to mask those 
         priority levels below primask.

**********************************************************************/
void CK_INTC_MaskFastIrq(IN CK_UINT32 primask)
{
  CK_UINT32 psrbk;

  CK_CPU_EnterCritical(&psrbk);
  icrp->ICR_ISR &= 0xffe0ffff;
  icrp->ICR_ISR |= ((primask & 0x0000001f) << 16);
  icrp->ICR_ISR |= ICR_MFI;
  icrp->ICR_ISR |= ICR_ME;
  CK_CPU_ExitCritical(psrbk);
}

/**********************************************************************
This function disables fast interrupt masking.

primask: The priority level that would be unmasked.

***********************************************************************/
void CK_INTC_UnMaskFastIrq(void)
{
  CK_UINT32 psrbk;

  CK_CPU_EnterCritical(&psrbk);
  icrp->ICR_ISR &= ~ICR_MFI;
  CK_CPU_ExitCritical(psrbk);
}

/**********************************************************************
This function is used for registering interrupt functions
INPUT:
    irqid: irqid need to regist
    pirqhandler:
    The pointer pointing the interrupt data struct which would be registerd.
RETURN VALUE: execute result
    FAILURE: invalid irqid or have been registed
    SUCCESS: registed
********************************************************************/
CK_INT32 CK_INTC_RequestIrq(INOUT CK_UINT32 irqid, IrqHandler pirqhandler)
{
  PCKStruct_IRQHandler phandler;

  /* Judge the validity of irqid and pirqhandler */
  if((irqid < 0) || (irqid > 63) ||
     (NULL == pirqhandler))
  {
    return FAILURE;
  }
  /* If the list of this irqid is empty */
  phandler = &irq_desc[irqid];
  if(phandler -> handler != NULL){
    return FAILURE;
  }
  phandler -> handler = pirqhandler;
  phandler -> irqid = irqid;
  /* If is normal interrupt */
  if(irqid < 32)
  {
    /* Setting Priority */
    CK_UINT32 pr_index;
    CK_UINT32 shift;
    CK_UINT32 psrbk;

    pr_index = (irqid) / 4;
    shift = (3-(irqid) % 4) * 8;
    CK_CPU_EnterCritical(&psrbk);
    icrp->PR[pr_index] &= ~(0x000000ff << shift);
    icrp->PR[pr_index] |= ((irqid&(~0x7)) << shift);
    CK_CPU_ExitCritical(psrbk);
    CK_INTC_EnNormalIrq(irqid);
  }
  /* If is fast interrupt */
  else
  {
     CK_INTC_EnFastIrq(irqid);
  }
  return SUCCESS;
}



/*************************************************************************** 
This function is used for freeing those functions which have been register
INPUT:
    irqid: irqid need to free
RETURN VALUE: execute result
    FAILURE: invalid irqid or not be registed
    SUCCESS: released(can be reused)
**************************************************************************/
CK_INT32 CK_INTC_FreeIrq(INOUT CK_UINT32 irqid)
{
  PCKStruct_IRQHandler phandler;

  /* Judge the validity of irqid */
  if((irqid < 0) || (irqid > 63))
  {
    return FAILURE;
  }
  /* Judge has registered */
  phandler = &irq_desc[irqid];
  if(phandler -> handler == NULL){
    return FAILURE;
  }
  /* Invalidate handler */
  phandler -> handler = NULL;
  phandler -> irqid = 0xFFFFFFFF;
  if(irqid < 32)
  {
    CK_INTC_DisNormalIrq(irqid);
  }
  else
  {
    CK_INTC_DisFastIrq(irqid);
  }

  return SUCCESS;
 }

/*********************************************************
CK_Exception_Init -- Initialize the vector table

INPUT: None

RETURN VALUE: None

*********************************************************/

void CK_Exception_Init (void)
{
  int i;

  /* migrate from silan-ivs2 (liucy) */
  // set all exception vector table as hw_vsr_default
  for (i = 1; i < CKCORE_VECTOR_SYS; i++)
  {
      ckcpu_vsr_table[i] = (CK_UINT32) hw_vsr_default;
  }
  // set interrupt vector table
  for(; (i < 64); i++)
  {
      ckcpu_vsr_table[i] = (CK_UINT32) hw_vsr_inthandler;
  }
  // set fast interrupt vector table
  for(; (i < 96); i++)
  {
      ckcpu_vsr_table[i] = (CK_UINT32) hw_vsr_fiqhandler;
  }

  ckcpu_vsr_table[CKCORE_VECTOR_ALIGN] = (CK_UINT32) hw_vsr_alignment;
#if CONFIG_CKCPU_DEFAULT_EXCEPTION
  ckcpu_vsr_table[CKCORE_VECTOR_PRIV] = (CK_UINT32) hw_vsr_privil;
#if CONFIG_CKCPU_HAS_FPU
  ckcpu_vsr_table[CKCORE_VECTOR_FPE] = (CK_UINT32) hw_vsr_handle_fpe;
#endif
  ckcpu_vsr_table[CKCORE_VECTOR_TRAP] = (CK_UINT32) hhw_vsr_trap;
#endif

  // set the auto interrupt vector table
  ckcpu_vsr_table[CKCORE_VECTOR_AUTOVEC] = (CK_UINT32) hw_vsr_autovec;
  ckcpu_vsr_table[CKCORE_VECTOR_FASTAUTOVEC] = (CK_UINT32) hw_vsr_fiqautovec | 0x1;

  CK_CPU_EnAllNormalIrq();
  CK_CPU_EnAllFastIrq();

  __clear_dcache();
  __flush_icache();

}

/*********************************************************
CK_INTC_UpdateExceptStatus -- When Exception happened, Update Exception Status
called by "Exceptionhandler"

INPUT: 
vector : the exception vector number

regs : None(@todo Register the error code to a Table ? )

RETURN VALUE: None

*********************************************************/

void CK_INTC_UpdateExceptStatus (int vector)
{
    PCKStruct_ExceptSts pExceptSts = (PCKStruct_ExceptSts)exception_status_addr;

//    printf("Exception: %d\n", vector);
    if ( vector >= 32)
    {
        vector = 31;
    }
    pExceptSts[vector].count++;
    pExceptSts[vector].status = 1;
}

/*********************************************************
CK_INTC_DefaultService -- Execute the Default service,
called by "Defaulthandler"

INPUT: 
vector : the exception vector number

regs : None(@todo Register the error code to a Table ? )

RETURN VALUE: None

*********************************************************/

void CK_INTC_DefaultService (int vector)
{
    CK_INTC_UpdateExceptStatus(vector);
}

/*********************************************************
CK_INTC_FpeService -- Execute the Fpe service,
called by "Fpehandler"

INPUT: 
fesr : the states of fpe exception

regs : None(@todo Register the error code to a Table ? )

RETURN VALUE: None

*********************************************************/

void CK_INTC_FpeService (int vector, int fesr, Ckcore_SavedRegisters *regs)
{
    CK_INTC_UpdateExceptStatus(vector);
    /* @todo set external variable to report to simulator ? */
    fpe_status = fesr;
}

/*********************************************************
CK_INTC_IrqService -- Execute the interrupt service,
called by "Inthandler"

INPUT:
 
offset : the offset in  the array irq_desc

RETURN VALUE: None

*********************************************************/

void CK_INTC_IrqService (int offset)
{
  CK_UINT32 volatile saveStatus = icrp->NIER;
  PCKStruct_IRQHandler phandler = &irq_desc[offset];

  /* mask irq  */
  icrp->NIER &= ~(0xff << (((phandler -> irqid) & (~0x7))));
  asm  ("sync 0");

  /* enable nested irq  */
  asm  ("psrset fe,ie");
  /* Call Interrupt Handler */
  phandler -> handler(phandler -> irqid);
  /* unmask irq  */
  asm  ("psrclr fe,ie");
  icrp->NIER = saveStatus;
}

/*********************************************************
CK_INTC_FiqService -- Execute the interrupt service,
called by "Inthandler"

INPUT:
 
offset : the offset in  the array irq_desc

RETURN VALUE: None

*********************************************************/

void CK_INTC_FiqService (int offset)
{
  PCKStruct_IRQHandler phandler = &irq_desc[offset];
  phandler -> handler(phandler -> irqid);
}

/*********************************************************
CK_CPU_EnAllNormalIrq -- Config cpu to enable all normal
interrupt

INPUT: None

RETURN VALUE: None

*********************************************************/

void CK_CPU_EnAllNormalIrq(void)
{
  asm  ("psrset ee,ie");
}

/*********************************************************
CK_CPU_DisAllNormalIrq -- Config cpu to disable all normal
interrupt

INPUT: None

RETURN VALUE: None

*********************************************************/

void CK_CPU_DisAllNormalIrq(void)
{
 asm  ("psrclr ie"); 
}

/*********************************************************
CK_CPU_EnAllFastIrq -- Config cpu to enable all fast
interrupt

INPUT: None

RETURN VALUE: None

*********************************************************/
void CK_CPU_EnAllFastIrq(void)
{
 asm  ("psrset fe");
}

/*********************************************************
CK_CPU_DisAllFastIrq -- Config cpu to disable all fast
interrupt

INPUT: None

RETURN VALUE: None

*********************************************************/
void CK_CPU_DisAllFastIrq(void)
{
  asm  ("psrclr fe"); 
}

/*********************************************************
CK_CPU_EnterCritical -- This function should be called 
before executing critical code which should not be 
interrupted by other interrupts  


INPUT: psr, to store the value of psr

RETURN VALUE: None

*********************************************************/

void CK_CPU_EnterCritical(CK_UINT32 *psr)
{
  asm volatile ("mfcr    %0, psr\n\r"
                "psrclr  ie, fe"
                 : "=r" (*psr) );
}

/*********************************************************
CK_CPU_ExitCritical -- This function should be called 
after exiting critical area.

INPUT: psr, contain the backup value of psr

RETURN VALUE: None

*********************************************************/

void CK_CPU_ExitCritical(CK_UINT32 psr)
{
  asm volatile ("mtcr   %0, psr"
                 : 
                 :"r"(psr));
}

/*********************************************************
CK_CPU_ClearFpeStatus -- This function should be called 
befroe Silumator Execute float calculation

INPUT: None

RETURN VALUE: None

*********************************************************/
void CK_CPU_ClearFpeStatus(void)
{
    fpe_status = 0;
}

/*********************************************************
CK_CPU_GetFpeStatus -- This function should be called 
After Silumator Execute float calculation

INPUT: None

RETURN VALUE: fpe_status

*********************************************************/
CK_UINT32 CK_CPU_GetFpeStatus(void)
{
    return fpe_status;
}

/*********************************************************
CK_CPU_ClearExceptStatus -- This function should be called 
When Silumator wants to get Exception Status

INPUT: None

RETURN VALUE: None

*********************************************************/
void CK_CPU_ClearExceptStatus(int vector)
{
    PCKStruct_ExceptSts pExceptSts = (PCKStruct_ExceptSts)exception_status_addr;

    pExceptSts[vector].count = 0;
    pExceptSts[vector].status = 0;
}

/*********************************************************
CK_CPU_GetExceptStatus -- This function should be called 
When Silumator wants to get Exception Status

INPUT: None

RETURN VALUE: exception_status

*********************************************************/
PCKStruct_ExceptSts CK_CPU_GetExceptStatus(int vector)
{
    PCKStruct_ExceptSts pExceptSts = (PCKStruct_ExceptSts)exception_status_addr;
    return &pExceptSts[vector];
}

/*********************************************************
CK_CPU_TrapTrigger -- This function should be called 
When Silumator want to trap

INPUT: None

RETURN VALUE: None

*********************************************************/
void CK_CPU_TrapTrigger(void)
{
    asm  ("trap 0"); 
}

