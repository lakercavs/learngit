#include "misc.h"
#include "pci.h"
#include "pci_regs.h"
#include "pci_vendor_reg.h"
#include "pci_cmdq.h"

#define PCI_CONFIG_SPACE_BASE_ADDR CK_PCIE_EP_ADDRBASE

/* PCI operation address */
#define PORT_PCI_CMD           0x0cf8
#define PORT_PCI_REBOOT        0x0cf9
#define PORT_PCI_DATA          0x0cfc

#define PCI_ROM_SLOT 6
#define PCI_NUM_REGIONS 7
#define PCI_BRIDGE_NUM_REGIONS 2

/* pcie warm reset and code reset */
Bit32u pci_reset(){

	return 0;
}

/* pci Built-In Self Test */
Bit32u pci_bist(void){
	Bit8u val;
	val = readb((void *)(PCIE_BASEADDR_F0_LEA_CFG_SPACE + PCI_BIST));
	if((val&PCI_BIST_CAPABLE) == PCI_BIST_CAPABLE){
		/* Devie support BIST */
		writeb(val | PCI_BIST_START, (void *)(PCIE_BASEADDR_F0_LEA_CFG_SPACE + PCI_BIST));

		/*sleep two seconds */

		val = readb((void *)(PCIE_BASEADDR_F0_LEA_CFG_SPACE + PCI_BIST)) & PCI_BIST_CODE_MASK;
		if(val == 0){
			printf("Device test passed.\n");
			return 0;
		}
		else{
			printf("Device test failture.\n");
			return -1;
		}
	}
	else{
		/* Devie don't support BIST */
		printf("Device don't support BIST.\n");
		return 0;
	}
}

Bit32u pci_ib_read_taraddr(Bit8u number){
	Bit32u target_addr;

	target_addr = PCIE_BASEADDR_ATU_INBOUND + (number << 8) + PCIE_BASEADDR_IB_TRGT_LADDR;
	return readl((void*)target_addr);
}

/*
 * Final interface.
 * 64 bit---> 32 bit
 * Bit32u pci_ib_memory_rw(Bit8u type, Bit64u start_addr, Bit32u len);
 * 32 bit---> 64bit
 * Bit64u pci_ob_memory_rw(Bit8u type, Bit32u start_addr, Bit32u len);
 * */
/* 
 * Inbound Bar mode, operation. get transfer addr
 * We use region number 0, 1, 2 for function 0 three bars mapping
 * Only remap function0 here, the function number is 0
 * @number means function number, also means region number
 * @return value: 32 bit target address
 * */
Bit32u pci_get_ib_bar_match(Bit8u number){
	Bit32u region_addr;
	Bit32u value = 0; 

	number &= 0x7;
	region_addr = PCIE_BASEADDR_ATU_INBOUND + (number << 8) + PCIE_BASEADDR_IB_CTRL;
	value |= PCIE_IB_CTRL_MODE(PCIE_IB_CTRL_MODE_BAR);
	value |= PCIE_IB_CTRL_BAR_NUM(number);
	value |= PCIE_IB_CTRL_FUNC_NUM(PCIE_IB_CTRL_FUNC_NUM_0);
	writel(value, (void *)region_addr);

	/* enable translation */
	value |= PCIE_IB_CTRL_ENABLE;
	writel(value, (void *)region_addr);

	return pci_ib_read_taraddr(number);
}

/*
 * Inbound Bar mode, address translation
 * @return address: 32 bit target address
 * */
Bit32u pci_ib_getaddr_addr_match(Bit64u start_addr, Bit64u end_addr, Bit8u region_number){
	Bit32u addr;
	Bit32u value = 0; 

	/* start address low addr */
	addr = PCIE_BASEADDR_ATU_INBOUND + (region_number << 8) + PCIE_BASEADDR_IB_ST_LADDR;
	writel(start_addr & 0xFFFF, (void *)addr);

	/* start address high addr */
	addr = PCIE_BASEADDR_ATU_INBOUND + (region_number << 8) + PCIE_BASEADDR_IB_ST_UADDR;
	writel(start_addr >> 32, (void *)addr);

	/* end address low addr */
	addr = PCIE_BASEADDR_ATU_INBOUND + (region_number << 8) + PCIE_BASEADDR_IB_END_LADDR;
	writel(start_addr & 0xFFFF, (void *)addr);

	/* end address high addr */
	addr = PCIE_BASEADDR_ATU_INBOUND + (region_number << 8) + PCIE_BASEADDR_IB_END_UADDR;
	writel(start_addr >> 32, (void *)addr);

	/* enable translation */
	addr = PCIE_BASEADDR_ATU_INBOUND + (region_number << 8) + PCIE_BASEADDR_IB_CTRL;
	value |= PCIE_IB_CTRL_ENABLE;
	writel(value, (void *)addr);

	return pci_ib_read_taraddr(region_number);
}


Bit32u pci_ob_read_taraddr(Bit8u number){
	Bit32u target_addr;
	Bit64u value;

	target_addr = PCIE_BASEADDR_ATU_OUTBOUND + (number << 8) + PCIE_BASEADDR_OB_TRGT_LADDR;
	value = readl((void*)target_addr);

	target_addr = PCIE_BASEADDR_ATU_OUTBOUND + (number << 8) + PCIE_BASEADDR_OB_TRGT_UADDR;
	value |= ((Bit64u)readl((void*)target_addr) << 32);

	return value;
}

/*
 * This is a function called by data rw
 * */
Bit64u pci_ob_transfer_addr(Bit32u start_addr, Bit32u end_addr, Bit8u region_number, Bit32u ctrl_value){
	Bit32u addr;
	/* start address low addr */
	addr = PCIE_BASEADDR_ATU_OUTBOUND + (region_number << 8) + PCIE_BASEADDR_OB_ST_LADDR;
	writel(start_addr & 0xFFFF, (void *)addr);

	/* start address high addr */
	//addr = PCIE_BASEADDR_ATU_OUTBOUND + (region_number << 8) + PCIE_BASEADDR_OB_ST_UADDR;
	//writel(start_addr, (void *)addr);

	/* end address low addr */
	addr = PCIE_BASEADDR_ATU_OUTBOUND + (region_number << 8) + PCIE_BASEADDR_OB_END_LADDR;
	writel(start_addr & 0xFFFF, (void *)addr);

	/* end address high addr */
	//addr = PCIE_BASEADDR_ATU_OUTBOUND + (region_number << 8) + PCIE_BASEADDR_OB_END_UADDR;
	//writel(((Bit32u)start_addr >> 32), (void *)addr);

	/* enable translation */
	addr = PCIE_BASEADDR_ATU_OUTBOUND + (region_number << 8) + PCIE_BASEADDR_OB_CTRL;
	ctrl_value |= PCIE_IB_CTRL_ENABLE;
	writel(ctrl_value, (void *)addr);

	return pci_ob_read_taraddr(region_number);
}

/* 
 * Outbound operation, memory read/write 
 * @type 0:read, 1:write, the read and write is selected by software
 * @func_sel, 0:from internal, 1:from external
 * @func_number This bit if valid if @func_sel==1, this number is used for Group1, Group0, Group2
 * */
Bit64u pci_ob_memory_rw(Bit8u type, Bit8u func_sel, Bit8u func_number, Bit32u start_addr, Bit32u end_addr,   Bit8u region_number){
	Bit32u addr;
	Bit32u ctrl_value = 0;  /* value for ctrl register */
	Bit32u misc_value = 0; 

	ctrl_value |= PCIE_OB_CTRL_TRANS_TYPE(PCIE_OB_CTRL_TRANS_TYPE_MEM);
	ctrl_value |= PCIE_OB_CTRL_FUNC_SEL(func_sel);
	addr = PCIE_BASEADDR_ATU_OUTBOUND + (region_number << 8) + PCIE_BASEADDR_OB_CTRL;
	writel(ctrl_value, (void *)addr);

	if(func_sel == 1)
		switch(func_number){
			case 0:
				misc_value |= PCIE_OB_WR_MSIC_TLP_FUNC(PCIE_OB_WR_MSIC_TLP_FUNC_GRP1);  
				break;
			case 1:
				misc_value |= PCIE_OB_WR_MSIC_TLP_FUNC(PCIE_OB_WR_MSIC_TLP_FUNC_GRP0);  
				break;
			case 2:
				misc_value |= PCIE_OB_WR_MSIC_TLP_FUNC(PCIE_OB_WR_MSIC_TLP_FUNC_GRP2);  
				break;
		}

	misc_value |= PCIE_OB_RD_MSIC_TLP_TYPE(PCIE_TLP_HEADER_MSG_TYPE_MRD);

	switch(type){
		case 0: 	/* read */
			addr = PCIE_BASEADDR_ATU_OUTBOUND + (region_number << 8) + PCIE_BASEADDR_OB_RD_MSIC;
			writel(misc_value, (void *)addr);
			break;
		case 1: 	/* write */
			addr = PCIE_BASEADDR_ATU_OUTBOUND + (region_number << 8) + PCIE_BASEADDR_OB_WR_MSIC;
			writel(misc_value, (void *)addr);
			break;
		default:
			break;
	}

	return pci_ob_transfer_addr(start_addr, end_addr, region_number, ctrl_value);
}

/* 
 * Outbound operation, configuation read/write 
 * @type 0:read, 1:write, the read and write is selected by software, But we only use configuration write here.
 * @func_sel, 0:from internal, 1:from external
 * @func_number This bit if valid if @func_sel==1, this number is used for Group1, Group0, Group2
 * */
Bit64u pci_ob_cfg_rw(Bit8u type, Bit8u func_sel, Bit8u fun_number, Bit8u bus_numer, Bit8u device_number, Bit8u function_number,  Bit32u start_addr, Bit32u end_addr,   Bit8u region_number){
	Bit32u addr;
	Bit32u ctrl_value = 0;  /* value for ctrl register */
	Bit32u misc_value = 0;
	Bit32u bdf_value = 0; 	/* Bus devie function value */

	ctrl_value |= PCIE_OB_CTRL_TRANS_TYPE(PCIE_OB_CTRL_TRANS_TYPE_CFG);
	ctrl_value |= PCIE_OB_CTRL_FUNC_SEL(func_sel);
	addr = PCIE_BASEADDR_ATU_OUTBOUND + (region_number << 8) + PCIE_BASEADDR_OB_CTRL;
	writel(ctrl_value, (void *)addr);

	if(func_sel == 1)
		switch(function_number){
			case 0:
				misc_value |= PCIE_OB_WR_MSIC_TLP_FUNC(PCIE_OB_WR_MSIC_TLP_FUNC_GRP1);  
				break;
			case 1:
				misc_value |= PCIE_OB_WR_MSIC_TLP_FUNC(PCIE_OB_WR_MSIC_TLP_FUNC_GRP0);  
				break;
			case 2:
				misc_value |= PCIE_OB_WR_MSIC_TLP_FUNC(PCIE_OB_WR_MSIC_TLP_FUNC_GRP2);  
				break;

		}

	misc_value |= PCIE_OB_RD_MSIC_TLP_TYPE(PCIE_OB_CTRL_TRANS_TYPE_CFG);

	switch(type){
		case 0: 	/* read */
			addr = PCIE_BASEADDR_ATU_OUTBOUND + (region_number << 8) + PCIE_BASEADDR_OB_RD_MSIC;
			writel(misc_value, (void *)addr);
			break;
		case 1: 	/* write */
			addr = PCIE_BASEADDR_ATU_OUTBOUND + (region_number << 8) + PCIE_BASEADDR_OB_WR_MSIC;
			writel(misc_value, (void *)addr);

			bdf_value = PCIE_OB_CFG_FUNC_NUM(bus_numer) + PCIE_OB_CFG_DEV_NUM(device_number) + PCIE_OB_CFG_FUNC_NUM(function_number);
			addr = PCIE_BASEADDR_ATU_OUTBOUND + (region_number << 8) + PCIE_BASEADDR_OB_CFG_ADDR;
			writel(bdf_value, (void *)addr);
			break;
		default:
			break;
	}


	return pci_ob_transfer_addr(start_addr, end_addr, region_number, ctrl_value);
}

/* 
 * Outbound operation, message write.
 * @type 0:read, 1:write, the read and write is selected by software
 * @func_sel, 0:from internal, 1:from external
 * @func_number This bit if valid if @func_sel==1, this number is used for Group1, Group0, Group2
 * @dw3: TLP DW3 content.
 * @dw4: TLP DW4 content.
 * */
Bit64u pci_ob_message_write(Bit8u func_sel, Bit8u func_number, Bit32u start_addr, Bit32u end_addr, Bit8u region_number, Bit32u dw3, Bit32u dw4, Bit8u msg_code){
	Bit32u addr;
	Bit32u ctrl_value = 0;  /* value for ctrl register */
	Bit32u misc_value = 0; 

	ctrl_value |= PCIE_OB_CTRL_TRANS_TYPE(PCIE_OB_CTRL_TRANS_TYPE_MSG);
	ctrl_value |= PCIE_OB_CTRL_FUNC_SEL(func_sel);
	addr = PCIE_BASEADDR_ATU_OUTBOUND + (region_number << 8) + PCIE_BASEADDR_OB_CTRL;
	writel(ctrl_value, (void *)addr);

	if(func_sel == 1)
		switch(func_number){
			case 0:
				misc_value |= PCIE_OB_WR_MSIC_TLP_FUNC(PCIE_OB_WR_MSIC_TLP_FUNC_GRP1);  
				break;
			case 1:
				misc_value |= PCIE_OB_WR_MSIC_TLP_FUNC(PCIE_OB_WR_MSIC_TLP_FUNC_GRP0);  
				break;
			case 2:
				misc_value |= PCIE_OB_WR_MSIC_TLP_FUNC(PCIE_OB_WR_MSIC_TLP_FUNC_GRP2);  
				break;

		}

	misc_value |= PCIE_OB_RD_MSIC_TLP_TYPE(PCIE_TLP_HEADER_MSG_TYPE_MRD);
	misc_value |= PCIE_OB_WR_MSIC_TLP_MSG(msg_code);

	addr = PCIE_BASEADDR_ATU_OUTBOUND + (region_number << 8) + PCIE_BASEADDR_OB_WR_MSIC;
	writel(misc_value, (void *)addr);

	addr = PCIE_BASEADDR_ATU_OUTBOUND + (region_number << 8) + PCIE_BASEADDR_OB_WR_HDR_DW3;
	writel(dw3, (void *)addr);

	addr = PCIE_BASEADDR_ATU_OUTBOUND + (region_number << 8) + PCIE_BASEADDR_OB_WR_HDR_DW4;
	writel(dw4, (void *)addr);


	return pci_ob_transfer_addr(start_addr, end_addr, region_number, ctrl_value);
}

/*
 * Find Capability pointer
 * @cap_id:
 * 			PCI_CAP_ID_PM
 * 			PCI_CAP_ID_MSI
 * 			PCI_CAP_ID_MSIX
 * 			PCI_CAP_ID_EXP	
 * @return value: Capability pointer address, or 0
 * */
Bit16u pci_find_capbility_pointer(Bit8u cap_id){
	Bit8u i;

	for(i=0; i< PCIE_CAP_SIZE; i++){
		if(pcie_all_cap[i].cap_id == cap_id)
			return pcie_all_cap[i].cap_addr;
	}

	return 0;
}

/*
 * As EP. we use cmdq interrupt for msi/msix, pm ,pice capbilities receive
 * */

/*
 * init MSI capbility
 * For EP, we do nothing here
 * For RC, we config the msi related address, and use ATU to tranlate address.
 * 	we need alloc a meory for msg data,and use ATU.
 * */
Bit16u pci_rc_cap_msi_init(Bit8u cap_addr){
	/* 1. init msi control register here 
	 * Address translation here
	 * */


	return 0;
}

/*
 * init MSI-X capbility
 * For EP, we do nothing here
 * For RC, we config the msi related address, and use ATU to tranlate address.
 * we need alloc a meory for msg data,and use ATU.
 * */
Bit16u pci_rc_cap_msix_init(Bit8u cap_addr){
	return 0;
}

/*
 * init PM capbility
 * Not initilize here.
 * We get interrupt from hardware, and dealwith it in irqhandler
 * */
Bit16u pci_rc_cap_pm_init(Bit8u cap_addr){
	return 0;
}

/*
 * init PCI Express capbility
 * Not initilize here.
 * We get interrupt from hardware, and dealwith it in irqhandler
 * */
Bit16u pci_rc_cap_pcie_init(Bit8u cap_addr){
	return 0;
}

Bit16u pci_find_capabilities(){
	Bit8u cap_id;
	Bit32u addr; 	/* capbility list entry */
	Bit32u pcap; 	/* capability pointer */
	Bit8u i;

	addr = PCIE_BASEADDR_F0_LEA_CFG_SPACE + PCI_CAPABILITY_LIST; 	/* pcie capbility list pointer */

	for(i=0; i< PCIE_CAP_SIZE; i++){
		pcap = readl((void *)addr);
		if(pcap == 0){
			printf("Finished reading capabilities\n");
			return 0;
		}

		pcie_all_cap[i].cap_addr = pcap;
		pcie_all_cap[i].cap_id = readl((void *)pcap);
		addr = pcap + 8;

		/* Init capbility */
		switch(pcie_all_cap[i].cap_id){
			case PCI_CAP_ID_PM: 		/* Power Management */
				//pci_rc_cap_pm_init(pcie_all_cap[i].cap_addr);
				break;
			case PCI_CAP_ID_MSI: 		/* MSI */
				//pci_rc_cap_msi_init(pcie_all_cap[i].cap_addr);
				break;
			case PCI_CAP_ID_MSIX: 		/* MSI-X */
				//pci_rc_cap_msix_init(pcie_all_cap[i].cap_addr);
				break;
			case PCI_CAP_ID_EXP: 		/* PCI Express */
				//pci_rc_cap_pcie_init(pcie_all_cap[i].cap_addr);
				break;
			default:
				break;
		}
	}

	return 0;

}

/* PCI3.0 Page:227 
 * we use set bar when we operate PCIE rc 
 * */
/*
void pci_set_bar(Bit64u bar){
	Bit32u val_hi, val_lo;
	pcie_writel(rcp_space, 0xFFFFFFFF, bar0_hi);
	val_hi = pcie_readl(rcp_space, bar0_hi);
	pcie_writel(rcp_space, 0xFFFFFFFF, bar0_lo);
	val_lo = pcie_readl(rcp_space, bar0_lo);
	if((val_hi & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_IO){
		val_hi = (val_hi&PCI_BASE_ADDRESS_IO_MASK) | (bar >>32);
		pcie_writel(rcp_space, val_hi, bar0_hi);
		val_lo = (val_hi&PCI_BASE_ADDRESS_IO_MASK) | (bar >>32);
		pcie_writel(rcp_space, val_lo, bar0_lo);
	}
	if((val_hi & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_MEMORY){
		val_hi = (val_hi&PCI_BASE_ADDRESS_MEM_MASK) | (bar >>32);
		pcie_writel(rcp_space, val_hi, bar0_hi);
		val_lo = (val_hi&PCI_BASE_ADDRESS_MEM_MASK) | (bar >>32);
		pcie_writel(rcp_space, val_lo, bar0_hi);
	}
}
*/

Bit32u pcie_verify_id(){
	/*
	Bit32u val;
	val = readl((void *)(PCIE_BASEADDR_F0_LEA_CFG_SPACE));
	if((val&0xFFFF) != CKCORE_VENDOR_ID){
		printf("wrong vendor id:0x%o8x\n", val&0xFFFF);
		return -1;
	}
	if(((val >> 16)&0xFFFF) != CKCORE_DEVICE_ID){
		printf("wrong device id:0x%o8x\n", (val >> 16)&0xFFFF);
		return -1;
	}
	*/

	return 0;
}

/*
 * If we are rc, we need init configspce.
 * */
Bit32u pci_rc_init_configspace(){
	/* Write command and BAR addr */

	return 0;
}

Bit32u pci_ep_init(){
	Bit32u val;
	Bit32u addr_bar0; /* GRP2 PCIe Command Response Queue */
	Bit32u addr_bar1; /* 4MB MEM POOL access range for GRP2 SOC */
	Bit32u addr_bar2; /* User defined exernal application register and MSI-X interrupt register for GRP2*/

	/* reset pcie */
	if (pci_reset() != 0) { 
		printf("reset pcie error.\n");
		return -1;
	}

	/* pcie self test */
	val = pci_bist();
	if(val < 0){
		printf("pcie not prepared now.\n");
		return -1;
	}

	/* verify vendor id and deivce id */
	val = pcie_verify_id();
	if(val < 0){
		printf("pcie vendor id or device id error.\n");
		return -1;
	}

	/* get msi-x capability address */
	//get_msix_cap_addr(bar0);

	addr_bar0 = pci_get_ib_bar_match(PCIE_IB_CTRL_BAR_NUM_BAR1_BAR0);
	addr_bar1 = pci_get_ib_bar_match(PCIE_IB_CTRL_BAR_NUM_BAR3_BAR2);
	addr_bar2 = pci_get_ib_bar_match(PCIE_IB_CTRL_BAR_NUM_BAR5_BAR4);

	/* msi x software configuration */
#if 0
	val = pcie_capability_msi();
	if(val !=0 ){
		printf("msi-x is not supported.\n");
		return -1;
	}
#endif

	/* get mis-x table offset 
	 * Table BIR for MSI-X get the bar region */

	/* get mis-x PBA offset 
	 * PBA BIR for MSI-X get the bar region */

	/* get mis-x PBA offset 
	 * PBA BIR for MSI-X get the bar region */

	/* set 
	 * message address, message data, vector control, PBA 
	 * for Table Entries */

	return 0;
}

Bit32u pci_rc_init(){
	pci_rc_init_configspace();
}
