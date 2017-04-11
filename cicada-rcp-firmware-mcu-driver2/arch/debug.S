/*
 * linux/arch/unicore32/kernel/debug-macro.S
 *
 * Code specific to PKUnity SoC and UniCore ISA
 *
 * Copyright (C) 2001-2010 GUAN Xue-tao
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Debugging macro include header
 */
#include "ck810.h"

/* UART registers addr definition */
#define CK_UART_RBR       0x00    /* Receive Buffer Register (32 bits, R) */
#define CK_UART_THR       0x00    /* Transmit Holding Register (32 bits, W) */
#define CK_UART_DLL       0x00    /* Divisor Latch(Low)  (32 bits, R/W) */
#define CK_UART_IER       0x01    /* Interrupt Enable Register (32 bits, R/W) */
#define CK_UART_DLH       0x01    /* Divisor Latch(High) (32 bits, R/W) */
#define CK_UART_IIR       0x02    /* Interrupt Identity Register (32 bits, R) */
#define CK_UART_FCR       0x02    /* fifo Countrol Register (32 bits, W) */
#define CK_UART_LCR       0x03    /* Line Control Register (32 bits, R/W) */
#define CK_UART_MCR       0x04    /* Modem Control Register (32 bits, W) */
#define CK_UART_LSR       0x05    /* Line Status Register (32 bits, R) */
#define CK_UART_MSR       0x06    /* Modem Status Register (32 bits, R/W) */
#define CK_UART_USR       0x1f    /* UART Status Register (32 bits, R/W) */

#define CK_LSR_PFE              0x80
#define CK_LSR_TEMT             0x40
#define CK_LSR_THRE             0x20
#define	CK_LSR_BI               0x10
#define	CK_LSR_FE               0x08
#define	CK_LSR_PE               0x04
#define	CK_LSR_OE               0x02
#define	CK_LSR_DR               0x01
#define CK_LSR_TRANS_EMPTY      0x20

.export printhex8
.export printhex4
.export printhex2
.export printhex
.export printascii
.export printch

.data
hexbuf:
.space 16

//		.macro	put_word_ocd, rd, rx=r16
//1001:		movc		\rx, p1.c0, #0
//		cand.a	\rx, #2
//		bne	1001b
//		movc		p1.c1, \rd, #1
//		.endm

//#ifdef CONFIG_DEBUG_OCD
//		/* debug using UniCore On-Chip-Debugger */
//		.macro	addruart, rx
//		.endm
//
//		.macro	senduart, rd, rx
//		put_word_ocd	\rd, \rx
//		.endm
//
//		.macro	busyuart, rd, rx
//		.endm
//
//		.macro	waituart, rd, rx
//		.endm
//#else
/* #define UART_CLK_DEFAULT        3686400 * 20 */
#define UART_CLK_DEFAULT        25000000
	/* Uartclk = MCLK/ 2, The MCLK on my board is 3686400 * 40  */
#define BAUD_RATE_DEFAULT	    115200
	/* The baud rate of the serial port */

#define UART_DIVISOR_DEFAULT	(UART_CLK_DEFAULT \
				/ (16 * BAUD_RATE_DEFAULT) - 1)

		.macro	addruart, rx
//		mrc	p0, #0, \rx, c1, c0
//		tst	\rx, #1			@ MMU enabled?
//		moveq	\rx, #0xee000000	@ physical base address
//		movne	\rx, #0x6e000000	@ virtual address
        lrw     r2, 0xb9020000
//        lrw     r2, 0x19020000
//        mfcr    r5, cr<18,0>
//        movi    r7, 0x3
//        or      r5, r7
//        cmpnei  r5, 0
//        bt      2001f
//        lrw     r7, PERI_BASE
//        sub     r2, r7
//2001:
        mov     \rx, r2
		movi 	r5, 0

		movi 	r2, UART_DIVISOR_DEFAULT
		bseti 	r5, 7
		mov 	r1, r5
		stw	    r1, (\rx, CK_UART_LCR*4)
		movi    r6, 0xff00
		and     r1, r2, r6
		lsri	r1, 8
		stw	    r1, (\rx, CK_UART_DLH*4)
		andi 	r1, r2, 0xff
		stw	    r1, (\rx, CK_UART_DLL*4)
		bclri 	r5, 7
		mov  	r1, r5
		stw	    r1, (\rx, CK_UART_LCR*4)
        /* pariry */
		bclri 	r5, 3
        /* stop */
		bclri 	r5, 2
        /* DLS */
		bseti 	r5, 1
		bseti 	r5, 0
		mov  	r1, r5
		stw	    r1, (\rx, CK_UART_LCR*4)

		movi 	r1, 0x7
		stw	    r1, (\rx, CK_UART_FCR*4)
		movi 	r1, 0x0
		stw	    r1, (\rx, CK_UART_IER*4)
		.endm

		.macro	senduart,rd,rx
		stw	    \rd, (\rx, CK_UART_THR*4)
		.endm

		.macro	waituart,rd,rx
1001:
        ldw	    \rd, (\rx, CK_UART_LSR*4)
        btsti   \rd, 5/* THRE */
		bf	    1001b
		.endm

		.macro	busyuart,rd,rx
1001:
        ldw     \rd, (\rx, CK_UART_LSR*4)
        btsti   \rd, 6/* TEMT */
		bt	    1001b
		.endm

/*
 * linux/arch/unicore32/kernel/debug.S
 *
 * Code specific to PKUnity SoC and UniCore ISA
 *
 * Copyright (C) 2001-2010 GUAN Xue-tao
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  32-bit debugging code
 */

/*
 * Some debugging routines (useful if you've got MM problems and
 * printk isn't working).  For DEBUGGING ONLY!!!  Do not leave
 * references to these in a production kernel!
 */

/*
 * Useful debugging routines
 */
    .text
	.balign 4
printhex8:
		movi	r1, 8
		br	    printhex
printhex4:
		movi	r1, 4
		br	    printhex
printhex2:
		movi	r1, 2
printhex:
        lrw	    r2, hexbuf
		addu	r3, r2, r1
		movi	r1, 0
		st.b	r1, (r3)
1:
        andi	r1, r0, 15
		lsri    r0, 4
		cmplti	r1, 10
		bf      2f
		addi    r1, '0'
		bf      3f
2:
        addi    r1, 'a' - 10
3:
		subi	r3, 1
		st.b r1, (r3)
//		addi	r3, 1
		cmpne	r3, r2
		bt	    1b
		mov	    r0, r2
		br	    printascii

//		.ltorg
    .text
	.balign 4

printascii:
		addruart r3
		br	2f
1:
        waituart    r2, r3
		senduart    r1, r3
		busyuart    r2, r3
		cmpnei	    r1, '\n'
        bt          2f
		movi	    r1, '\r'
		br	        1b
2:
        cmpnei	    r0, 0
		bf	        3f
		ld.b        r1, (r0, 0x0)
        addi        r0, 1
		cmpnei	    r1, 0
		bt	        1b
3:
       rts
//        mfcr	    pc, lr

    .text
	.balign 4
printch:
		addruart r3
		mov	r1, r0
		movi	r0, 0
		br	1b

