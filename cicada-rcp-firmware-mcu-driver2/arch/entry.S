/*
 * entry.S:
 * CKCORE exception, normal interrupt and fast interrupt entries.
 *
 * Copyright (C) 2012  Hangzhou C-SKY Microsystems Co., Ltd
 * Modify by liu jirang  on 2012-09-11
 */

#include "ck810.h"

.import __start
.import __error_p

.global default_exception_handler
.global hw_vsr_default
.global hw_vsr_fatal
.global hw_vsr_privil
.global hw_vsr_accel
.global hw_vsr_alignment
.global hw_vsr_tlb
.global hw_vsr_fpe
.global hw_vsr_trap
.global hw_vsr_autovec
.global hw_vsr_fiqautovec
.global hw_vsr_inthandler
.global hw_vsr_fiqhandler

.extern CK_INTC_UpdateExceptStatus
.extern CK_INTC_DefaultService
.extern CK_INTC_IrqService
.extern CK_INTC_FiqService
.extern CK_INTC_FastInterruptService
.extern CK_INTC_FpeService
.extern CK_INTC_AlignmentService

.global __get_user_bad
.global __put_user_bad

.macro	SAVE_ALL
	/* save context */
	subi    sp, 28             /* Allocate space for all registers */
	stw     a0, (sp, 0)
	stw     a1, (sp, 4)
	stw     a2, (sp, 8)
	stw     a3, (sp, 12)
	stw     t0, (sp, 16)
	stw     t1, (sp, 20)
	/* local registers need not be saved */
	stw     lr, (sp, 24)

	subi    sp, 8
	mfcr    a3, epsr
	stw     a3, (sp, 4)	/* save epsr registwr */
	mfcr    a2, epc
	stw     a2, (sp, 0)	/* save epc register */
.endm

.macro	RESTORE_ALL
	/* restore all the regs */
	ldw     a3, (sp, 0)
	mtcr    a3, epc		/* restore the epc */
	ldw     a2, (sp, 4)
	mtcr    a2, epsr	/* restore the epsr */
	addi    sp, 8

	ldw     a0, (sp, 0)
	ldw     a1, (sp, 4)
	ldw     a2, (sp, 8)
	ldw     a3, (sp, 12)
	ldw     t0, (sp, 16)
	ldw     t1, (sp, 20)
	/* local registers need not be saved */
	ldw     lr, (sp, 24)
	addi    sp, 28
.endm


/*
 * default exception service routine, before r0 initialized
 */
.section .handler
	.balign 4
default_exception_handler:
	br	default_exception_handler
	rte

/*
 * This function is used to handle default exception.
 * buserr, zerodiv, unrecover, TLB HW unrecover, TLB HW refill
 */
	.balign 4
hw_vsr_default:
	/* save context */
	SAVE_ALL
    /* register excepton status */
	mfcr    a0, psr                /* Get PSR register */
	lsri    a0, 16                 /* Get vector in 7 bits */
	movi    a2, 0x7f
	and     a0, a2
	bsr     CK_INTC_DefaultService   /* Call C level handler */
    /* print vector before UART init */
	mov	    r9, r0
    bsr     __error_p
	/* restore all the regs */
	RESTORE_ALL
    /* @todo Need to discuss Panic(Halt), register TAINT? */
    br      __start
//	rte

/*
 * Alignment_exception handler.
 */
	.balign 4
hw_vsr_alignment:
	/* save context */
	SAVE_ALL
	psrset   ee                         /* Enable Exception */
	mfcr    a0, psr                /* Get PSR register */
	lsri    a0, 16                 /* Get vector in 7 bits */
	movi    a2, 0x7f
	and    	a0, a2
	mov     a1, sp                     /* Push Stack pointer arg */
	bsr     CK_INTC_AlignmentService   /* Call C-level align exception handler */
	/* restore all the regs */
	RESTORE_ALL
	rte

#if !CONFIG_CKCPU_DEFAULT_EXCEPTION
/*
 * This function is used to handle Privil exception.
 */
	.balign 4
hw_vsr_privil:
	mtcr    a0, ss0                /* Save using register */
	mfcr    a0, psr                /* get psr register */
	bseti   a0, 31                 /* Set S bit */
	mtcr    a0, psr                /* Set psr register */
	lsri    a0, 16                 /* Get vector in 7 bits */
	movi    a2, 0x7f
	and     a0, a2
	bsr     CK_INTC_UpdateExceptStatus   /* Call C level handler */
	mfcr    a0, ss0                /* Restore used register */
	rte

/*
 * handle FPU exception.
 */
#if defined(CONFIG_CPU_HAS_FPU)
	.balign 4
hw_vsr_fpe:
	/* save context */
	SAVE_ALL

	/* Clear FPU exception state */
	mfcr    a0, psr                /* get psr register */
	bseti   a0, 31                 /* Set S bit */
	mtcr    a0, psr                /* Set psr register */
	lsri    a0, 16                 /* Get vector in 7 bits */
	movi    a2, 0x7f
	and     a0, a2
	mfcr    a1, cr<2, 2>	            /* fpu fesr is cr<2,2> in CSKY_CPUV2 */
	mov     a2, sp                      /* Push Stack pointer arg */
	bsr     CK_INTC_FpeService          /* Call C-level fpe handler */
	/* restore all the regs */
	RESTORE_ALL
    /* @todo Need to discuss Panic(Halt), register TAINT? */
    br      __start
//	rte
#endif

/*
 * trap 0 for Bochs Exception
 */
	.balign 4
hw_vsr_trap:
	/* save context */
	SAVE_ALL
    /* register excepton status */
	mfcr    a0, psr                /* Get PSR register */
	lsri    a0, 16                 /* Get vector in 7 bits */
	movi    a2, 0x7f
	and     a0, a2
	bsr     CK_INTC_UpdateExceptStatus   /* Call C level handler */
    /* print vector before UART init */
	mov	    r9, r0
    bsr      __error_p
	/* restore all the regs */
	RESTORE_ALL
    /* @todo Need to discuss Panic(Halt), register TAINT? */
    br      __start
//	rte
#endif

/*
 * Normal interrupt vector handler
 */
	.balign 4
hw_vsr_autovec:
	/* save context */
	SAVE_ALL
	psrset  ee                     /* Enable Exception */
	/* read the interrupt vector number from interrupt status register */
	lrw     a3, CK_INTC_BASEADDRESS
	ldw     a0, (a3, 0x0)
	movi    a2, 0x7f
	and     a0, a2
	subi    a0, 32                  /* Real irq nomber need sub VEC offset(32)*/
	bsr     CK_INTC_IrqService

	/* restore all the regs */
	RESTORE_ALL
	rte

/*
 * handle interrupt.
 */
	.balign 4
hw_vsr_inthandler:
	/* save context */
	SAVE_ALL
	psrset  ee                     /* Enable Exception */
	/* read the interrupt vector number from PSR.VEC */
	mfcr    a0, psr                /* Get PSR register */
	lsri    a0, 16                 /* Get vector in 7 bits */
	movi    a2, 0x7f
	and     a0, a2
	subi    a0, 32                  /* Real irq nomber need sub VEC offset(32)*/
	bsr     CK_INTC_IrqService

	/* restore all the regs */
	RESTORE_ALL
	rte

/*
 * Fast interrupt vector handler.
 */
	.balign 4
hw_vsr_fiqautovec:
	/* read the interrupt vector number from interrupt status register */
	lrw     a3, CK_INTC_BASEADDRESS
	ldw     a0, (a3, 0x0)
	movi    a2, 0x7f
	and     a0, a2
	subi    a0, 32                  /* Real irq nomber need sub VEC offset(32)*/
	bsr     CK_INTC_FiqService
	rfi		                        /*   Restore the psr and pc     */

/*
 * handle fast interrupt.
 */
	.balign 4
hw_vsr_fiqhandler:
	/* read the interrupt vector number from PSR.VEC */
	mfcr    a0, psr                /* Get PSR register */
	lsri    a0, 16                 /* Get vector in 7 bits */
	movi    a2, 0x7f
	and     a0, a2
	subi    a0, 32                  /* Real irq nomber need sub VEC offset(32)*/
	bsr     CK_INTC_FiqService
	rfi		                        /*   Restore the psr and pc     */

