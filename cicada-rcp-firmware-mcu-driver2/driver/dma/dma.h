#ifndef __DMA_H__
#define __DMA_H__
#include "io.h"
#include "ck810.h"
#include "datatype.h"
#include "intc.h"

#define DMA_ID  		0xFF			/* should be modified */
#define DMA_VERSION  	0x3130302A			/* should be modified */
//Bit8u channelid=0;

#define DW_DMA_MAX_NR_CHANNELS 8
#define DW_DMA_LLI_LENGTH  0x06

/*
 * Error code for DMA module. It should used for return value of function.
 */
typedef enum {
    DMA_OK = 0, 
    DMA_ERROR = -1, 
    DMA_NO_FREE_CHANNEL = -2, 
    DMA_INTERRUPT_SOURCE_ERROR = -3,
}dma_status_t;

/*Common Register definition */

/* 0x10 DMAC Configuration Register */
#define DMA_CFG_DMAC_EN (1 << 0)		/* Global Interrupt Enable bit */
#define DMA_CFG_INT_EN 	(1 << 1)		/* DMAC Enable bit */	

/* 0x18 DMAC Channel Enable Register Low */
#define DMA_CH_EN		(1 << 0)		/* Channel Enable bit*/
#define DMA_CH_EN_WE	(1 << 8)		/* Channel Enable Writ Enable bit*/
#define DMA_CH_SUSP		(1 << 16)		/* Channel Suspend Request bit*/
#define DMA_CH_SUSP_WE	(1 << 24)		/* Channel Suspend Request Enalbe bit*/
/* 0x18 DMAC Channel Enable Register High */
#define DMA_CH_ABORT	(1 << 0)		/* Channel Abort Request bit*/
#define DMA_CH_ABORT_WE	(1 << 8)		/* Channel Abort Request Write Enable bit*/

/* 0x30 DMAC Interrupt Status Register */
#define DMA_COMREG_INT_STAT	0x10000			/* CommonReg Interrupt Status Bit */
#define DMA_CH8_INT_STAT	0x80			/* Channel 8 Interrupt Status Bit */
#define DMA_CH7_INT_STAT	0x40			/* Channel 7 Interrupt Status Bit */
#define DMA_CH6_INT_STAT	0x20			/* Channel 6 Interrupt Status Bit */
#define DMA_CH5_INT_STAT	0x10			/* Channel 5 Interrupt Status Bit */
#define DMA_CH4_INT_STAT	0x08			/* Channel 4 Interrupt Status Bit */
#define DMA_CH3_INT_STAT	0x04			/* Channel 3 Interrupt Status Bit */
#define DMA_CH2_INT_STAT	0x02			/* Channel 2 Interrupt Status Bit */
#define DMA_CH1_INT_STAT	0x01			/* Channel 1 Interrupt Status Bit */

/* 0x38 DMAC Common Register Interrupt Clear Register */
/* 0x40 DMAC Common Register Interrupt Status Enable Register */
/* 0x40 DMAC Common Register Interrupt Signal Enable Register */
/* 0x50 DMAC Commom Register Interrupt status Register */
#define DMA_COM_INT_DEC_ERR (1 << 0)      /* Slave Interface Common Register Decode Error Bit */
#define DMA_COM_INT_WR2RO_ERR (1 << 1)    /* Slave Interface Common Register Write to Read only Error Bit */
#define DMA_COM_INT_RD2WO_ERR (1 << 2)    /* Slave Interface Common Register Read to Write only Error Bit */
#define DMA_COM_INT_WRONHOLD_ERR (1 << 3) /* Slave Interface Common Register Write On Hold Error Bit */
#define DMA_COM_INT_UN_DEC_ERR (1 << 8)   /* Slave Interface Undefined Register Decode Error Bit */
#define DMA_COM_INT_ALL        (0xFF)     /* All commonreg IT */

/*
 * Redefine this macro to handle differences between 32- and 64-bit
 * addressing, little endian.
 **/
#define DW_REG(name) Bit32u name; Bit32u name##_hi

/* Hardware register definitions. */

struct dw_dma_common_reg{
    DW_REG(ID);        		/* 0x00 DMAC ID Register */
    DW_REG(CompVer);   		/* 0x08 DMAC Component Version Register */
    Bit32u CFG;   			/* 0x10 DMAC Configuration Register */
	Bit32u Reserved0;
    Bit32u ChEn_LO;   		/* 0x18 DMAC Channel Enable Register Low */
    Bit32u ChEn_HI;   		/* 0x18 DMAC Channel Enable Register High */
	DW_REG(Reserved1);		/* 0x20 Reserved */
	DW_REG(Reserved2);		/* 0x28 Reserved */
    DW_REG(IntStat);   		/* 0x30 DMAC Interrupt Status Register */
    DW_REG(ComIntClr);   	/* 0x38 DMAC Common Register Interrupt Clear Register */
    DW_REG(ComIntStatEn);  	/* 0x40 DMAC Common Register Interrupt status Enable Register */
    DW_REG(ComIntSigEn);   	/* 0x48 DMAC Common Register Interrupt Signal Enable Register */
    DW_REG(ComIntStat); 	/* 0x50 DMAC Commom Register Interrupt status Register */
    DW_REG(Reset);  		/* 0x58 DMAC Reset Register */
};

/* DMA Link List related register */
#define DMA_MAX_PER_TRF_SIZE 32*1024		/* Data width:32, Burst size:1024 */

/* DMA CHx Control register Low */
#define DMA_CHX_CTL_SMS(x)			((x) << 0)	/* Source Master Slect */
#define DMA_CHX_CTL_DMS(x)			((x) << 2)	/* Destination Master Select */
#define DMA_CHX_CTL_SING(x)			((x) << 4)	/* Source Address Increment */
#define DMA_CHX_CTL_DING(x)			((x) << 6)	/* Destination Address Increment */
#define DMA_CHX_CTL_SRC_TR_WIDTH(x)	((x) << 8)	/* Source Transfer Width */
#define DMA_CHX_CTL_DST_TR_WIDTH(x) ((x) << 11) /* Destination Transfer Width */
#define DMA_CHX_CTL_SRC_MSIZE(x)	((x) << 14) /* Source Burst Transaction Length */
#define DMA_CHX_CTL_DST_MSIZE(x)	((x) << 18)	/* Destination Burst Transaction Length */
/* Bit[25:22] axi 'ar_cache' signal */
/* Bit[29:26] axi 'aw_cache' signal */
#define DMA_CHX_CTL_NON_POSTED_LAST_WRITE_EN (1 << 30) /* Non Posted Last Write Enable */

/* DMA CHx Control register High */
/* Bit[34:32] axi 'ar_prot' signal */
/* Bit[37:35] axi 'aw_prot' signal */
#define HI(x)	(x-32)
#define DMA_CHX_CTL_ARLEN_EN	(1 << HI(38))		/* Source Burst Length Enable */
#define DMA_CHX_CTL_ARLEN(x)	((x) << HI(39))		/* Source Burst Length */
#define DMA_CHX_CTL_AWLEN_EN	(1 << HI(47))		/* Destination Burst Length Enable*/
#define DMA_CHX_CTL_AWLEN(x)	((x) << HI(48))		/* Destination Burst Legnth */
#define DMA_CHX_CTL_SRC_STAT_EN (1 << HI(56))		/* Source Status Enable */
#define DMA_CHX_CTL_DST_STAT_EN	(1 << HI(57))		/* Destination Status Enable */
#define DMA_CHX_CTL_IOC_BLK_TFR	(1 << HI(58))		/* Interrupt On completion of Block Transfer */
#define DMA_CHX_CTL_SHADOWREG_OR_LLI_LAST (1 << HI(62))	/* Last Shadow Register/Linked List Item */
#define DMA_CHX_CTL_SHADOWREG_OR_LLI_VALID (1 << HI(63))	/* Shadow Register/Linked List Item Valid */

/* DMA CHx Cofig register Low */
#define DMA_CHX_CFG_SRC_MULTBLK_TYPE(x)	((x) << 0)		/* Source Multi Block Transfer Type */
#define DMA_CHX_CFG_DST_MULTBLK_TYPE(x) ((x) << 2)      /* Destination Multi Block Transfer Type */

/* DMA CHx Cofig register High */
#define DMA_CHX_CFG_TT_FC(x)	((x) << HI(32)) 		/* Transfer Type and Flow Control */
#define DMA_CHX_CFG_HS_SEL_SRC	(1 << HI(35))			/* Source Software or Hardware Handshaking Select */
#define DMA_CHX_CFG_HS_SEL_DST  (1 << HI(36))           /* Destination Software or Hardware Handshaking Select */
#define DMA_CHX_CFG_SRC_HWHS_POL (1 << HI(37))			/* Source Hardware Handshaking Interface Polarity */
#define DMA_CHX_CFG_DST_HWHS_POL (1 << HI(38))			/* Destination Hardware Handshaking Interface Polarity */
#define DMA_CHX_CFG_SRC_PER(x)	((x) << HI(39))			/* Assigns a hardware handshaking interface ot the source of channel */
#define DMA_CHX_CFG_DST_PER(x)	((x) << HI(44))			/* Assigns a hardware handshaking interface ot the destination of channel */
#define DMA_CHX_CFG_CH_PRIOR(x)	((x) << HI(49))			/* Channel Priority */
#define DMA_CHX_CFG_LOCK_CH		(1 << HI(52))			/* Channel Lock bit */
#define DMA_CHX_CFG_LOCK_CH_L(x) ((x) << HI(53))		/* Channel Lock Level */
#define DMA_CHX_CFG_SRC_OSR_LMT(x) ((x) << HI(55))		/* Source Outstanding Request Limit */
#define DMA_CHX_CFG_DST_OSR_LMT(x) ((x) << HI(59))		/* Destination Outstanding Request Limit */

/* DMA CHx LLP Register */
#define DMA_CHX_LLP_LMS		(1 << 0)			/* LLI master Select */

/* Software Handshake Source Register */
#define DMA_CHX_SWHS_REQ_SRC		(1 << 0)		/* Softeware Handshake Request for Channel Source */
#define DMA_CHX_SWHS_REQ_SRC_WE		(1 << 1)		/* Softeware Handshake Request for Channel Source Write Enable */
#define DMA_CHX_SWHS_SIGREQ_SRC		(1 << 2)		/* Softeware Handshake Signal Request for Channel Source */
#define DMA_CHX_SWHS_SIGREQ_SRC_WE	(1 << 3)		/* Softeware Handshake Signla Request for Channel Source Write Enable */
#define DMA_CHX_SWHS_LAST_SRC		(1 << 4)		/* Softeware Handshake Last Request for Channel Source */
#define DMA_CHX_SWHS_LAST_SRC_WE	(1 << 5)		/* Softeware Handshake Last Request for Channel Source Write Enable */

/* Software Handshake Destination Register */
#define DMA_CHX_SWHD_REQ_DST		(1 << 0)		/* Softeware Handshake Request for Channel Destination */
#define DMA_CHX_SWHD_REQ_DST_WE		(1 << 1)		/* Softeware Handshake Request for Channel Destination Write Enable */
#define DMA_CHX_SWHD_SIGREQ_DST		(1 << 2)		/* Softeware Handshake Signal Request for Channel Destination */
#define DMA_CHX_SWHD_SIGREQ_DST_WE	(1 << 3)		/* Softeware Handshake Signal Request for Channel Destination Write Enable */
#define DMA_CHX_SWHD_LAST_DST		(1 << 4)		/* Softeware Handshake Last Request for Channel Destination */
#define DMA_CHX_SWHD_LST_DST_WE		(1 << 5)		/* Softeware Handshake Last Request for Channel Destination Write Enable */

/* Block Transfer Resume Request Register */
#define DMA_CHX_BLK_TFR_RESUMEREQ	(1 << 0)		/* Block Tranfer Resume Request tranfer */

/* Channelx AXI ID Register */
#define DMA_CHX_IDREG_READID_SUFFIX(x) ((x) << 0)	/* AXI Read ID Suffix */
#define DMA_CHX_IDREG_WRITEID_SUFFIX(x) ((x) << 16)	/* AXI Write ID Suffix */

/* Channelx AXI QOSREG */
#define DMA_CHX_AXI_AWQOS(x)		((x) << 0)		/* AXI AWQOS */
#define DMA_CHX_AXI_ARQOS(x)		((x) << 4)		/* AXI ARQOS */

/* 0x80 Interrupt Status Enable Register */
/* 0x88 Interrupt Status Register */
/* 0x90 Interrupt Signal Enable Register */
/* 0x98 Interrupt Clear Register */
#define DMA_CHX_BLOCK_TFR_DONE_INT		(1 << 0)		/* Block Transfer Done Interrupt */
#define DMA_CHX_DMA_TFR_DONE_INT		(1 << 1)		/* DMA Transfer Done Interrupt */
#define DMA_CHX_SRC_TRC_INT				(1 << 3)		/* Source Transaction Completed */
#define DMA_CHX_DST_TRC_INT				(1 << 4)		/* Destination Transaction Completed */
#define DMA_CHX_SRC_DEC_ERR_INT			(1 << 5)		/* Source Decode Error */
#define DMA_CHX_DST_DEC_ERR_INT			(1 << 6)		/* Destination Decode Error */
#define DMA_CHX_SRC_SLV_ERR_INT			(1 << 7)		/* Source Slave Error */
#define DMA_CHX_DST_SLV_ERR_INT			(1 << 8)		/* Destination Slave Error */
#define DMA_CHX_RD_DEC_ERR_INT			(1 << 9)		/* LLI Read Decode Error */
#define DMA_CHX_WR_DEC_ERR_INT			(1 << 10)		/* LLI Write Decode Error */
#define DMA_CHX_RD_SLV_ERR_INT			(1 << 11)		/* LLI Read Slave Error */
#define DMA_CHX_WR_SLV_ERR_INT			(1 << 12)		/* LLI Write Slave Error */
#define DMA_CHX_SR_OR_LLI_INV_ERR_INT	(1 << 13)		/* Shadows Register or LLI Invalid Error */
#define DMA_CHX_SLVIF_MBT_ERR_INT		(1 << 14)		/* Slave Interface Multi Block Type Error */
#define DMA_CHX_SLVIF_DEC_ERR_INT		(1 << 16)		/* Slave Interface Decode Error */
#define DMA_CHX_SLVIF_WR2RO_ERR_INT		(1 << 17)		/* Slave Interface Write to Read Only Error */
#define DMA_CHX_SLVIF_RD2RWO_ERR_INT	(1 << 18)		/* Slave Interface Read to Write Only Error */
#define DMA_CHX_SLVIF_WRONCHEN_ERR_INT	(1 << 19)		/* Slave Interface Write On Channeld Eanbled Error */
#define DMA_CHX_SLVIF_SR_WRON_VALID_ERR_INT (1 << 20)	/* Shadow Register Write On Valid Error*/
#define DMA_CHX_SLVIF_WRONHODL_ERR_INT		(1 << 21)	/* Slave Interface Write On Hold Error */
#define DMA_CHX_CHLOCK_CLEARED_INT		(1 << 27)		/* Channel Lock Cleared */
#define DMA_CHX_CH_SRC_SUSPENDED_INT	(1 << 28)		/* Channel Source Suspended */
#define DMA_CHX_CH_SUSPENDED_INT	(1 << 29)			/* Channel Suspended */
#define DMA_CHX_CH_DISABLED_INT		(1 << 30)			/* Channel Disabled */
#define DMA_CHX_CH_ABORTERD_INT		(1 << 31)			/* Channel Aborted */
#define DMA_CHX_CH_ALL_INT		    0xFFFFFFFF			/* All Channel IT */


/*  
 *  Handshaking interface.
 */
typedef enum {
    DMA_HARDWARE_HS_IF = 0x0,   /* Hardware handshaking interface */
    DMA_SOFTWARE_HS_IF = 0x1,   /* Software handshaking interface */
}dma_hS_iface_t;

/* dma source/destination addr type */
typedef enum{
	DMA_ADDR_INC =	0,		/* Address increment */
	DMA_ADDR_FIXED=	1,		/* Address fixed */
}dma_addr_type_t;

/* dma master layer section 3.4 */
typedef enum{
	DMX_AXI_MASTER_1= 0,		/* AXI Master 1*/
	DMX_AXI_MASTER_2= 1,		/* AXI Master 2*/
}dma_mater_layer_t;

/* 
 *  Transfer mode for DMA, Multiblock type.
 * 	Table 6-19
 */
typedef enum {
    DMA_SINGLE_BLOCK = 0,                     /* Single-block or last transfer of multi-block */
    DMA_MULTI_BLOCK_AUTORELOAD = 1,           /* Auto-reloading */
    DMA_MULTI_BLOCK_CONTIGUOUS = 2,           /* contiguous address between blocks */
    DMA_MULTI_BLOCK_LINKEDLIST = 3            /* Block chaining using Linked lists */
}dma_transfer_mode_t;

/* 
 *  DMA's channel priority, 7 is the highest Priority, and 0 is the lowest.
 *  Chx CFG bit[51:49]
 */
typedef enum {
    DMA_CHANNEL_PRIO_0 = 0x0,  
    DMA_CHANNEL_PRIO_1 = 0x1,  
    DMA_CHANNEL_PRIO_2 = 0x2,  
    DMA_CHANNEL_PRIO_3 = 0x3,  
    DMA_CHANNEL_PRIO_4 = 0x4,  
    DMA_CHANNEL_PRIO_5 = 0x5,  
    DMA_CHANNEL_PRIO_6 = 0x6,  
    DMA_CHANNEL_PRIO_7 = 0x7  
}dma_channel_priority_t;

/* 
 *  Transfer type for DMA.
 *  Table 6-20
 */
typedef enum {                              	/**     Transfer Type  **	|**  Flow Control      **/
    DMA_CFG_MEM2MEM_WITH_DMA_CTRL = 0x0,        /*  Memory to Memory      	|       DW_axi_dmac   	*/
    DMA_CFG_MEM2PER_WITH_DMA_CTRL = 0x1,        /*  Memory to Peripheral    |       DW_axi_dmac   	*/
    DMA_CFG_PER2MEM_WITH_DMA_CTRL = 0x2,        /*  Peripheral to Memory    |       DW_axi_dmac   	*/
    DMA_CFG_PER2PER_WITH_DMA_CTRL = 0x3,        /*  Peripheral to Peripheral|       DW_axi_dmac   	*/
    DMA_CFG_PER2MEM_WITH_PER_CTRL = 0x4,        /*  Peripheral to Memory    |  (Source)Peripheral   */
    DMA_CFG_PER2PER_WITH_SRCPER_CTRL = 0x5,     /*  Peripheral to Peripheral|  (Source)Peripheral	*/
    DMA_CFG_MEM2PER_WITH_PER_CTRL = 0x6,        /*  Memory to Peripheral    |(Destination)Peripheral*/
    DMA_CFG_PER2PER_WITH_DSTPER_CTRL = 0x7      /*  Peripheral to Peripheral|(Destination)Peripheral*/
}dma_transfer_type_t;

/**
 * enum dma_transfer_direction - dma transfer mode and direction indicator
 * @DMA_MEM_TO_MEM: Async/Memcpy mode
 * @DMA_MEM_TO_DEV: Slave mode & From Memory to Device
 * @DMA_DEV_TO_MEM: Slave mode & From Device to Memory
 * @DMA_DEV_TO_DEV: Slave mode & From Device to Device
 */
typedef enum{
    DMA_MEM_TO_MEM,
    DMA_MEM_TO_DEV,
    DMA_DEV_TO_MEM,
    DMA_DEV_TO_DEV,
    DMA_TRANS_NONE,
}dma_transfer_direction_t;

/*
 * DMA SRC/DST CTL.MSIZE Number of data items to be transferred(Burst Length)
 * Table 6-17
 */
typedef enum {
	DMA_CTL_BURST_LENGTH_1	=	0x0,
	DMA_CTL_BURST_LENGTH_4	=	0x1,
	DMA_CTL_BURST_LENGTH_8	=	0x2,
	DMA_CTL_BURST_LENGTH_16	=	0x3,
	DMA_CTL_BURST_LENGTH_32	=	0x4,
	DMA_CTL_BURST_LENGTH_64	=	0x5,
	DMA_CTL_BURST_LENGTH_128=	0x6,
	DMA_CTL_BURST_LENGTH_256=	0x7,
	DMA_CTL_BURST_LENGTH_512=	0x8,
	DMA_CTL_BURST_LENGTH_1024=	0x9,
}dma_msize_type_t;

/*
 * SRC/DST CTL.TR_WIDTH Transfer width
 * Table 6-18
 **/
typedef enum{
	DMA_CTL_TR_WIDTH_8	=	0x0,
	DMA_CTL_TR_WIDTH_16	=	0x1,
	DMA_CTL_TR_WIDTH_32	=	0x2,
	DMA_CTL_TR_WIDTH_64	=	0x3,
	DMA_CTL_TR_WIDTH_128=	0x4,
	DMA_CTL_TR_WIDTH_256=	0x5,
	DMA_CTL_TR_WIDTH_512=	0x6,
}dma_tr_width_t;

#if 0
typedef enum{
	DMA_CHX_BLOCK_TFR_DONE_INT=(1 << 0),
	DMA_CHX_DMA_TFR_DONE_INT=(1 << 1),
	DMA_CHX_SRC_TRC_INT=(1 <<3),
	DMA_CHX_DST_TRC_INT=(1 << 4),
	DMA_CHX_SRC_DEC_ERR_INT=(1 << 5),
	DMA_CHX_DST_DEC_ERR_INT=(1 << 6),
	DMA_CHX_SRC_SLV_ERR_INT=(1 << 7),
	DMA_CHX_DST_SLV_ERR_INT=(1 << 8),
	DMA_CHX_BLOCK_TFR_DONE_INTRD_DEC_ERR_INT=(1 << 9),
	DMA_CHX_WR_DEC_ERR_INT=(1 << 10),
	DMA_CHX_RD_SLV_ERR_INT=(1 << 11),
	DMA_CHX_WR_SLV_ERR_INT=(1 <<12),
	DMA_CHX_SR_OR_LLI_INV_ERR_INT=(1 << 13),
	DMA_CHX_SLVIF_MBT_ERR_INT_ERR_INT=(1 << 14),
	DMA_CHX_SLVIF_DEC_ERR_INT=(1 << 16),
	DMA_CHX_RD_SLV_ERR_INTCHX_SLVIF_WR2RO_ERR_INT=(1 << 17),
	DMA_CHX_SLVIF_RD2RWO_ERR_INT=(1 << 18),
	DMA_CHX_SLVIF_WRONCHEN_ERR_INT=(1 << 19),
	DMA_CHX_SLVIF_DEC_ERR_INTF_SR_WRON_VALID_ERR_INT= (1 << 20),
	DMA_CHX_SLVIF_WRONHODL_ERR_INT=(1 << 21),
	DMA_CHX_CHLOCK_CLEARED_INT=(1 << 27),
	DMA_CHX_CH_SRC_SUSPENDED_INTC_SUSPENDED_INT=(1 << 28),
	DMA_CHX_CH_SUSPENDED_INT=(1 << 29),
	DMA_CHX_CH_DISABLED_INT=(1 << 30),
	DMA_CHX_CH_ABORTERD_INT=Bit32u(1 <<31),
	DMA_CHX_CH_INT_ALL=0xFFFFFFFFF
}CH_IT_status_t;
#endif

struct dw_dma_chan_reg{
    DW_REG(SAR);        			/* 0x00 Source Address Register */
    DW_REG(DAR);        			/* 0x08 Destination Address Register */
    DW_REG(BLOCK_TS);  				/* 0x10 Block Transfer Size Register */
    Bit32u CTL_LO;     				/* 0x18 Control Register Low */
    Bit32u CTL_HI;     				/* 0x1c Control Register High */
    Bit32u CFG_LO;     				/* 0x20 Configuration Register Low */
    Bit32u CFG_HI;     				/* 0x24 Configuration Register High */
    DW_REG(LLP);        			/* 0x28 Linked List Pointer */
    Bit32u STATUSREG_LO;  			/* 0x34 Status Register Low */
    Bit32u STATUSREG_HI;  			/* 0x30 Status Register High */
    DW_REG(SWHSSRCREG); 			/* 0x38 Software Handshake Source Register */
    DW_REG(SWHSDSTREG); 			/* 0x40 Software Handshake Destination Register */
    DW_REG(BLK_TFR_RESUME_REQ); 	/* 0x48 Block Transfer Resume Request Register */
    DW_REG(AXI_IDREG); 				/* 0x50 AXI ID Register */
    DW_REG(AXI_QOSREG); 			/* 0x58 AXI QOS Register */
    DW_REG(SSTAT);					/* 0x60 Source status Register */
    DW_REG(DSTAT);					/* 0x68 Destination Status Register */
    DW_REG(SSTATAR);				/* 0x70 Source Status Fetch Address Register */
    DW_REG(DSTATAR);				/* 0x78 Destination Status Fetch Address Register */
    DW_REG(INT_STAT_EN);			/* 0x80 Interrupt Status Enable Register */
    DW_REG(INT_STAT);				/* 0x88 Interrupt Status Register */
    DW_REG(INTSIG_EN);				/* 0x90 Interrupt Signal Enable Register */
    DW_REG(INTCLR);					/* 0x98 Interrupt Clear Register */
};

/*
 *  LLI structre.
*/
struct lli_status_t {
    DW_REG(sar);      		/* source address */
    DW_REG(dar);      		/* destination address */
	DW_REG(block_ts);		/* block transfer size */
    DW_REG(llp);      		/* linked list pointer */
    Bit32u ctl_lo;    		/* control register CTLx[31:0] */
    Bit32u ctl_hi;    		/* control register CTLx[63:32] */
    Bit32u sstat;    		/* Write back for source status */
    Bit32u dstat;    		/* Write back for destination status */
	DW_REG(llp_stat); 		/* chx llp status */
	DW_REG(resv);			/* reserved */
}lli[DW_DMA_LLI_LENGTH] __attribute__((aligned(64)));

/* 
 *  The parameters needed for initializing a DMA transfer.
 * @srcAddr: 32bit or 64bit, depends on hardware.
 * @dstAddr: 32bit or 64bit, depends on hardware.
*/
struct dma_chx_config_t
{
    Bit32u channel;             		/* DMA channel, it will be returned after init. */
    dma_transfer_direction_t dir;   	/* Transfer direction. It must be set before init. */
	dma_mater_layer_t sms;				/* Source Master Select */
	dma_mater_layer_t dms;				/* Destination Master Select */
    Bit32u data_size;            		/* Data size (bytes). If the DMA is the flow controller(defined in dma_transfer_type_t), this size must be set before init. */
    Bit32u src_addr;             		/* The source address must be set before init. */
    Bit32u dst_addr;            		/* The dest address must be set before init. */
    dma_addr_type_t src_addr_mode; 		/* Source address mode. It must be set before init. */
    dma_addr_type_t dst_addr_mode;		/* Destination address mode. It must be set before init. */
    dma_transfer_type_t trf_type;     	/* Transfer type. It must be set before init. */
    dma_transfer_mode_t src_trf_mode; 	/* Transfer mode. It must be set before init. */
    dma_transfer_mode_t dst_trf_mode;	/* Transfer mode. It must be set before init. */
    struct lli_status_t *lli;    		/* If the source or destination transfer mode is linked list, this pointer must be set before init. */
};

/*
 * First arg is functin args
 * Second arg is return value */
//typedef int(*intc_callback)(void*, CH_IT_status_t*);
/* 
 *  The parameters needed for initializing a DMA transfer.
 * @srcAddr: 32bit or 64bit, depends on hardware.
 * @dstAddr: 32bit or 64bit, depends on hardware.
*/
struct dma_chx_user_config_t
{
    dma_transfer_direction_t dir;   	/* Transfer direction. It must be set before init. */
    Bit32u data_size;            		/* Data size (bytes). If the DMA is the flow controller(defined in dma_transfer_type_t), this size must be set before init. */
    Bit32u src_addr;             		/* The source address must be set before init. */
    Bit32u dst_addr;            		/* The dest address must be set before init. */
    dma_addr_type_t src_addr_mode; 		/* Source address mode. It must be set before init. */
    dma_addr_type_t dst_addr_mode;		/* Destination address mode. It must be set before init. */
	//intc_callback* func;               	/* if any interrupt occurs return intc statuc to user. you can add your call back here. */
};


struct dw_dma{
	struct dw_dma_common_reg *common_reg;
	struct dw_dma_chan_reg * chan_reg[8];
	struct dma_chx_config_t config[8];
}dma;

/* common register operation */
Bit32u dma_get_id(struct dw_dma_common_reg *reg);
Bit32u dma_get_comp_ver(struct dw_dma_common_reg *reg);
void dma_gloabe_int_enable(struct dw_dma_common_reg *reg);
void dma_gloabe_int_disable(struct dw_dma_common_reg *reg);
void dma_dmac_enable(struct dw_dma_common_reg *reg);
Bit32u dma_dmac_disable(struct dw_dma_common_reg *reg);
Bit32u dma_dmac_is_enable(struct dw_dma_common_reg *reg);
Bit32u dma_dmac_gloabe_int_is_enable(struct dw_dma_common_reg *reg);
void dma_ch_enable(struct dw_dma_common_reg *reg, Bit8u channel);
void dma_ch_disable(struct dw_dma_common_reg *reg, Bit8u channel);
Bit32u dma_ch_is_free(struct dw_dma_common_reg *reg, Bit8u channel);
void dma_ch_susp_enable(struct dw_dma_common_reg *reg, Bit8u channel);
void dma_ch_susp_disable(struct dw_dma_common_reg *reg, Bit8u channel);
void dma_ch_abort_enable(struct dw_dma_common_reg *reg, Bit8u channel);
void dma_ch_abort_disable(struct dw_dma_common_reg *reg, Bit8u channel);
Bit32u dma_int_status_read(struct dw_dma_common_reg *reg);
Bit8u dma_chx_int_status_read(struct dw_dma_common_reg *reg, Bit8u channel);
void dma_com_int_status_clear(struct dw_dma_common_reg *reg, Bit32u val);
void dma_com_int_status_enable(struct dw_dma_common_reg *reg, Bit32u val);
void dma_com_int_status_disable(struct dw_dma_common_reg *reg, Bit32u val);
void dma_com_int_signal_enable(struct dw_dma_common_reg *reg, Bit32u val);
void dma_com_int_signal_disable(struct dw_dma_common_reg *reg, Bit32u val);
Bit32u dma_com_int_status_read(struct dw_dma_common_reg *reg);
void dma_reset(struct dw_dma_common_reg *reg);
Bit32u dma_reset_read(struct dw_dma_common_reg *reg);

/* link list registr operation */
Bit32u dma_chx_sar_read(struct dw_dma_chan_reg *reg);
void dma_chx_sar_write(struct dw_dma_chan_reg *reg, Bit32u val);
Bit32u dma_chx_dar_read(struct dw_dma_chan_reg *reg);
void dma_chx_dar_write(struct dw_dma_chan_reg *reg, Bit32u val);
Bit32u dma_chx_block_ts_read(struct dw_dma_chan_reg *reg);
void dma_chx_block_ts_write(struct dw_dma_chan_reg *reg, Bit32u val);

Bit32u dma_chx_ctl_lo_read(struct dw_dma_chan_reg *reg);
void dma_chx_ctl_lo_write(struct dw_dma_chan_reg *reg, Bit32u val);
void dma_chx_ctl_lo_direct_write(struct dw_dma_chan_reg *reg, Bit32u val);
Bit32u dma_chx_ctl_hi_read(struct dw_dma_chan_reg *reg);
void dma_chx_ctl_hi_write(struct dw_dma_chan_reg *reg, Bit32u val);
void dma_chx_ctl_hi_direct_write(struct dw_dma_chan_reg *reg, Bit32u val);
Bit32u dma_chx_cfg_lo_read(struct dw_dma_chan_reg *reg);
void dma_chx_cfg_lo_write(struct dw_dma_chan_reg *reg, Bit32u val);
void dma_chx_cfg_lo_direct_write(struct dw_dma_chan_reg *reg, Bit32u val);
Bit32u dma_chx_cfg_hi_read(struct dw_dma_chan_reg *reg);
void dma_chx_cfg_hi_write(struct dw_dma_chan_reg *reg, Bit32u val);
void dma_chx_cfg_hi_direct_write(struct dw_dma_chan_reg *reg, Bit32u val);
Bit32u dma_chx_llp_read(struct dw_dma_chan_reg *reg);
void dma_chx_llp_write(struct dw_dma_chan_reg *reg, Bit32u val);

Bit32u dma_chx_compled_blk_transfer_size_read(struct dw_dma_chan_reg *reg);
Bit32u dma_chx_data_left_in_fifo_read(struct dw_dma_chan_reg *reg);
Bit32u dma_chx_swhs_read(struct dw_dma_chan_reg *reg);
void dma_chx_swhs_write(struct dw_dma_chan_reg *reg, Bit32u val);
Bit32u dma_chx_swhd_read(struct dw_dma_chan_reg *reg);
void dma_chx_swhd_write(struct dw_dma_chan_reg *reg, Bit32u val);
void dma_blk_tfr_resume_req(struct dw_dma_chan_reg *reg);
Bit32u dma_chx_axi_id_read(struct dw_dma_chan_reg *reg);
void dma_chx_axi_id_write(struct dw_dma_chan_reg *reg, Bit32u val);
Bit32u dma_chx_axi_qos_read(struct dw_dma_chan_reg *reg);
void dma_chx_axi_qos_write(struct dw_dma_chan_reg *reg, Bit32u val);
Bit32u dma_chx_sstat_read(struct dw_dma_chan_reg *reg);
Bit32u dma_chx_dstat_read(struct dw_dma_chan_reg *reg);
Bit32u dma_chx_sstatar_read(struct dw_dma_chan_reg *reg);
void dma_chx_sstatar_write(struct dw_dma_chan_reg *reg, Bit32u val);
Bit32u dma_chx_dstatar_read(struct dw_dma_chan_reg *reg);
void dma_chx_dstatar_write(struct dw_dma_chan_reg *reg, Bit32u val);
void dma_chx_int_status_eanble(struct dw_dma_chan_reg *reg, Bit32u val);
void dma_chx_int_status_diable(struct dw_dma_chan_reg *reg, Bit32u val);
Bit32u dma_chx_spec_int_status_read(struct dw_dma_chan_reg *reg);
void dma_chx_int_signal_eanble(struct dw_dma_chan_reg *reg, Bit32u val);
void dma_chx_int_signal_diable(struct dw_dma_chan_reg *reg, Bit32u val);
void dma_chx_int_status_clear(struct dw_dma_chan_reg *reg, Bit32u val);
void CK_DMAC_Handler(CK_UINT32 irqid);
Bit32u dma_intc_init(void);
int test_dma_lli(void);
int test_dma_singal(void);

Bit8u dma_channel_free_query(struct dw_dma* dma,Bit8u chanid);
Bit32u dma_intc_init_1(struct dw_dma* dma,Bit8u chanid);
int test_dma_singal_allchan(Bit8u testnum);
int test_dma_lli_allchan(Bit8u testnum);
#endif /*__DMA_H__*/
