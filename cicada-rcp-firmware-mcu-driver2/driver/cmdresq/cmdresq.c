#include "cmdresq.h"
#include "intc.h"
#include "io.h"

struct cmdresq_t* p_cmdresq;
static CKStruct_IRQHandler irqhandler;  /*the struct must be put outside the function*/

Bit32u cmdresq_msg_recv(cmdresq_mcu_number mcu_number, struct comdresq_msg_info* msg_info){
	Bit32u payload;
	Bit32u val;
	Bit32u pending_number;

	val = reg_readl(p_cmdresq, irq);

	switch(mcu_number){
		case cmdresq_mcu1:
			pending_number = (val >> CMDRESQ_IRQ_LEFT_MCU1_BIT)&CMDRESQ_IRQ_LEFT_MCU_MASK;
		   break;
		case cmdresq_mcu2:
			pending_number = (val >> CMDRESQ_IRQ_LEFT_MCU2_BIT)&CMDRESQ_IRQ_LEFT_MCU_MASK;
		   break;	
		case cmdresq_mcu3:
			pending_number = (val >> CMDRESQ_IRQ_LEFT_MCU3_BIT)&CMDRESQ_IRQ_LEFT_MCU_MASK;
		   break;	
		default:
		   break;
	}

	if(pending_number){
		val = reg_readl(p_cmdresq, cmd);
		payload = val&CMDRESQ_PAYLOAD_MASK;
		msg_info->src =  (val>>CMDRESQ_CMD_SOURCE)&CMDRESQ_ADDR_MASK;
	}

	msg_info->payload = payload;
	msg_info->dst = mcu_number; 

	return 0;
}

Bit32u cmdresq_msg_send(struct comdresq_msg_info* msg_info){
	Bit32u val = 0;

	/* set source addr */
	val |= (msg_info->src << CMDRESQ_PD_DESTINATION);
	val |= (msg_info->payload&CMDRESQ_PAYLOAD_MASK);
	reg_writel(val, p_cmdresq, cmd);
}

void CK_cmdresq_Handler(Bit32u irqid)
{
	Bit32u val;
	val = reg_readl(p_cmdresq, irq); 

	/* deal with irq */

	/* clear irq */
	reg_direct_writel(val, p_cmdresq, irq);
}

Bit32u cmdresq_init(){
	p_cmdresq = (struct cmdresq_t*)CMDRESQ_BASE_ADDR;

	/* disable irq, not ready  */
	reg_clearl(CMDRESQ_CTRL_IRQENABLE, p_cmdresq, ctrl);
	/* clear interrupt flag */
	reg_clearl(0, p_cmdresq, irq);

	/* register cmdresq irq */
	CK_INTC_RequestIrq(CK_INTC_CMDRESQ, CK_cmdresq_Handler);

	/* enable irq */
	reg_writel(CMDRESQ_CTRL_IRQENABLE, p_cmdresq, ctrl);
	return 0;
}
