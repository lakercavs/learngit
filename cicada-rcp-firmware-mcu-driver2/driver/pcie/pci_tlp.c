#include "pci_tlp.h"
#include "include/io.h"
#include "include/misc.h"
#include "pci_regs.h"

Bit32u ffs(Bit32u word)  
{  
	Bit32u num = 0;  
  
	if ((word & 0xffff) == 0) {  
		num += 16;  
		word >>= 16;  
	}  
	if ((word & 0xff) == 0) {  
		num += 8;  
		word >>= 8;  
	}  
	if ((word & 0xf) == 0) {  
		num += 4;  
		word >>= 4;  
	}  
	if ((word & 0x3) == 0) {  
		num += 2;  
		word >>= 2;  
	}  
	if ((word & 0x1) == 0)  
		num += 1;  
	return num;  
}

Bit32u pcie_tlp_recv(struct cfg_status *cfg){
	Bit32u val;
	Bit32u rel;
	Bit8u type;
	Bit8u i;
	struct tlp_header_total_3dw dw={0};
	cfg->recv_data = 0;

	/* while rx data is ready */
	//while((rel = readl((void *)ALTERA_RP_RX_CTRL)&ALTERA_RP_RX_CTRL_SOP) != ALTERA_RP_RX_CTRL_SOP);
	
	while((readl((void *)ALTERA_RP_RX_CTRL)&ALTERA_RP_RX_CTRL_EOP) !=  ALTERA_RP_RX_CTRL_EOP){
		//printf("link status:0x%x\n", readl((void *)ALTERA_STATUS));
		dw.dw1 = readl((void *)ALTERA_RP_RX_REG0);
		dw.dw2 = readl((void *)ALTERA_RP_RX_REG1);
		//printf("recv dw.dw1:0x%x dw.dw2:0x%x\n",dw.dw1, dw.dw2);
		//printf("rx ctrl regiser status:0x%x\n", readl((void *)ALTERA_RP_RX_CTRL));
	}

	cfg->recv_data = readl((void *)ALTERA_RP_RX_REG0);
	printf("recv data0:0x%x, data1:0x%x\n", cfg->recv_data, readl((void *)ALTERA_RP_RX_REG1));

#if 0
	type = ((dw.dw1>>24)&0xFF);
	if (type == FMT_TYPE_CFG_CPLD){
		cfg->recv_data = readl((void *)ALTERA_RP_RX_REG0);
		//readl((void *)ALTERA_RP_RX_REG1);
		printf("recv data0:0x%x, data1:0x%x\n", cfg->recv_data, readl((void *)ALTERA_RP_RX_REG1));
	}
	printf("ctrl regiser status:0x%x\n", readl((void *)ALTERA_RP_RX_CTRL));
#endif
	
	return 0;
}

Bit32u pcie_tlp_send(struct cfg_status *cfg){
	struct tlp_header_total_3dw dw = {0};
	Bit8u type = 0;
	Bit32u val;
	Bit32u rel;

	dw.dw1 = (cfg->status << 24) | 1;
	dw.dw2 = 0x1f0f;
	dw.dw3 = cfg->reg;

	while(readl((void *)ALTERA_STATUS)!=0x0F);

	writel(dw.dw1, (void *)ALTERA_RP_TX_REG0);
	writel(dw.dw2, (void *)ALTERA_RP_TX_REG1);
	writel(ALTERA_RP_TX_CTRL_SOP, (void *)ALTERA_RP_TX_CTRL);
	printf("send dw.dw1:0x%x dw.dw2:0x%x\n", dw.dw1, dw.dw2);
	type = ((dw.dw1>>24)&0xFF);

	switch (type){
		case FMT_TYPE_MEM_WD_32:
			/* config read, 3dw no data */
			writel(dw.dw3, (void *)ALTERA_RP_TX_REG0);
			writel(0x0, (void *)ALTERA_RP_TX_REG1);
			writel(0x00, (void *)ALTERA_RP_TX_CTRL);
			printf("send dw.dw3:0x%x dw.dw4:0x%x\n",dw.dw3, 0);

			writel(cfg->send_data, (void *)ALTERA_RP_TX_REG0);
			writel(0x0, (void *)ALTERA_RP_TX_REG1);
			writel(ALTERA_RP_TX_CTRL_EOP, (void *)ALTERA_RP_TX_CTRL);
			printf("send cfg->send_data:0x%x\n", cfg->send_data);
			break;
		case FMT_TYPE_MEM_RD_32:
			/* config read, 3dw no data */
			writel(dw.dw3, (void *)ALTERA_RP_TX_REG0);
			writel(0x0, (void *)ALTERA_RP_TX_REG1);
			writel(ALTERA_RP_TX_CTRL_EOP, (void *)ALTERA_RP_TX_CTRL);
			printf("send dw.dw3:0x%x dw.dw4:0x%x\n",dw.dw3, 0);
			break;
		case FMT_TYPE_CFG_RD1:
			/* config read, 3dw no data */
			dw.dw3 |= 0x01000000;  /* add bus number here */
			writel(dw.dw3, (void *)ALTERA_RP_TX_REG0);
			writel(0x0, (void *)ALTERA_RP_TX_REG1);
			writel(ALTERA_RP_TX_CTRL_EOP, (void *)ALTERA_RP_TX_CTRL);
			printf("send dw.dw3:0x%x dw.dw4:0x%x\n",dw.dw3, 0);
			break;
		case FMT_TYPE_CFG_RD0:
			/* config read, 3dw no data */
			writel(dw.dw3, (void *)ALTERA_RP_TX_REG0);
			writel(0x0, (void *)ALTERA_RP_TX_REG1);
			writel(ALTERA_RP_TX_CTRL_EOP, (void *)ALTERA_RP_TX_CTRL);
			printf("send dw.dw3:0x%x dw.dw4:0x%x\n",dw.dw3, 0);
			break;
		case FMT_TYPE_CFG_WD1:
			dw.dw3 |= 0x01000000; /* add bus number here */
			if(((cfg->reg&0x4) == 0x4)||
				((cfg->reg&0xc) == 0xc)){ /* DW unaligned package */
				writel(dw.dw3, (void *)ALTERA_RP_TX_REG0);
				writel(cfg->send_data, (void *)ALTERA_RP_TX_REG1);
				writel(ALTERA_RP_TX_CTRL_EOP, (void *)ALTERA_RP_TX_CTRL);
				printf("send dw.dw3:0x%x dw.dw4:0x%x\n",dw.dw3, cfg->send_data);
			}
			else{/* DW aligned package */
				/* config write, 3dw with data */
				writel(dw.dw3, (void *)ALTERA_RP_TX_REG0);
				writel(0x0, (void *)ALTERA_RP_TX_REG1);
				writel(0x00, (void *)ALTERA_RP_TX_CTRL);
				printf("send dw.dw3:0x%x dw.dw4:0x%x\n",dw.dw3, 0);

				writel(cfg->send_data, (void *)ALTERA_RP_TX_REG0);
				writel(0x0, (void *)ALTERA_RP_TX_REG1);
				writel(ALTERA_RP_TX_CTRL_EOP, (void *)ALTERA_RP_TX_CTRL);
				printf("send cfg->send_data:0x%x\n", cfg->send_data);
			}
			break;

		case FMT_TYPE_CFG_WD0:
			if(((cfg->reg&0x4) == 0x4)||
				((cfg->reg&0xc) == 0xc)){ /* DW unaligned package */
				writel(dw.dw3, (void *)ALTERA_RP_TX_REG0);
				writel(cfg->send_data, (void *)ALTERA_RP_TX_REG1);
				writel(ALTERA_RP_TX_CTRL_EOP, (void *)ALTERA_RP_TX_CTRL);
				printf("send dw.dw3:0x%x dw.dw4:0x%x\n",dw.dw3, cfg->send_data);
			}
			else{
				/* config write, 3dw with data */
				writel(dw.dw3, (void *)ALTERA_RP_TX_REG0);
				writel(0x0, (void *)ALTERA_RP_TX_REG1);
				writel(0x00, (void *)ALTERA_RP_TX_CTRL);
				printf("send dw.dw3:0x%x dw.dw4:0x%x\n",dw.dw3, 0);

				writel(cfg->send_data, (void *)ALTERA_RP_TX_REG0);
				writel(0x0, (void *)ALTERA_RP_TX_REG1);
				writel(ALTERA_RP_TX_CTRL_EOP, (void *)ALTERA_RP_TX_CTRL);
				printf("send cfg->send_data:0x%x :0x%x\n",cfg->send_data, 0);
			}
			break;
		default:
			break;
	}

	rel = pcie_tlp_recv(cfg);

	return 0;
}
