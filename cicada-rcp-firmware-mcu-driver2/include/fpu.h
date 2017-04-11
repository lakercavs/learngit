/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2009  Hangzhou C-SKY Microsystems co.,ltd.
 */

#ifndef __CSKY_FPU_H
#define __CSKY_FPU_H

/*
 * Define the fesr bit for fpe handle. 
 */
#define  FPE_ILLE  (1 << 16)    /* Illegal instruction  */
#define  FPE_FEC   (1 << 7)     /* Input float-point arithmetic exception */
#define  FPE_IDC   (1 << 5)     /* Input denormalized exception */
#define  FPE_IXC   (1 << 4)     /* Inexact exception */
#define  FPE_UFC   (1 << 3)     /* Underflow exception */
#define  FPE_OFC   (1 << 2)     /* Overflow exception */
#define  FPE_DZC   (1 << 1)     /* Divide by zero exception */
#define  FPE_IOC   (1 << 0)     /* Invalid operation exception */

#ifdef CONFIG_OPEN_FPU_IDE
#define IDE_STAT   (1 << 5)
#else 
#define IDE_STAT   0
#endif

#ifdef CONFIG_OPEN_FPU_IXE
#define IXE_STAT   (1 << 4)
#else 
#define IXE_STAT   0
#endif

#ifdef CONFIG_OPEN_FPU_UFE
#define UFE_STAT   (1 << 3)
#else
#define UFE_STAT   0
#endif

#ifdef CONFIG_OPEN_FPU_OFE
#define OFE_STAT   (1 << 2)
#else
#define OFE_STAT   0
#endif

#ifdef CONFIG_OPEN_FPU_DZE
#define DZE_STAT   (1 << 1)
#else
#define DZE_STAT   0
#endif

#ifdef CONFIG_OPEN_FPU_IOE
#define IOE_STAT   (1 << 0)
#else
#define IOE_STAT   0
#endif

#define FMFS_FPU_REGS(frw, frx, fry, frz)       \
	"fmfs   %0, "#frw" \n\r"        \
	"fmfs   %1, "#frx" \n\r"        \
	"fmfs   %2, "#fry" \n\r"        \
	"fmfs   %3, "#frz" \n\r"

#define FMTS_FPU_REGS(frw, frx, fry, frz)       \
	"fmts   %0, "#frw" \n\r"        \
	"fmts   %1, "#frx" \n\r"        \
	"fmts   %2, "#fry" \n\r"        \
	"fmts   %3, "#frz" \n\r"

#define FMFVR_FPU_REGS(vrx, vry)        \
	"fmfvrl %0, "#vrx" \n\r"        \
	"fmfvrh %1, "#vrx" \n\r"        \
	"fmfvrl %2, "#vry" \n\r"        \
	"fmfvrh %3, "#vry" \n\r"

#define FMTVR_FPU_REGS(vrx, vry)        \
	"fmtvrl "#vrx", %0 \n\r"        \
	"fmtvrh "#vrx", %1 \n\r"        \
	"fmtvrl "#vry", %2 \n\r"        \
	"fmtvrh "#vry", %3 \n\r"

#define STW_FPU_REGS(a, b, c, d)        \
	"stw    %0, (%4, "#a") \n\r"    \
	"stw    %1, (%4, "#b") \n\r"    \
	"stw    %2, (%4, "#c") \n\r"    \
	"stw    %3, (%4, "#d") \n\r"

#define LDW_FPU_REGS(a, b, c, d)        \
	"ldw    %0, (%4, "#a") \n\r"    \
	"ldw    %1, (%4, "#b") \n\r"    \
	"ldw    %2, (%4, "#c") \n\r"    \
	"ldw    %3, (%4, "#d") \n\r"

/* enable and init FPU */
inline void init_fpu(void)
{
	unsigned long flg;
	unsigned long cpwr, fcr;

	cpwr = 0xf0000007; // set for reg CPWR(cp15): ie, ic, ec, rp, wp, en = 1 
	fcr = (IDE_STAT | IXE_STAT | UFE_STAT | OFE_STAT | DZE_STAT | IOE_STAT);
	__asm__ __volatile__("mfcr    %0, psr	 \n"
                 :"=r"(flg) : :"memory"
                );

	__asm__ __volatile__("mtcr    %0, cr<1, 2> \n\t"
			     ::"r"(fcr)
			    );

	__asm__ __volatile__("mtcr    %0, psr  \n"
                 ::"r" (flg) :"memory"
                );
}

#endif /* __CSKY_FPU_H */
