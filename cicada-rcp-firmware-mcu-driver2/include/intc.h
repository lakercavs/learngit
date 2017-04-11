/*
 *  intr.h - Define the struct and interface for interrupt controller
 *
 *  Copyright (C):  2008 Hangzhou C-SKY Microsystems Co.,LTD.
 *  Author: Dongqi Hu  (dongqi_hu@c-sky.com)
 *          Jianyong Jiang  (jianyong_jiang@c-sky.com)
 *  Contributiors: Chunqiang Li  
 *  Date: 2008-09-26
 *  Modify by liu jirang  on 2012-09-11
 */           

#ifndef _INTR_H_
#define _INTR_H_

#include "datatype.h"

#define CKCORE_VECTOR_SYS  32
#define CKCORE_VECTOR_MAX  256

/* migrate from silan-ivs2 (liucy) */
#define CKCORE_VECTOR_RESET         0
#define CKCORE_VECTOR_ALIGN         1
#define CKCORE_VECTOR_ACCESS        2
#define CKCORE_VECTOR_ZERODIV       3
#define CKCORE_VECTOR_ILLEGAL       4
#define CKCORE_VECTOR_PRIV          5
#define CKCORE_VECTOR_TRACE         6
#define CKCORE_VECTOR_BREAKPOINT    7
#define CKCORE_VECTOR_UNRECOVER     8
#define CKCORE_VECTOR_IDLY          9
#define CKCORE_VECTOR_AUTOVEC       10
#define CKCORE_VECTOR_FASTAUTOVEC   11
#define CKCORE_VECTOR_HWACCEL       12
#define CKCORE_VECTOR_TLBUNRECOVER  13

#define	CKCORE_VECTOR_TLBMISS	    14
#define	CKCORE_VECTOR_TLBMODIFIED	15

#define	CKCORE_VECTOR_TRAP	        16

#define	CKCORE_VECTOR_TLBINVALIDL	20
#define	CKCORE_VECTOR_TLBINVALIDS	21

#define CKCORE_VECTOR_PRFL          29
#define CKCORE_VECTOR_FPE           30

/* define the data structure of interrupt description */
typedef void (*IrqHandler)(CK_UINT32);
typedef struct CKS_IRQ_Handler{
//      char        *devname;
      CK_UINT32   irqid;
//    CK_UINT32    priority;
      IrqHandler  handler;
}CKStruct_IRQHandler, *PCKStruct_IRQHandler;

typedef struct CKS_Exception_Status{
    CK_UINT8 count:7;
    CK_UINT8 status:1;
}CKStruct_ExceptSts[32], *PCKStruct_ExceptSts;

/* Statement of those functions which are used in intc.c*/
void CK_CPU_EnAllNormalIrq(void);
void CK_CPU_DisAllNormalIrq(void);
void CK_CPU_EnAllFastIrq(void);
void CK_CPU_DisAllFastIrq(void);
void CK_CPU_EnterCritical(CK_UINT32 *psr);
void CK_CPU_ExitCritical(CK_UINT32 psr);
void CK_INTC_EnNormalIrq(IN CK_UINT32 irqid);
void CK_INTC_DisNormalIrq(IN CK_UINT32 irqid);
void CK_INTC_EnFastIrq(IN CK_UINT32 irqid); 
void CK_INTC_DisFastIrq(IN CK_UINT32 irqid);
void CK_INTC_MaskNormalIrq(IN CK_UINT32 primask);
void CK_INTC_UnMaskNormalIrq(void);
void CK_INTC_MaskFastIrq(IN CK_UINT32 primask);
void CK_INTC_UnMaskFastIrq(void);
CK_INT32 CK_INTC_RequestIrq(INOUT CK_UINT32 irqid, IrqHandler pirqhandler);
CK_INT32 CK_INTC_FreeIrq(INOUT CK_UINT32 irqid);

void CK_CPU_ClearFpeStatus(void);
CK_UINT32 CK_CPU_GetFpeStatus(void);
void CK_CPU_ClearExceptStatus(int vector);
PCKStruct_ExceptSts CK_CPU_GetExceptStatus(int vector);

void CK_CPU_TrapTrigger(void);

#endif
