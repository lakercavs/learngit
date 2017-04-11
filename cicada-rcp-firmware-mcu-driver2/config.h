/*
 * file: config.h
 *
 * Some CPU configure infomation:
 * Data type, Baudrate for UART;
 * MGU, CACHE, SPM, System Frequency.
 *  
 * Copyright (C) 2008  Hangzhou C-SKY Microsystems Co., Ltd
 */

#ifndef __DEMO_CONFIG_H
#define __DEMO_CONFIG_H

#define CONFIG_FPGA_BOARD
//#define CONFIG_ASIC_BOARD

/* FPGA BOARD */
#if defined(CONFIG_FPGA_BOARD)
/*******************************
 * Config System
 *******************************/
/*
 *  Run on SPARK ?
 *  0 -- run on board, 1 -- run on spark.
 */
/* PLL input ckock(crystal frequency) */
#define CONFIG_PLL_INPUT_CLK    25000000    /* HZ */
/* CPU frequence definition */
#define CPU_DEFAULT_FREQ        25000000    /* Hz */
/* AHB frequence definition */
#define AHB_DEFAULT_FREQ        25000000    /* Hz */
/* APB frequence definition */
#define APB_DEFAULT_FREQ        25000000    /* Hz */
/* UART frequence definition */
#define UART_DEFAULT_FREQ       25000000    /* Hz */

/*******************************
 * Config CPU cache
 ******************************/
/* 0 - rw; 1 - rwc; 2 - rwc; 3 - rw */
#define CONFIG_CKCPU_MGU_BLOCKS         0xff06

/* 0 - baseaddr: 0x0; size: 4G */
#define CONFIG_CKCPU_MGU_REGION1        0x3f
/* 1- baseaddr: 0x8000000; size: 8M */
#define CONFIG_CKCPU_MGU_REGION2        0x800002f
/* 2- baseaddr: 0x8600000; size: 256K for MAC */
#define CONFIG_CKCPU_MGU_REGION3        0x2f
/* 3- Disable */
#undef CONFIG_CKCPU_MGU_REGION4

#define CONFIG_CKCPU_MMU
//#define ONFIG_CPU_HAS_FPU
#define CONFIG_SOFT_HANDMISSALIGN
//#define CONFIG_CSKY_INSTRUCTION_CACHE
//#define CONFIG_CSKY_DATA_CACHE
//#define CONFIG_CSKY_L2_CACHE

/*******************************
 * Config PERI BASE ADDRESS
 ******************************/
#if defined(CONFIG_CKCPU_MMU)
#define CONFIG_CKCPU_DEBUG_MEM
#define CONFIG_SSEG0_BASE 0x80000000
#define CONFIG_SSEG1_BASE 0xa0000000
#define PERI_BASE 0x0
#else
#define PERI_BASE 0x0
#endif

/* ASIC BOARD */
#elif defined(CONFIG_ASIC_BOARD)
/*******************************
 * Config System
 *******************************/
/*
 *  Run on SPARK ?
 *  0 -- run on board, 1 -- run on spark.
 */
/* PLL input ckock(crystal frequency) */
#define CONFIG_PLL_INPUT_CLK    1000000000  /* HZ */
/* CPU frequence definition */
#define CPU_DEFAULT_FREQ        1000000000  /* Hz */
/* AHB frequence definition */
#define AHB_DEFAULT_FREQ        500000000   /* Hz */
/* APB frequence definition */
#define APB_DEFAULT_FREQ        500000000   /* Hz */
/* UART frequence definition */
#define UART_DEFAULT_FREQ       250000000   /* Hz */

/*******************************
 * Config CPU cache
 ******************************/
/* 0 - rw; 1 - rwc; 2 - rwc; 3 - rw */
#define CONFIG_CKCPU_MGU_BLOCKS         0xff06

/* 0 - baseaddr: 0x0; size: 4G */
#define CONFIG_CKCPU_MGU_REGION1        0x3f
/* 1- baseaddr: 0x8000000; size: 8M */
#define CONFIG_CKCPU_MGU_REGION2        0x800002f
/* 2- baseaddr: 0x8600000; size: 256K for MAC */
#define CONFIG_CKCPU_MGU_REGION3        0x2f
/* 3- Disable */
#undef CONFIG_CKCPU_MGU_REGION4

#define CONFIG_CKCPU_MMU
//#define ONFIG_CPU_HAS_FPU
#define CONFIG_SOFT_HANDMISSALIGN
#define CONFIG_CSKY_INSTRUCTION_CACHE
#define CONFIG_CSKY_DATA_CACHE
#define CONFIG_CSKY_L2_CACHE

/*******************************
 * Config PERI BASE ADDRESS
 ******************************/
#if defined(CONFIG_CKCPU_MMU)
#define PERI_BASE 0x0
#else
#define PERI_BASE 0x0
#endif

#endif /* CONFIG_ASIC_BOARD */

#endif /* __DEMO_CONFIG_H */
