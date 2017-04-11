#ifndef __I2C_H__
#define __I2C_H__

#include "io.h"
#include "include/ck810.h"
#include "include/datatype.h"
#include "include/intc.h"
#include "include/types.h"
#include "types.h"
#include "ck810.h"

#define  CK_IIC_IRQ   CK_INTC_IIC
typedef struct{
	Bit32u IC_CON; 				/* 0x00 I2c Control */
	Bit32u IC_TAR; 				/* 0x04 I2C Target Address */
	Bit32u IC_SAR; 			    /* 0x08 I2C Slave Address */
	Bit32u IC_HS_MADDR; 		/* 0x0c I2C HS Master Mode Code Address */
	Bit32u IC_DATA_CMD; 		/* 0x10 I2C Rx/Tx Data Buffer and Command */
	Bit32u IC_SS_SCL_HCNT; 		/* 0x14 Standard speed I2C Clock SCL High Count */
	Bit32u IC_SS_SCL_LCNT; 		/* 0x18 Standard speed I2C Clock SCL Low Count */
	Bit32u IC_FS_SCL_HCNT; 		/* 0x1C Fast Mode and Fast Mode Plus I2C Clock SCL High Count */
	Bit32u IC_FS_SCL_LCNT; 		/* 0x20 Fast Mode and Fast Mode Plus I2C Clock SCL Low Count */
	Bit32u IC_HS_SCL_HCNT; 		/* 0x24 High speed I2C Clock SCL High Count */
	Bit32u IC_HS_SCL_LCNT; 		/* 0x28 High speed I2C Clock SCL Low Count */
	Bit32u IC_INTR_STAT; 		/* 0x2c I2C Interrupt Status */
	Bit32u IC_INTR_MASK; 		/* 0x30 I2C Interrupt MASK */
	Bit32u IC_RAW_INTR_STAT; 	/* 0x34 I2C Raw Interrupt Status */
	Bit32u IC_RX_TL; 			/* 0x38 I2C Receive FIFO Threshold */
	Bit32u IC_TX_TL; 			/* 0x3c I2C Transmit FIFO Threshold */
	Bit32u IC_CLR_INTR; 		/* 0x40 Clear Combined and Individual Interrupts */
	Bit32u IC_CLR_RX_UNDER; 	/* 0x44 Clear RX_UNDER Interrupt */
	Bit32u IC_CLR_RX_OVER; 		/* 0x48 Clear RX_OVER Interrupt */
	Bit32u IC_CLR_TX_OVER; 		/* 0x4c Clear TX_OVER Interrupt */
	Bit32u IC_CLR_RD_REQ; 		/* 0x50 Clear RD_REQ Interrupt */
	Bit32u IC_CLR_TX_ABRT; 		/* 0x54 Clear TX_ABRT Interrupt */
	Bit32u IC_CLR_RX_DONE; 		/* 0x58 Clear RX_DONE Interrupt */
	Bit32u IC_CLR_ACTIVITY; 	/* 0x5c Clear ACTIVITY Interrupt */
	Bit32u IC_CLR_STOP_DET; 	/* 0x60 Clear STOP_DEL Interrupt */
	Bit32u IC_CLR_START_DET; 	/* 0x64 Clear START_DEL Interrupt */
	Bit32u IC_CLR_GEN_CALL; 	/* 0x68 Clear CEN_CALL Interrupt */
	Bit32u IC_ENABLE; 			/* 0x6c I2C Enable */
	Bit32u IC_STATUS; 			/* 0x70 I2C Status */
	Bit32u IC_TXFLR; 			/* 0x74 Transmit FIFO Level Register */
	Bit32u IC_RXFLR; 			/* 0x78 Receive FIFO Level Register */
	Bit32u IC_SDA_HOLD; 		/* 0x7c SDA hold time length Register */
	Bit32u IC_TX_ABRT_SOURCE; 	/* 0x80 I2C Transmit Abort Status Register */
	Bit32u IC_SLV_DATA_NACK_ONLY; 	/* 0x84 Generate SLV_DATA_NACH Register */
	Bit32u IC_DMA_CR; 			/* 0x88 DMA Control Register. not used now */
	Bit32u IC_DMA_TDLR; 		/* 0x8c DMA Transmit Data Level Register. not used now */
	Bit32u IC_DMA_RDLR; 		/* 0x90 DMA Receive Data Level Register. not used now */
	Bit32u IC_SDA_SETUP; 		/* 0x94 I2C SDA Setup Register */
	Bit32u IC_ACK_GENERAL_CALL; 	/* 0x98 I2C ACK General Call Register */
	Bit32u IC_ENABLE_STATUS; 	/* 0x9c I2C Enable Status Register */
	Bit32u IC_FS_SPKLEN; 		/* 0xA0 ISS and FS spike suppression limit Register */
	Bit32u IC_HS_SPKLEN; 		/* 0xA4 HS spike suppression limit Register */
	Bit32u IC_CLR_RESTART_DET; 	/* 0xA8 Clear RESTART_DEL Register */
	Bit32u IC_SCL_STUCK_AT_LOW_TIMEOUT; /* 0xAC I2C SCL stuck at low timeout register */
	Bit32u IC_SDA_STUCK_AT_LOW_TIMEOUT; /* 0xB0 I2C SDA stuck at low timeout register */
	Bit32u IC_CLR_SCL_STUCK_DET; /* 0xB4 Clear SCL Stuck at Low Detect register */
	Bit32u IC_DEVICE_ID; 		 /* 0xB8 I2C Device ID register */
	Bit32u Rev[14];              /* 0xBC~0xF0*/
	Bit32u IC_COMP_PARAM_1;      /* 0xF4 I2C Component Parameter Register 1*/
	Bit32u IC_COMP_VERSION;      /* 0xF8 I2C Component Version Register*/
	Bit32u IC_COMP_TYPE;         /* 0xFC I2C Component Type Register*/


}I2C_RegTypeDef;

#define I2C    ((I2C_RegTypeDef *)APB_I2C_BASEADDR)            //define i2c reg struct,and get the i2c addrbase
//#define I2C_CLOCK      APB_DEFAULT_FREQ                      //I2C_CLOCK=APB CLK

struct I2C_Config_par{

};

/* 0x00 I2c Control */
#define IC_CON_SMBUS_SLV_ADDR_EN 		(1 << 19)	/* This bit controls to enable DW_apb_i2c slave as persistent or non-persistent slave.*/
#define IC_CON_SMBUS_ARP_EN 	 		(1 << 18) 	/* This bit controls whether DW_apb_i2c should enable Address Resolution Logic in SMBus Mode.*/
#define IC_CON_SMBUS_SLV_CMD_EN 		(1 << 17) 	/* DW_apb_i2c slave only receives Quick commands in SMBus Mode */
#define IC_CON_OPTIONAL_SAR_CTRL 		(1 << 16) 	/* Enables the usage of IC_OPTIONAL_SAR register. */
#define IC_CON_BUS_CLR_FEATURE_CTRL 	(1 << 11) 	/* Bus Clear Feature is enabled */
#define IC_CON_STOP_DEL_IF_ACTIVE 		(1 << 10) 	/* Issues the STOP_DET interrupt only when the master is active*/
#define IC_CON_RX_FIFO_FULL_HLD_CTRL 	(1 << 9) 	/* This bit controls whether DW_apb_i2c should hold the bus when the Rx FIFO is physically full*/
#define IC_CON_TX_EMPTY_CTRL 			(1 << 8) 	/* This bit controls the generation of the TX_EMPTY interrupt */
#define IC_CON_STOP_DEL_IFADDRED 		(1 << 7) 	/* issues the STOP_DET interrupt only when it is addressed */
#define IC_CON_SLAVE_DISABLE 			(1 << 6) 	/* This bit controls whether I2C has its slave disabled */
#define IC_CON_RESTART_EN 				(1 << 5) 	/* Determines whether RESTART conditions may be sent when acting as a master */
#define IC_CON_10BITADDR_MASTER 		(1 << 4) 	/* whether the DW_apb_i2c starts its transfers in 7- or 10-bit addressing mode when acting as a master */
#define IC_CON_10BITADDR_SLAVE 			(1 << 3) 	/* whether the DW_apb_i2c starts its transfers in 7- or 10-bit addressing mode when acting as a slave */
#define IC_CON_SPEED(x) 				((x) << 1) 	/* i2c speed config */
#define IC_CON_MASTER_MODE 				(1 << 0) 	/* This bit controls whether the DW_apb_i2c master is enabled. */

typedef enum{
	MASTER ,
    SLAVE
}IC_Mode_Typedef;

typedef enum{
	SPEED_STANDARD_MODE = 2,
	SPEED_FAST_MODE = 4,
	SPEED_HIGH_SPEED_MODE = 6
}IC_SpeedMode_Typedef;

typedef enum{
	address_bit_10 ,
	address_bit_7
}IC_AddrMode_Typedef;

typedef enum{
	Write=0 ,
	Read=1
}IC_OperaMode_Typedef;

typedef enum
{
  IC_IT_MASK = 0,
  IC_IT_UMASK = 1
}IC_ITMask_TypeDef;
/* 0x04 I2C Target Address */
#define IC_TAR_DEVICE_ID 				(1 << 13) 	/* whether a Device-ID of a particular slave mentioned in IC_TAR[6:0] is to be performed */
#define IC_TAR_10BITADDR_MASTER 		(1 << 12)  	/* whether the DW_apb_i2c starts its transfers in 7-or 10-bit addressing mode when acting as a master */
#define IC_TAR_SPECIAL 					(1 << 11) 	/* whether software performs a Device-ID, General Call or START BYTE command */
#define IC_TAR_GC_OR_START 				(1 << 10) 	/* indicates whether a General Call or START byte command is to be performed by the DW_apb_i2c */
#define IC_TAR_TAR(x) 					((x) << 0) 	/* This is the target address for any master transaction */

/* 0x08 I2C Slave Address */
/* 0x0c I2C HS Master Mode Code Address */
#define IC_HS_MAR(x)  					((x) << 0) 	/* This bit field holds the value of the I2C HS mode master code. */
/* 0x10 I2C Rx/Tx Data Buffer and Command */
#define IC_DB_FIRST_DATA_BYTE 			(1 << 11) 	/* Indicates the first data byte received after the address phase for receive 
													 * transfer in Master receiver or Slave receiver mode.*/
#define IC_DB_RESTART 					(1 << 10) 	/* This bit controls whether a RESTART is issued before the byte is sent or received */
#define IC_DB_STOP 						(1 << 9) 	/* This bit controls whether a STOP is issued after the byte is sent or received */
#define IC_DB_CMD 						(1 << 8) 	/* This bit controls whether a read or a write is performed */
#define IC_DB_DAT(x) 					x  	        /* This register contains the data to be transmitted or received on the I2C bus. */

/* 0x14 Standard speed I2C Clock SCL High Count */
/* 0x18 Standard speed I2C Clock SCL Low Count */
/* 0x1C Fast Mode and Fast Mode Plus I2C Clock SCL High Count */
/* 0x20 Fast Mode and Fast Mode Plus I2C Clock SCL Low Count */
/* 0x24 High speed I2C Clock SCL High Count */
/* 0x28 High speed I2C Clock SCL Low Count */

/* 0x2c I2C Interrupt Status */
/* 0x30 I2C Interrupt MASK */


/* 0x34 I2C Raw Interrupt Status */
typedef enum{
	IC_INT_SCL_STUCK_AT_LOW = 16384,
	IC_INT_MST_ON_HOLD = 8192,
	IC_INT_RESTART_DET = 4096,
	IC_INT_GEN_CALL = 2048,
	IC_INT_START_DET = 1024,
	IC_INT_STOP_DET = 512,
	IC_INT_ACTIVITY = 256,
	IC_INT_RX_DONE = 128,
	IC_INT_TX_ABRT = 64,
	IC_INT_RD_REQ = 32,
	IC_INT_TX_EMPTY = 16,
	IC_INT_TX_OVER = 8,
	IC_INT_RX_FULL = 4,
	IC_INT_RX_OVER = 2,
	IC_INT_RX_UNDE = 1,
	IC_INT_ALL = 32767,
	IC_INT_NONE = 0,
}IC_INTR_Typedef;

#if 0
#define IC_INT_SCL_STUCK_AT_LOW 		(1 << 14) 			/* Indicates whether the SCL Line is stuck at low for the
															 * IC_SCL_STUCK_LOW_TIMOUT number of ic_clk periods*/
#define IC_INT_MST_ON_HOLD 				(1 << 13) 			/* Indicates whether a master is holding the bus and the Tx FIFO is empty */
#define IC_INT_RESTART_DET 				(1 << 12) 			/* Indicates whether a RESTART condition has occurred on the I2C interface */
#define IC_INT_GEN_CALL 				(1 << 11) 			/* Set only when a General Call address is received and it is acknowledged */
#define IC_INT_START_DET 				(1 << 10) 			/* Indicates whether a START or RESTART condition has occurred */
#define IC_INT_STOP_DET 				(1 << 9) 			/* Indicates whether a STOP condition has occurred */
#define IC_INT_ACTIVITY 				(1 << 8) 			/* This bit captures DW_apb_i2c activity and stays set until it is cleared */
#define IC_INT_RX_DONE 					(1 << 7) 			/* This occurs on the last byte of the transmission */
#define IC_INT_TX_ABRT 					(1 << 6) 			/* unable to complete the intended actions on the contents of the transmit FIFO */
#define IC_INT_RD_REQ 					(1 << 5) 			/* acting as a slave and another I2C master is attempting to read data from DW_apb_i2c */
#define IC_INT_TX_EMPTY 				(1 << 4) 			/* TX_EMPTY */
#define IC_INT_TX_OVER 					(1 << 3) 			/* f the transmit buffer is filled to IC_TX_BUFFER_DEPTH */
#define IC_INT_RX_FULL 					(1 << 2) 			/* Set when the receive buffer reaches or goes above the RX_TL threshold in the IC_RX_TL register */
#define IC_INT_RX_OVER 					(1 << 1) 			/* Set if the receive buffer is completely filled to IC_RX_BUFFER_DEPTH */
#define IC_INT_RX_UNDER 				(1 << 0) 			/* Set if the processor attempts to read the receive buffer when it is empty */
#endif
/* 0x38 I2C Receive FIFO Threshold */
/* 0x3c I2C Transmit FIFO Threshold */
/* 0x40 Clear Combined and Individual Interrupts */
/* 0x44 Clear RX_UNDER Interrupt */
/* 0x48 Clear RX_OVER Interrupt */
/* 0x4c Clear TX_OVER Interrupt */
/* 0x50 Clear RD_REQ Interrupt */
/* 0x54 Clear TX_ABRT Interrupt */
/* 0x58 Clear RX_DONE Interrupt */
/* 0x5c Clear ACTIVITY Interrupt */
/* 0x60 Clear STOP_DEL Interrupt */
/* 0x64 Clear START_DEL Interrupt */
/* 0x68 Clear CEN_CALL Interrupt */
/* 0x6c I2C Enable */
#define IC_ENABLE_SDA_STUCK_RECOVERY_ENABLE 		(1 << 3) 			/* this bit is used as a control knob to initiate
																		 * the SDA Recovery Mechanism */
#define IC_ENABLE_TX_CMD_BLOCK 						(1 << 2) 			/* To block the execution of Master commands */
#define IC_ENABLE_ABORT 							(1 << 1) 			/* the controller initiates the transfer abort */
#define IC_ENABLE_ENABLE 							(1 << 0) 			/* Controls whether the DW_apb_i2c is enabled */

/* 0x70 I2C Status */
#if 0
#define IC_STAT_SDA_STUCK_NOT_RECOVERED 			(1 << 11) 			/* an SDA stuck at low is not recovered after the recovery mechanism */
#define IC_STAT_SLV_HOLD_RX_FIFO_FULL 				(1 << 10) 			/* the Rx FIFO being Full and an additional byte being received */
#define IC_STAT_SLV_HOLD_TX_FIFO_EMPTY 				(1 << 9) 			/* BUS Hold in Slave mode for the Read request when the Tx FIFO is empty */
#define IC_STAT_MST_HOLD_RX_FIFO_FULL 				(1 << 8) 			/* the Rx FIFO being Full and an additional byte being received */
#define IC_STAT_MST_HOLD_TX_FIFO_EMPTY 				(1 << 7) 			/* BUS Hold in Master mode for the Read request when the Tx FIFO is empty */
#define IC_STAT_SLV_ACTIVITY 						(1 << 6) 			/* Slave FSM Activity Status */
#define IC_STAT_MST_ACTIVITY 						(1 << 5) 			/* Master FSM Activity Status */
#define IC_STAT_RFF 								(1 << 4) 			/* Receive FIFO Completely Full Status */
#define IC_STAT_RFNE 								(1 << 3) 			/* Receive FIFO Not Empty Status */
#define IC_STAT_TFE 								(1 << 2) 			/* Transmit FIFO Completely Full Status */
#define IC_STAT_TFNF 								(1 << 1) 			/* Transmit FIFO Not Empty Status */
#define IC_STAT_ACTIVITY 							(1 << 0) 			/* I2C Activity Status */
#endif

typedef enum{
	IC_STAT_SDA_STUCK_NOT_RECOVERED = 2048,
	IC_STAT_SLV_HOLD_RX_FIFO_FULL = 1024,
	IC_STAT_SLV_HOLD_TX_FIFO_EMPTY = 512,
	IC_STAT_MST_HOLD_RX_FIFO_FULL = 256,
	IC_STAT_MST_HOLD_TX_FIFO_EMPTY = 128,
	IC_STAT_SLV_ACTIVITY = 64,
	IC_STAT_MST_ACTIVITY = 32,
	IC_STAT_RFF = 16,
	IC_STAT_RFNE= 8,
	IC_STAT_TFE = 4,
	IC_STAT_TFNF = 2,
	IC_STAT_ACTIVITY = 1,
}IC_STATUS_Typedef;

/* 0x74 Transmit FIFO Level Register */
/* 0x78 Receive FIFO Level Register */
/* 0x7c SDA hold time length Register */
#define IC_SDA_RX_HODL(x) 							((x) << 16) 		/* Sets the required SDA hold time when DW_apb_i2c acts as a reciever */
#define IC_SDA_TX_HODL(x) 							((x) << 0) 			/* Sets the required SDA hold time when DW_apb_i2c acts as a transmitter */
/* 0x80 I2C Transmit Abort Status Register */
#define IC_TX_ABRT_TX_FLUSH_CNT(x) 					((x) << 23) 		/* the number of Tx FIFO data */
#define IC_TX_ABRT_DEVICE_WRITE 					(1 << 20) 			/* the Tx-FIFO consists of write commands*/
#define IC_TX_ABRT_DEVICE_SLVADDR_NOACK 			(1 << 19) 			/* the slave address sent was not acknowledged by any slave*/
#define IC_TX_ABRT_DEVICE_NOACK 					(1 << 18) 			/* DEVICE_ID transfer and the device ID sent is not acknowledged by any slave */
#define IC_TX_ABRT_SDA_STUCK_AT_LOW 				(1 << 17) 			/* Master detects Master the SDA is Stuck at low */
#define IC_TX_ABRT_USER_ABRT 						(1 << 16) 			/* Master has detected the transfer abort */
#define IC_TX_ABRT_SLVRD_INTX 						(1 << 15) 			/* user writes a 1 in CMD (bit 8) of IC_DATA_CMD register */
#define IC_TX_ABRT_SLV_ARBLOST 						(1 << 14) 			/* Slave lost the bus while transmitting data to a remote master*/
#define IC_TX_ABRT_SLVFLUSH_TXFIFO 					(1 << 13) 			/* the slave issues a TX_ABRT interrupt to flush old data in TX FIFO */
#define IC_TX_ABRT_LOST 							(1 << 12) 			/* Master has lost arbitration */
#define IC_TX_ABRT_MASTER_DIS 						(1 << 11) 			/* initiate a Master operation with the Master mode disabled */
#define IC_TX_ABRT_10B_RD_NORSTRT 					(1 << 10) 			/* The restart is disabled */
#define IC_TX_ABRT_SBYTE_NORSTRT 					(1 << 9) 			/* The restart is disabled  and the user is trying to send a START Byte */
#define IC_TX_ABRT_HS_NORSTRT 						(1 << 8) 			/* The restart is disabled and the user is trying to use the master to
																		 * transfer data in High Speed mode */
#define IC_TX_ABRT_SBYTE_ACKDET 					(1 << 7) 			/* Master has sent a START Byte and the START Byte was acknowledged */
#define IC_TX_ABRT_HS_ACKDET 						(1 << 6) 			/* Master is in High Speed mode and the High Speed Master code was acknowledged */
#define IC_TX_ABRT_GCALL_READ 						(1 << 5) 			/* sent a General Call but the user programmed following the General Call */
#define IC_TX_ABRT_CGALL_NOACK 						(1 << 4) 			/* in master mode sent a General Call and no slave on the bus acknowledged the General Call*/
#define IC_TX_ABRT_TXDATA_NOACK 					(1 << 3) 			/* Master has received an acknowledgement for the address */
#define IC_TX_ABRT_10ADDR2_NOACK 					(1 << 2) 			/* Master is in 10-bit address mode and the second address byte of the 10-bit address
																		 * was not acknowledged by any slave */
#define IC_TX_ABRT_10ADDR1_NOACK 					(1 << 1) 			/* 10-bit address mode, first 10-bit address byte was not acknowledged by any slave */
#define IC_TX_ABRT_7B_ADDR_NOACK 					(1 << 0) 			/* 7-bit addressing, address sent was not acknowledged by any slave */
/* 0x84 Generate SLV_DATA_NACH Register */
/* 0x88 DMA Control Register. not used now */
/* 0x8c DMA Transmit Data Level Register. not used now */
/* 0x90 DMA Receive Data Level Register. not used now */
/* 0x94 I2C SDA Setup Register */
/* 0x98 I2C ACK General Call Register */
/* 0x9c I2C Enable Status Register */
#define IC_ENABLE_STATUS_SLV_RX_DATA_LOST 			(1 << 2) 			/* Slave Received Data Lost */
#define IC_ENABLE_STATUS_SLV_DISABLED_WHILE_BUSY 	(1 << 1) 			/* Slave Disabled While Busy (Transmit, Receive)*/
#define IC_ENABLE_STATUS_IC_EN 						(1 << 0) 			/* ic_en Status */
/* 0xA0 ISS and FS spike suppression limit Register */
/* 0xA4 HS spike suppression limit Register */
/* 0xA8 Clear RESTART_DEL Register */
/* 0xAC I2C SCL stuck at low timeout register */
/* 0xB0 I2C SDA stuck at low timeout register */
/* 0xB4 Clear SCL Stuck at Low Detect register */
/* 0xB8 I2C Device ID register */

/* 0xF4 I2C COMP PARAM 1 register */
#define IC_PARAM1_TX_BUFFER_DEPTH 					(0xFF << 16) 		/* The value of this register is derived from the IC_TX_BUFFER_DEPTH coreConsultant parameter */
#define IC_PARAM1_RX_BUFFER_DEPTH 					(0xFF << 8) 			/* The value of this register is derived from the IC_RX_BUFFER_DEPTH coreConsultant parameter */
#define IC_PARAM1_ADD_ENCODED_PARAMS 				(1 << 7) 			/* the capability of reading these encoded parameters via software has been included */
#define IC_PARAM1_HAS_DMA 							(1 << 6) 			/* IC_HAS_DMA */
#define IC_PARAM1_INTR_IO 							(1 << 5) 			/* IC_INTR_IO */
#define IC_PARAM1_HC_COUNT_VALUES 					(1 << 4) 			/* IC_HC_COUNT_VALUES */
#define IC_PARAM1_MAX_SPEED_MODE 					(0x3 << 2) 			/* IC_MAX_SPEED_MODE */
#define IC_PARAM1_APB_DATA_WIDTH 					(0x3 <<0) 			/* IC_APB_DATA_WIDTH */

/*Function1-5:Control TX empty IT  ,set bit8 of IC_CON reg(only in master mode)*/
Bit32u I2C_TXEMP_CTRL(BOOL NewState);
Bit32u I2C_Restart_EN(State_TypeDef NewState);
/*set IC target address ,set bit[9:0] of IC_TAR reg*/
Bit32u I2C_Set_TAR_Addr(Bit32u Address);
/*set IC slave address ,set bit[9:0] of IC_SAR reg*/
Bit32u I2C_Set_SLA_Addr(Bit32u Address);
/*send IC data ,set bit[7:0] of IC_DATA_CMD*/
Bit32u I2C_Send_Data(Bit8u Data);
/*Function5-6:set bit[8] to Read,read bit[10:8] of IC_DATA_CMD*/
void I2C_Receive_Set(void);
/*receive IC data read/write ,read bit[7:0] of IC_DATA_CMD*/
Bit8u I2C_Receive_Data(void);
/*restart transfer or not ,set bit[10] of IC_DATA_CMD*/
Bit32u I2C_Set_Restart(State_TypeDef NewState);
/*stop transfer or not ,set bit[9] of IC_DATA_CMD*/
Bit32u I2C_Set_Stop(State_TypeDef NewState);

/*set IC Standard Speed I2C Clock SCL High and Low Counts ,set bit[15:0] of IC_SS_SCL_HCNT & IC_SS_SCL_LCNT reg
 * unit:KHz
 * Duty ratio:50%
 * */
Bit32u I2C_Set_SS_SCL_FREQ(Bit8u Frequeny);
/*set Fast Mode or Fast Mode Plus I2C Clock SCL High and Low Counts ,set bit[15:0] of IC_FS_SCL_HCNT & IC_FS_SCL_LCNT reg*/
Bit32u I2C_Set_FS_SCL_FREQ(Bit32u Frequeny);
/*set High Speed I2C Clock SCL High and Low Counts ,set bit[15:0] of IC_HS_SCL_HCNT & IC_HS_SCL_LCNT reg*/
Bit32u I2C_Set_HS_SCL_FREQ(Bit32u Frequeny);
/*Mask or umask raw interrupt status,read bit[15:0] of IC_RAW_INRT_STAT reg*/
Bit32u I2C_Mask_IT_EN(Bit32u INTR_Item,IC_ITMask_TypeDef MaskState);
/*Mask raw interrupt status,read bit[15:0] of IC_RAW_INRT_STAT reg*/
Bit32u I2C_Mask_IT(IC_INTR_Typedef INTR_Item);
/*UMask raw interrupt status,read bit[15:0] of IC_RAW_INRT_STAT reg*/
Bit32u I2C_UMask_IT(IC_INTR_Typedef INTR_Item);
/*Query interrupt status,read bit[14:0] of IC_INRT_STAT reg*/
ITStatus_TypeDef I2C_Get_ITStat(IC_INTR_Typedef INRT_Item);
/*Query raw interrupt status,read bit[14:0] of IC_RAW_INRT_STAT reg*/
ITStatus_TypeDef I2C_Get_ITRStat(IC_INTR_Typedef INRT_Item);
/*set RX FIFO Threshold ,set bit[7:0] of IC_RX_TL reg*/
Bit32u I2C_Set_RXThreshold(Bit32u ThresholdValue);
/*set TX FIFO Threshold ,set bit[7:0] of IC_TX_TL reg*/
Bit32u I2C_Set_TXThreshold(Bit32u ThresholdValue);
/*Clear interrupt status,read IC_CLR_INTR or individua interrupt clear reg :
 * !!MST_ON_HOLD/TX_EMPTY/RX_FULL Set and Cleared by Hardware*/
Bit32u I2C_Clear_ITStat(IC_INTR_Typedef INRT_Item);
/*SDA stuck recovery ,set bit3 of IC_ENABLE reg */
Bit32u I2C_SDA_Reco_CMD(State_TypeDef NewState);
/*I2C block transmission enable,set bit2 of IC_ENABLE reg */
Bit32u I2C_Block_Trans_CMD(State_TypeDef NewState);
/*I2C abort enable,set bit1 of IC_ENABLE reg */
Bit32u I2C_Abort_CMD(State_TypeDef NewState);
/*I2C enable,set bit0 of IC_ENABLE reg */
Bit32u I2C_CMD(State_TypeDef NewState);
/*Function43-1:set IC_HS_SPKLEN ,set bit[7:0] of IC_HS_SPKLEN reg*/
Bit32u I2C_Set_HS_SPKLEN(Bit8u SpikeLength);
/*Function42-1:set IC_FS_SPKLEN ,set bit[7:0] of IC_FS_SPKLEN reg*/
Bit32u I2C_Set_FS_SPKLEN(Bit8u SpikeLength);
/*Query I2C status,read bit[11:0] of IC_STATUS reg*/
Bit32u I2C_Get_Stat(IC_STATUS_Typedef StatusItem);
/*set I2C RX and TX SDA hold time ,set bit[23:16] /bit[15:0]of IC_SDA_HOLD reg*/
Bit32u I2C_Set_SDA_Hold(Bit32u RX_Holdtime,Bit32u TX_Holdtime);
/*set SCL_STUCK_AT_LOW_TIMEOUT ,set bit[31:0] of IC_SCL_STUCK_AT_LOW_TIMEOUT reg*/
Bit32u I2C_Set_SCL_STUCK_AT_LOW_TIMEOUT(Bit32u Timeout);
/*set SDA_STUCK_AT_LOW_TIMEOUT ,set bit[31:0] of IC_SDA_STUCK_AT_LOW_TIMEOUT reg*/
Bit32u I2C_Set_SDA_STUCK_AT_LOW_TIMEOUT(Bit32u Timeout);
/*Query CompVersion,read  IC_COMP_VERSION reg*/
Bit32u I2C_Get_CompVer(void);
/*Query Device ID,read  IC_DEVICE_ID reg*/
Bit32u I2C_Get_DeviceID(void);
void I2C_Deactive(void);
/*I2C Master mode work flow*/
void I2C_Config(IC_Mode_Typedef Mode,IC_AddrMode_Typedef AddrMode,IC_SpeedMode_Typedef SpeedMode);
Bit32u I2C_IT_Register(void);
void CK_I2C_Handler(Bit32u irqid);
void I2C_Init_For_EEPROM(void);
/*EEPROM Byte write*/
void EEPROM_Byte_Write(Bit32u Address,Bit8u Data);
/*EEPROM Byte Read*/
void EEPROM_Byte_Read(Bit32u Startaddr,Bit32u Datasize);
void I2C_Test_For_EEPROM(void);
void I2C_Test_For_Uartdedug(Bit32u Readaddr,Bit32u Writeaddr,Bit32u Readnum);


#endif /*__I2C_H__*/
