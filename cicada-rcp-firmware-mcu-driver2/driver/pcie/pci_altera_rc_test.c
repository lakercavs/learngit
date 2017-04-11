#include "pci_tlp.h"
#include "pci_regs.h"
#include "misc.h"
#include "io.h"

/*
 *  mcu side, get bar address from config space 
 *  */
Bit32u altera_get_mcu_bar_addr(Bit64u addr){
	Bit32u val;

	printf("32bit addr:0x%x\n", (Bit32u)addr);
	val = (0x80000000 | ((Bit32u)addr)&0x3fffffff);
	printf("32bit addr val :0x%x\n", (Bit32u)val);
	return val;
}

Bit32u altera_bar_lower_addr(Bit64u addr){
 	return ((((Bit32u)addr)&0xC0000000));
}

Bit32u altera_bar_upper_addr(Bit64u addr){
 	return (Bit32u)(addr >> 32);
}

/*
 * Entry 0
 * Lower bits of Avalon-MM-to-PCI Express address map entry 0.
 * Address space indication for entry 0.
 * (volatile unsigned int *)0x0e001000  =   ((unsigned int)HOST_ADDR_64)&0xC0000000 + 1;
 * Upper bits of Avalon-MM-to-PCI Express address map  entry 0.
 * (volatile unsigned int *)0x0e001004  =   ((unsigned int)(HOST_ADDR_64>>32));
 * mcu side: 0x80000000 + ((unsigned int)HOST_ADDR_64)&0x3fffffff to get HOST_ADDR_64。
 * Entry1 offset is 0x1008 and 0x100c。
 * */
Bit32u altera_rc_set_bar(Bit64u addr){
	struct cfg_status cfg = {0, 0, 0, 0};

	Bit32u ep_bar0_len = 0;
	Bit32u ep_bar0_type = 0;
	Bit32u ep_lower_addr = 0x0;
	Bit32u ep_upper_addr = 0x0;

	Bit32u rc_bar0_len = 0;
	Bit32u rc_bar0_type = 0;
	Bit32u rc_lower_addr = 0x40000000;
	Bit32u rc_upper_addr = 0x0;

#if 0
	/* write low and upper addr to atu */
	ep_lower_addr = altera_bar_lower_addr(addr);
	ep_upper_addr = altera_bar_upper_addr(addr);
	/* add 1 here means transfer 64 bit addr */
	writel((ep_lower_addr | 1), (void *)ALTERA_ENTRY0_LO);
	writel(ep_upper_addr, (void *)ALTERA_ENTRY1_HI);

	printf("ep lower addr:0x%x\n", ep_lower_addr);
	printf("ep upper addr:0x%x\n", ep_upper_addr);
	printf("rc lower addr:0x%x\n", rc_lower_addr);
	printf("rc upper addr:0x%x\n", rc_upper_addr);
#endif

#if 0
	printf("*************rc bar address operation*************************\n");
	printf("Write bar0 0xFFFFFFFF to rc PCI_BASE_ADDRESS_0\n");
	cfg.status = FMT_TYPE_CFG_WD0;
	cfg.reg = PCI_BASE_ADDRESS_0;
	cfg.send_data = 0xFFFFFFFF;
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");
	
	printf("Write bar1 0xFFFFFFFF to rc PCI_BASE_ADDRESS_1\n");
	cfg.status = FMT_TYPE_CFG_WD0;
	cfg.reg = PCI_BASE_ADDRESS_1;
	cfg.send_data = 0xFFFFFFFF;
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");

	printf("Read bar0 from rc PCI_BASE_ADDRESS_0\n");
	cfg.status = FMT_TYPE_CFG_RD0;
	cfg.reg = PCI_BASE_ADDRESS_0;
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");

	rc_bar0_len = (1 << ffs(cfg.recv_data&0xFFFFFFF0));
	printf("rc_bar0_len is 0x%x\n", rc_bar0_len);
	rc_bar0_type = cfg.recv_data&0xF;

	printf("Read bar1 from rc PCI_BASE_ADDRESS_1\n");
	cfg.status = FMT_TYPE_CFG_RD0;
	cfg.reg = PCI_BASE_ADDRESS_1;
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");


	printf("Write bar0 upper to rc PCI_BASE_ADDRESS_0\n");
	cfg.status = FMT_TYPE_CFG_WD0;
	cfg.reg = PCI_BASE_ADDRESS_1;
	cfg.send_data = rc_upper_addr;
	pcie_tlp_send(&cfg);
	printf("\n");

	printf("Write bar0 lower addr to rc PCI_BASE_ADDRESS_0\n");
	cfg.status = FMT_TYPE_CFG_WD0;
	cfg.reg = PCI_BASE_ADDRESS_0;
	cfg.send_data = rc_lower_addr;
	pcie_tlp_send(&cfg);
	printf("\n");

	//printf("rc side 32bit addr:0x%x\n", altera_get_mcu_bar_addr(addr));

	printf("Read bar0 lower from rc PCI_BASE_ADDRESS_0\n");
	cfg.status = FMT_TYPE_CFG_RD0;
	cfg.reg = PCI_BASE_ADDRESS_0;
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");

	printf("Read bar1 upper from rc PCI_BASE_ADDRESS_1\n");
	cfg.status = FMT_TYPE_CFG_RD0;
	cfg.reg = PCI_BASE_ADDRESS_1;
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data); printf("\n");
#endif

#if 0
	/*set pcie rp bar space */
	/* Write 0xFFFFFFFF to ep lower and upper addr */
	printf("Write bar0 0xFFFFFFFF to ep PCI_BASE_ADDRESS_0\n");
	cfg.status = FMT_TYPE_CFG_WD1;
	cfg.reg = PCI_BASE_ADDRESS_0;
	cfg.send_data = 0xFFFFFFFF;
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");
	
	printf("Write bar1 0xFFFFFFFF to ep PCI_BASE_ADDRESS_1\n");
	cfg.status = FMT_TYPE_CFG_WD1;
	cfg.reg = PCI_BASE_ADDRESS_1;
	cfg.send_data = 0xFFFFFFFF;
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");


	/* Read back ep lower and upper addr */
	printf("Read bar0 from ep PCI_BASE_ADDRESS_0\n");
	cfg.status = FMT_TYPE_CFG_RD1;
	cfg.reg = PCI_BASE_ADDRESS_0;
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");

	ep_bar0_len = (1 << ffs(cfg.recv_data&0xFFFFFFF0));
	printf("ep_bar0_len is 0x%x\n", ep_bar0_len);
	ep_bar0_type = cfg.recv_data&0xF;

	printf("Read bar1 from ep PCI_BASE_ADDRESS_1\n");
	cfg.status = FMT_TYPE_CFG_RD1;
	cfg.reg = PCI_BASE_ADDRESS_1;
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");


	/* Write ep upper and lower addr */
	printf("Write bar0 upper to ep PCI_BASE_ADDRESS_0\n");
	cfg.status = FMT_TYPE_CFG_WD1;
	cfg.reg = PCI_BASE_ADDRESS_1;
	cfg.send_data = ep_upper_addr;
	pcie_tlp_send(&cfg);
	printf("\n");
#endif

	printf("Write bar0 lower addr to ep PCI_BASE_ADDRESS_0\n");
	cfg.status = FMT_TYPE_CFG_WD1;
	cfg.reg = PCI_BASE_ADDRESS_0;
	//cfg.send_data = ep_lower_addr;
	cfg.send_data = 0x400;
	pcie_tlp_send(&cfg);
	printf("\n");

#if 0
	printf("Read bar0 lower from ep PCI_BASE_ADDRESS_0\n");
	cfg.status = FMT_TYPE_CFG_RD1;
	cfg.reg = PCI_BASE_ADDRESS_0;
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");
#endif

	/* Write ep command register */
	printf("Write ep command register\n");
	cfg.status = FMT_TYPE_CFG_WD1;
	cfg.reg = PCI_COMMAND;
	cfg.send_data = (PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
	pcie_tlp_send(&cfg);
	printf("\n");

#if 0
	printf("Read ep command register\n");
	cfg.status = FMT_TYPE_CFG_RD1;
	cfg.reg = PCI_COMMAND; 
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");
#endif

	printf("Write bar0 lower addr to rc PCI_BASE_ADDRESS_0\n");
	cfg.status = FMT_TYPE_CFG_WD0;
	cfg.reg = PCI_BASE_ADDRESS_0;
	cfg.send_data = 0x100400;
	pcie_tlp_send(&cfg);
	printf("\n");

#if 0
	printf("Read bar0 lower from rc PCI_BASE_ADDRESS_0\n");
	cfg.status = FMT_TYPE_CFG_RD0;
	cfg.reg = PCI_BASE_ADDRESS_0;
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");
#endif

	/* Write rc command register */
	printf("Write rc command register\n");
	cfg.status = FMT_TYPE_CFG_WD0;
	cfg.reg = PCI_COMMAND;
	cfg.send_data = (PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
	pcie_tlp_send(&cfg);
	printf("\n");

#if 0
	printf("Read rc command register\n");
	cfg.status = FMT_TYPE_CFG_RD0;
	cfg.reg = PCI_COMMAND; 
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");
#endif

}

/*
 *  work flow
 *  1. Configure the rootport bus number register so that we can pass CFG1 TLPs downstream to configure the endpoint.
 *  2. Configure the endpoint BAR0 register with an address we want to accept memory read and write TLPs at.
 *  3. Configure the endpoint command register to enable the memory space decoder and enable it to bus master so it can initiate requests.
 *  4. Configure the rootport BAR0 register with an address we want to accept memory read and write TLPs at.
 *  5. Configure the rootport command register to enable the memory space decoder and enable it to bus master so it can initiate requests.
*/

/* R/W ep using cfg type 1
 * R/W rc using cfg type 0 */
Bit32u pci_altera_rc_test(){
	struct cfg_status cfg = {0, 0, 0, 0};
	Bit32u val;
	Bit64u addr = 0x50000000;

	while(readl((void *)ALTERA_STATUS)!=0x0F);

	/* Write bus number to rc */
	printf("Write bus number to rc\n");
	cfg.status = FMT_TYPE_CFG_WD0;
	cfg.reg = PCI_RC_BUS_NUMBER;
	cfg.send_data = 0x10100;
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");

	/* Read vendor id register from ep
	 * Should be 4 byte aligned  */
	printf("Read vendor id register from ep\n");
	cfg.status = FMT_TYPE_CFG_RD1;
	cfg.reg = PCI_VENDOR_ID;
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");

	/* Read vendor id register from rc
	 * Should be 4 byte aligned  */
	printf("Read vendor id register from rc\n");
	cfg.status = FMT_TYPE_CFG_RD0;
	cfg.reg = PCI_VENDOR_ID;
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");

	//printf("64bit addr lower %x\n", addr);
	//printf("64bit addr upper %x\n", addr>>32);
	altera_rc_set_bar(addr);

#if 0
	/* Read capabilities from ep  */
	printf("Read capabilities list from ep\n");
	cfg.status = FMT_TYPE_CFG_RD1;
	cfg.reg = PCI_CAPABILITY_LIST;
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");
#endif


	/* memory read and writ */
	/* meory write root to end */
	printf("memory write root to end\n");
	cfg.status = FMT_TYPE_MEM_WD_32;
	cfg.reg = 0x400;
	cfg.send_data= 0xcafebeef;
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");

	/* meory read root to end */
	printf("memory read root to end\n");
	cfg.status = FMT_TYPE_MEM_WD_32;
	cfg.reg = 0x400;
	pcie_tlp_send(&cfg);
	printf("cofing response data. 0x%x\n", cfg.recv_data);
	printf("\n");

	return 0;
}
