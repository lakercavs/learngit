#include "misc.h"
#include "pci.h"
#include "pci_regs.h"
#include "pci_vendor_reg.h"
#include "pci_cmdq.h"
#include "ck810.h"

#define PCI_EP_CONFIG_SPACE_BASE_ADDR CK_PCIE_EP_ADDRBASE

Bit32u pci_altera_verify_id(){
	Bit32u val;

	val = readl((void *)(PCI_EP_CONFIG_SPACE_BASE_ADDR));
	/*
	if((val&0xFFFF) != CKCORE_VENDOR_ID){
		printf("wrong vendor id:0x%o8x\n", val&0xFFFF);
		return -1;
	}
	if(((val >> 16)&0xFFFF) != CKCORE_DEVICE_ID){
		printf("wrong device id:0x%o8x\n", (val >> 16)&0xFFFF);
		return -1;
	}
	*/

	printf("vendor id is:0x%x, device id is:0x%x\n", (val&0xFFFF), ((val >> 16)&0xFFFF));

	return 0;
}

Bit16u pci_altera_find_capabilities(){
    Bit8u cap_id;
    Bit32u addr;    /* capbility list entry */
    Bit32u pcap;    /* capability pointer */
    Bit8u i;

    addr = PCI_EP_CONFIG_SPACE_BASE_ADDR + PCI_CAPABILITY_LIST;    /* pcie capbility list pointer */

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
            case PCI_CAP_ID_PM:         /* Power Management */
                printf("PM addr:0x%04x\n", pcie_all_cap[i].cap_addr);
                break;
            case PCI_CAP_ID_MSI:        /* MSI */
                printf("MSI addr:0x%04x\n", pcie_all_cap[i].cap_addr);
                break;
            case PCI_CAP_ID_MSIX:       /* MSI-X */
                printf("MSIX addr:0x%04x\n", pcie_all_cap[i].cap_addr);
                break;
            case PCI_CAP_ID_EXP:        /* PCI Express */
                printf("EXP addr:0x%04x\n", pcie_all_cap[i].cap_addr);
                break;
            default:
                break;
        }
    }

    return 0;

}

Bit16u pci_altera_get_capability(Bit16u cap_id){
	Bit32u i;

	for(i=0; i< PCIE_CAP_SIZE; i++){
		/* Init capbility */
		if(cap_id == pcie_all_cap[i].cap_id){
			printf("find capability addr:0x%04x\n", pcie_all_cap[i].cap_addr);
			return pcie_all_cap[i].cap_addr;
		}
	}
	return 0;
}

/*
 * @lenght 32bit length
 * */
Bit16u pci_altera_read_bar0(Bit32u length){
	Bit32u i;
	Bit32u addr = CMDQ_INTC_BASE;

	printf("Read pcie bar0\n");
	for(i=0; i<length; i++){
		if(i%4 == 0)
			printf("\naddr:0x%08x: ", addr);
		printf("0x%08x ", readl((void *)(addr + i*4)));
	}
	return 0;
}

/*
 * translate 64 bit address to 32 bit address
 * */
Bit32u pci_altera_enable_atu(Bit64u addr){
	Bit32u addr_lo;
	Bit32u addr_hi;

	addr_lo = (addr&0xFFFFFFFF);
	addr_hi = ((addr >> 32)&0xFFFFFFFF);

	printf("addr_lo:0x%08x hi:0x%08x\n", addr_lo, addr_hi);
		
	writel((addr_lo&0xfe000000) + 1, (void *)(PCM_MEM_BASE1 + 0x1000));
	writel(addr_hi, (void *)(PCM_MEM_BASE1 + 0x1004));

	printf("Read write addr:0x%08x\n", readl((void *)(PCM_MEM_BASE1 + 0x1000)));
	return 0;
}

/*
 * @addr msi addr
 * */
Bit16u pci_altera_msi_test(Bit16u addr){
	Bit64u msi_addr = 0;
	Bit16u msi_data = 0;

	printf("msi offset addr:0x%02x\n", addr);
	/* lo addr */
	msi_addr = readl((void*)(PCI_EP_CONFIG_SPACE_BASE_ADDR + addr + PCI_MSI_ADDRESS_LO));
	/* hi addr */
	msi_addr |= (readl((void*)(PCI_EP_CONFIG_SPACE_BASE_ADDR + addr + PCI_MSI_ADDRESS_HI)) << 32);

	msi_data = readl((void*)(PCI_EP_CONFIG_SPACE_BASE_ADDR + addr + PCI_MSI_DATA_64));

	pci_altera_enable_atu(msi_addr);

	writel(msi_data, (void*)(0x8000000 + (msi_addr&0x1ffffff)));
	return 0;
}

Bit32u pci_altera_ep_test(){
	Bit16u msi_addr;

	printf("pci altera ep test\n");
	pci_altera_verify_id();

	pci_altera_find_capabilities();

	pci_altera_read_bar0(32);

	msi_addr = pci_altera_get_capability(PCI_CAP_ID_MSI);
	if(msi_addr == 0){
		printf("Can not find MSI capablility\n");
	}
	else{
		pci_altera_msi_test(msi_addr);
		
	}

	return 0;
}
