/*
 * crt0.S -- startup codes for CK-CPU.
 *
 * Define a exception vector table
 *
 * Initial PSR register.
 * Initial VBR register, relocate exception vector table.
 * Initial sp(stack pointer) register.
 * Initial MMU (for ckcpu having MMU)
 * Initial Cache.
 * Initial BSS section to "0"
 * Then goto "main".
 *
 * Copyright (C) 2012  Hangzhou C-SKY Microsystems Co., Ltd
 */

#include "config.h"

.file "crt0.S"


/*
 * For importing variable or functions from script file "ckcpu.ld".
 */
.import __stack		/* The stack pointer from linking script file */
.import	__sbss		/* The end of bss from linking script file */
.import __ebss		/* The end of bss from linking script file */

/*
 * For importing variable or functions from other c or assemble files.
 */
.import main				/* The c codes entry */
.import printhex4
.import printascii

/*
 * For importing variable or functions from entry.S.
 */
.import default_exception_handler /* default exception service routine */

/* reset mode cold or warm */
.export reset_status

/* For exporting varialbe or functions */
//.export _ramvec
.export exception_status_addr
.export fpe_status
.export	ckcpu_vsr_table	/* Vector table base address. */
.export __start		    /* The system entry. */
.export __error_p		/* Error print rountine. */


.data

//_ramvec:
//.long	0

reset_status:
.long   0
exception_status_addr:
.fill	32, 1, 0
fpe_status:
.long   0

str_p1:
.asciz	"\nException happened psr.vec[] is (0x"
str_p2:
.asciz	").\n"
str_p3:
.asciz	"\nIndex:MEH:MEL0:MEL1"
str_p4:
.asciz	").\n"

.section .exp_table
/* Vector table space. */

.align 10
ckcpu_vsr_table:
	.long	__start
.align 2
.rept 255
	.long	default_exception_handler
.endr

.macro	INIT_ALL_REG
//initialize all registers
	movi r0, 0
	movi r1, 0
	movi r2, 0
	movi r3, 0
	movi r4, 0
	movi r5, 0
	movi r6, 0
	movi r7, 0
	movi r8, 0
	movi r9, 0
	movi r10, 0
	movi r11, 0
	movi r12, 0
	movi r13, 0
	movi r14, 0
	movi r15, 0
	movi r16, 0
	movi r17, 0
	movi r18, 0
	movi r19, 0
	movi r20, 0
	movi r21, 0
	movi r22, 0
	movi r23, 0
	movi r24, 0
	movi r25, 0
	movi r26, 0
	movi r27, 0
	movi r28, 0
	movi r29, 0
	movi r30, 0
	movi r31, 0

    //initialize all control register
	bgeni   a0, 31
	mtcr    a0, cr<0,0>
	movi    a0, 0
	mtcr    a0, cr<1,0>
	mtcr    a0, cr<11,0>
	mtcr    a0, cr<17,0>
	mtcr    a0, cr<18,0>
	mtcr    a0, cr<22,0>
	mtcr    a0, cr<23,0>
	mtcr    a0, cr<24,0>
	mtcr    a0, cr<31,0>
	mtcr    a0, cr<0,15>
	mtcr    a0, cr<2,15>
	mtcr    a0, cr<3,15>
	mtcr    a0, cr<4,15>
	mtcr    a0, cr<6,15>
	mtcr    a0, cr<8,15>
	mtcr    a0, cr<29,15>
	mtcr    a0, cr<30,15>
	mtcr    a0, cr<31,15>
.endm

.macro	CONSTRUCT_TLB
#if defined(CONFIG_FPGA_BOARD)
 	/* Contruct sTLB Entry for periphral */
    lrw     r6, 0x18000000  //base addr
    lrw		r7, 0xfff    	//page mask
    lrw     r8, 1           //total entry
    lrw     r9, 25          //virtual addr bit
    lrw     r10, 25         //phsical addr bit
    lrw     r11, 520        //base entry
    lrw     r12, 0x16       //flags
    bsr     __fill_tlb

 	/* Contruct sTLB Entry for pcie interrupt */
    lrw     r6, 0x0c000000  //base addr
    lrw		r7, 0xfff    	//page mask
    lrw     r8, 1           //total entry
    lrw     r9, 25          //virtual addr bit
    lrw     r10, 25         //phsical addr bit
    lrw     r11, 522        //base entry
    lrw     r12, 0x16       //flags
    bsr     __fill_tlb

 	/* Contruct sTLB Entry for pcie interrupt */
    //lrw     r6, 0x0e000000  //base addr
    lrw     r6, 0x1a000000  //base addr
    lrw		r7, 0xfff    	//page mask
    lrw     r8, 1           //total entry
    lrw     r9, 25          //virtual addr bit
    lrw     r10, 25         //phsical addr bit
    lrw     r11, 521        //base entry
    lrw     r12, 0x16       //flags
    bsr     __fill_tlb

#if defined (CONFIG_CKCPU_DEBUG_MEM)
 	/* Contruct sTLB Entry for pcie interrupt */
    lrw     r6, 0x02000000  //base addr
    lrw		r7, 0xfff    	//page mask
    lrw     r8, 1           //total entry
    lrw     r9, 25          //virtual addr bit
    lrw     r10, 25         //phsical addr bit
    lrw     r11, 523        //base entry
    lrw     r12, 0x16       //flags
    bsr     __fill_tlb
#endif

 	/* Contruct jTLB Entry for Memory Pool */
    lrw     r6, 0           //base addr
    lrw		r7, 0xf			//page mask
    lrw     r8, 32          //total entry
    lrw     r9, 17          //virtual addr bit
    lrw     r10, 19         //phsical addr bit
    lrw     r11, 0          //base entry
    lrw     r12, 0x1e       //flags
    bsr     __fill_tlb

    /* config default seg 0&1 */
	lrw     a0, CONFIG_SSEG0_BASE
	movi    a1, 0x6                // C = 1, D = 1, V = 1
	or      a0, a1
	mtcr    a0, cr<30,15>
	lrw     a0, CONFIG_SSEG1_BASE
	movi    a1, 0x6                // D = 1, V = 1
	or      a0, a1
	mtcr    a0, cr<31,15>

#elif defined(CONFIG_ASIC_BOARD)
 	/* Contruct jTLB Entry 0 (cacheable) */
    lrw     r6, 0x0         //base addr
    lrw		r7, 0xfff    	//page mask
    lrw     r8, 1           //total entry
    lrw     r9, 25          //virtual addr bit
    lrw     r10, 25         //phsical addr bit
    lrw     r11, 0          //base entry
    lrw     r12, 0x1e       //flags
    bsr     __fill_tlb

 	/* Contruct jTLB Entry 1 (uncacheable) */
    lrw     r6, 0x2000000   //base addr
    lrw		r7, 0xfff    	//page mask
    lrw     r8, 127         //total entry
//    lrw     r8, 1           //total entry
    lrw     r9, 25          //virtual addr bit
    lrw     r10, 25         //phsical addr bit
    lrw     r11, 1          //base entry
    lrw     r12, 0x16       //flags
    bsr     __fill_tlb

 	/* Recontruct jTLB Entry 2 (cacheable) */
//    lrw     r6, 0x4000000   //base addr
//    lrw     r7, 0xfff    	  //page mask
//    lrw     r8, 1           //total entry
//    lrw     r9, 25          //virtual addr bit
//    lrw     r10, 25         //phsical addr bit
//    lrw     r11, 2          //base entry
//    lrw     r12, 0x1e       //flags
//    bsr     __fill_tlb

 	/* Recontruct jTLB Entry 3~7 (uncacheable) */
//    lrw     r6, 0x6000000   //base addr
//    lrw     r7, 0xfff       //page mask
//    lrw     r8, 5           //total entry
//    lrw     r9, 25          //virtual addr bit
//    lrw     r10, 25         //phsical addr bit
//    lrw     r11, 3          //base entry
//    lrw     r12, 0x16       //flags
//    bsr     __fill_tlb

 	/* Recontruct jTLB Entry 8~11 (cacheable) */
//    lrw     r6, 0x10000000  //base addr
//    lrw     r7, 0xfff    	  //page mask
//    lrw     r8, 4           //total entry
//    lrw     r9, 25          //virtual addr bit
//    lrw     r10, 25         //phsical addr bit
//    lrw     r11, 8          //base entry
//    lrw     r12, 0x1e       //flags
//    bsr     __fill_tlb

 	/* Recontruct jTLB Entry 12~127 (uncacheable) */
//    lrw     r6, 0x18000000  //base addr
//    lrw     r7, 0xfff    	  //page mask
//    lrw     r8, 116         //total entry
//    lrw     r9, 25          //virtual addr bit
//    lrw     r10, 25         //phsical addr bit
//    lrw     r11, 12         //base entry
//    lrw     r12, 0x16       //flags
//    bsr     __fill_tlb
#endif
.endm

/* The ckcpu startup codes. */
.section .boot
	/*
	 * This is the codes first entry point. This is where it all
	 * begins...
	 */

__start:
	/*
	 * Init psr value, enable exception, disable interrupt and fast interrupt.
	 * psr = 0x80000100
	 */
	bgeni	a3, 31
	bseti	a3, 8
	mtcr	a3, psr

    /* invalid BHT & BTB */
	mfcr	a3, cr17
	bseti   a3, 16
	bseti   a3, 17
	mtcr    a3, cr17
	/* Check if cold or warm boot */
	lrw	    a0, 0x12345678
	lrw	    a1, reset_status
//    bseti   a1, 31
//    mfcr    r5, cr<18,0>
//    ori     r5, r7, 0x3
//    cmpnei  r5, 0
//    bt      __after_mmu_check
//    bclri   a1, 31
//__after_mmu_check:
	ldw     a2, (a1)
	cmpne   a0, a2
    bt      common_reset
//	bf      warm_reset
//cold_reset:
//    /* Set to Warm Reset */
//	stw     a0, (a1)
//    br      common_reset

warm_reset:
    /* restore context of exception */
//    rte
    /* clear instruction Queue */
	.rept   8
	mov     sp, sp
	.endr
    /* software reset registers */
    INIT_ALL_REG
    /* invalid BHT & BTB */
	mfcr	a3, cr17
	bseti   a3, 16
	bseti   a3, 17
	mtcr    a3, cr17
    /* check exception count */
	lrw	    a0, exception_status_addr
//    bseti   a0, 31
//    mfcr    r5, cr<18,0>
//    ori     r5, r7, 0x3
//    cmpnei  r5, 0
//    bt      __after_mmu_check1
//    bclri   a0, 31
//__after_mmu_check1:
    movi    a1, 0
1:
	ldr.b   a2, (a0, a1<<0)
    btsti   a2, 7
    bf      2f
    andi    a2, 0xf
    cmphsi  a2, 2
    bt      default_exception_handler
2:
    addi    a1, 1
    cmphsi  a1, 8
    bf      1b

common_reset:
    /* Initial CKCPU MMU & Cache */
#if defined(CONFIG_CKCPU_MMU)
	/*
	 * Step 0: Setup the initial page tables.
 	 */
__create_page_table:
	/*
	 *  Disable  CPU MMU.
	 */
	mfcr	a3, cr18
	bclri	a3, 0		// bit 0~1, mmu enable
	bclri	a3, 1
	mtcr	a3,	cr18

	/*
	 * invalid the jTLB
 	 */
	movi r1, 0
	mtcr    r1, cr<2, 15>
	mtcr    r1, cr<3, 15>
	mtcr    r1, cr<6, 15>
	bgeni	a0,	26
	mtcr	a0,	cr<8,15>

	/*
	 * Construct TLB
 	 */
	CONSTRUCT_TLB

	/*
	 * Enable predict for jump, return stack, and cache write back.
	 */
	mfcr    a3, cr18
	bseti   a3, 11                 /* BTB */
	bseti   a3, 6                  /* branch prediction */
	bseti   a3, 5                  /* forecast jump  for ck610  */
	bseti   a3, 4                  /* write back for ck610  */
	mtcr    a3, cr18

	/*
	 *  Enable  CPU MMU.
	 */
	mfcr    a3, cr18
	bseti   a3, 0
	bclri   a3, 1
	mtcr    a3, cr18

    lrw     r8, 128         //max index
    lrw     r9, 0           //start enrty
    bsr     __tlb_print

    lrw     r8, 4           //max index
    lrw     r9, 520         //start entry
//    bsr     __tlb_print

	/*
	 *	Enable internal cache.
	 */
#ifdef CONFIG_CSKY_INSTRUCTION_CACHE
	//Invalid instruction cache
	movi    a3, 0x11
	mtcr    a3, cr17

	//Enable instruction cache
	mfcr    a3, cr18
	bseti   a3, 2
	mtcr    a3, cr18
#endif

#ifdef CONFIG_CSKY_DATA_CACHE
	//Invalid Data cache
	movi    a3, 0x12
	mtcr    a3, cr17

    //Enable data cache
	mfcr    a3, cr18
	bseti   a3, 3
	mtcr    a3, cr18
#endif

#ifdef CONFIG_CSKY_L2_CACHE
	// Invalid L2 cache
	movi    a3, 0x10
	mtcr    a3, cr24

	// Enable data cache
	mfcr    a3, cr23
	bseti   a3, 3
	// write back L2
	bseti   a3, 4
	// write alloc
	bseti   a3, 12
	mtcr    a3, cr23
#endif

#else /* CONFIG_CKCPU_MMU */
    /* disable MMU */
	mfcr	a3, cr18
	bclri	a3, 0		// bit 0~1, mmu enable
	bclri	a3, 1		//
	bseti	a3,	4		// write back
	bseti	a3,	5		// return stack
	bseti	a3,	6		// jumping predict
	mtcr	a3,	cr18

	/*
	 * Step 3: enable instruction & data cache
	 */

	/* Invalid Instruction cache content */
	movi	a3, 0x11
	mtcr	a3, cr17

	/* disable instruction cache */
	mfcr	a2, cr18
	bclri	a2, 2
	mtcr	a2, cr18

	/* Invalid Data cache content */
	movi	a3, 0x12
	mtcr	a3, cr17

	/* disable Data cache */
	mfcr	a3, cr18
	bclri	a3, 3
	mtcr	a3, cr18

    /* disable L2	 cache */
	mfcr	a3, cr23
	bclri	a3, 3
	mtcr	a3, cr23
#endif /* CONFIG_CKCPU_MMU */

	/*
	 * Setup initial vector base table for interrupts and exceptions
	 */
	lrw     a2, ckcpu_vsr_table     /* Load pre-defined vbr */
	mtcr    a2, vbr                 /* Set vbr register with physical address */

	/*
	 * Zero out the bss region.
	 * NOTE: __sbss must align 4
	 */
	lrw		a3, __sbss	/* Get start of bss from linking script file */
	lrw		a2, __ebss	/* Get end of bss from linking script file */
	subu	a2, a3		/* Calculate size of bss */
	lsri	a2, 2		/* Size of whole words */
	movi	a1, 0		/* Set zero value to write */
2:
	stw		a1, (a3)	/* Zero next word */
	addi	a3, 4		/* Increase bss pointer */
	decne	a2			/* Decrease counter */
	bt		2b			/* Repeat for all bss */

	/* Initialize the normal stack pointer from the linker definition. */
	lrw		a3, __stack
	mov		sp, a3

	/*
	 * Assember start up done, C codes start here.
	 */
__goto_c:
    /* init for C++ code */
//    bsr     _init
//    lsli    r0, r0, 0
//    bsr    CK_Bsp_Init
//    lsli    r0, r0, 0
	bsr		main

//	bsr     _fini
//    lsli    r0, r0, 0

	/* Should never get here. */
__exit:
	bkpt

/* useful subroutine */
__fill_tlb:
    mov     r13, r15
	lrw     r4, 0           //start index
1001:
    lsl     a3, r4, r9
    addu    a3, r6
	mtcr  	a3,	cr<4,15>	// MEH

	/* page size */
	lsli    a2, r7, 13
	mtcr	a2,	cr<6,15>	// MPR

	/* MEL0 */
    lsl     r5, r4, r10
    addu    r5, r6
	mov     a1, r12
	or      a1, r5			// set pfn
	mtcr	a1,	cr<2,15>	// MEL0

	/* MEL1 */
	subi	a2, r9, 1
	movi	a3, 0x1
    lsl     a3, a2
    addu    r5, a3
	mov     a0, r12
	or      a0, r5			// set pfn
	mtcr	a0,	cr<3,15>	// MEL1

    /* write tlb */
    /* prepare TLB index */
    addu    a2, r4, r11
    mtcr    a2, cr<0,15>

	/* index write */
	bgeni	a0, 29
	mtcr  	a0,	cr<8,15>	//MCIR

    /* increment index */
    addi    r4, 1
    cmphs   r4, r8
    bf      1001b
    jmp     r13

/*
 * Exception handling.  Something went wrong and we can't proceed.  We
 * ought to tell the user, but since we don't have any guarantee that
 * we're even running on the right architecture, we do virtually nothing.
 *
 * If CONFIG_DEBUG_LL is set we try to print out something about the error
 * and hope for the best (useful if bootloader fails to pass a proper
 * machine ID for example).
 */
__error_p:
    mov     r11, r15
//#ifdef CONFIG_DEBUG_LL
	lrw	    r0, str_p1
//	bsr	    printascii
	mov	    r0, r9
//	bsr     printhex8
	lrw	    r0, str_p2
//	bsr	    printascii
	.rept   8
	mov     sp, sp
	.endr
//#endif
//    rts
    jmp     r11

__tlb_print:
    mov     r11, r15
//#ifdef CONFIG_DEBUG_LL
	lrw     r4, 0           //start index
1002:
	lrw	    r0, str_p3
//	bsr	    printascii

    /* prepare TLB index */
    addu    a2, r4, r9
    mtcr    a2, cr<0,15>

	/* index read */
	bgeni	a0, 30
	mtcr  	a0,	cr<8,15>	//MCIR

    /* print MEH MEl0 MEL1 */
	mfcr	r0, cr<4,15>
	lsri	r0, 11
//	bsr 	printhex8
	mfcr	r0, cr<2,15>
//	bsr 	printhex8
	mfcr	r0, cr<3,15>
//	bsr 	printhex8

    addi    r4, 1
    cmphs   r4, r8
    bf      1002b

	lrw	    r0, str_p4
//	bsr	    printascii
	.rept   8
	mov     sp, sp
	.endr
//#endif
//    rts
    jmp     r11

