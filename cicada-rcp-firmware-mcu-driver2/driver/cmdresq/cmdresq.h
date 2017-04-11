#ifndef __CMDRESQ_H__
#define __CMDRESQ_H__
#include "io.h"
#include "ck810.h"

#define CMDRESQ_BASE_ADDR 0x18000000 /* Modfiied to ck810.h cmdresq address */

#define CMDRESQ_PD 		(CMDRESQ_BASE_ADDR + 0x00) 	/* priority and destinaton */
#define CMDRESQ_CMD 	(CMDRESQ_BASE_ADDR + 0x04)  /* Cmd context and source mcu */
#define CMDRESQ_IRQ 	(CMDRESQ_BASE_ADDR + 0x08)  /* interrupt */
#define CMDRESQ_CTRL 	(CMDRESQ_BASE_ADDR + 0x0c) 	/* Contril Bit */

#define CMDRESQ_PD_PRIORITY(x) 	((x) << 30) 		/* priority */
#define CMDRESQ_PD_DESTINATION 	(28) 	/* destination of request */
#define CMDRESQ_CMD_SOURCE 		(28) 	/* source of adddress */
#define CMDRESQ_PAYLOAD_MASK (0x0FFFFFFFF) 				/* bit[27:0] payload */
#define CMDRESQ_ADDR_MASK (3) 				/* bit[29:28] mask */

/* 问题： mcu有cmdresq的中断
 *        每一个中断的状态如何关闭？
 *        full的状态是否通知到其它的mcu */
#define CMDRESQ_IRQ_PENDING 		(1 << 0) 		/* request in the queue */
#define CMDRESQ_IRQ_EMPTY_MCU1 	(1 << 1) 			/* MCU1 Empty  */
#define CMDRESQ_IRQ_EMPTY_MCU2 	(1 << 2) 			/* MCU2 Empty  */
#define CMDRESQ_IRQ_EMPTY_MCU3 	(1 << 3) 			/* MCU3 Empty  */
#define CMDRESQ_IRQ_NO_RERUEST 	(1 << 4) 			/* No request left in the queue  */
#define CMDRESQ_IRQ_FULL_MCU1 		(1 << 5) 		/* MCU1 Full */
#define CMDRESQ_IRQ_FULL_MCU2 		(1 << 6) 		/* MCU2 Full */
#define CMDRESQ_IRQ_FULL_MCU3 		(1 << 7) 		/* MCU3 Full */
#define CMDRESQ_IRQ_LEFT_MCU1_BIT 	(16) 			/* Number of pending command in MCU1  */
#define CMDRESQ_IRQ_LEFT_MCU2_BIT 	(20) 			/* Number of pending command in MCU2  */
#define CMDRESQ_IRQ_LEFT_MCU3_BIT 	(24) 			/* Number of pending command in MCU3  */
#define CMDRESQ_IRQ_LEFT_MCU_MASK 	(0xF)
#define CMDRESQ_CTRL_IRQENABLE 	(1 << 0)  			/* Interrupt enable control bit */
#define CMDRESQ_CTRL_MCUREADY 		(1 << 1)  		/* MCU is ready to accept request */

struct cmdresq_t{
	Bit32u prio_dest;
	Bit32u cmd;
	Bit32u irq;
	Bit32u ctrl;
};

struct comdresq_msg_info{
	Bit8u src;
	Bit8u dst;
	Bit32u payload;
};

typedef enum{
	cmdresq_mcu1 = 1,
	cmdresq_mcu2 = 2,
	cmdresq_mcu3 = 3
}cmdresq_mcu_number;

typedef enum{
	cmdresq_destination_broadcast = 0,	
	cmdresq_destination_to_mcu1 = 1,	
	cmdresq_destination_to_mcu2 = 2,	
	cmdresq_destination_to_mcu3 = 3
}cmdresq_destination_typedef;

#endif /* __CMDRESQ_H__ */
