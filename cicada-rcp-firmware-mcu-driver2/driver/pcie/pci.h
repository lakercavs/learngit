#ifndef __PCI_H__
#define __PCI_H__
#include "include/io.h"

/*
void pcie_write(Bit32u val, void* addr){
    Bit32u value;
    value =readl(addr);
    value |= val;
    writel(value, addr);
}

void pcie_clear(Bit32u val, void* addr){
    Bit32u value;
    value =readl(addr);
    value &= ~val;
    writel(value, addr);
}
*/


/*
 * We only support 3 capbilities here, so the array length is 3.
 * MSI/MSI-X
 * AER
 * PCIE Extend
 * */
#define PCIE_CAP_SIZE 6
struct pcie_cap_info{
	Bit32u cap_id;
	Bit32u cap_addr;
}pcie_all_cap[PCIE_CAP_SIZE];


Bit32u pci_reset(void);
Bit32u pci_bist(void);
Bit32u pci_ib_read_taraddr(Bit8u number);
Bit32u pci_get_ib_bar_match(Bit8u number);
Bit32u pci_ib_getaddr_addr_match(Bit64u start_addr, Bit64u end_addr, Bit8u region_number);
Bit32u pci_ob_read_taraddr(Bit8u number);
Bit64u pci_ob_transfer_addr(Bit32u start_addr, Bit32u end_addr, Bit8u region_number, Bit32u ctrl_value);
Bit64u pci_ob_memory_rw(Bit8u type, Bit8u func_sel, Bit8u func_number, Bit32u start_addr, Bit32u end_addr,   Bit8u region_number);
Bit64u pci_ob_cfg_rw(Bit8u type, Bit8u func_sel, Bit8u fun_number, Bit8u bus_numer, Bit8u device_number, Bit8u function_number,  Bit32u start_addr, Bit32u end_addr,   Bit8u region_number);
Bit64u pci_ob_message_write(Bit8u func_sel, Bit8u func_number, Bit32u start_addr, Bit32u end_addr, Bit8u region_number, Bit32u dw3, Bit32u dw4, Bit8u msg_code);
Bit16u pci_find_capbility_pointer(Bit8u cap_id);
Bit16u pci_rc_cap_msi_init(Bit8u cap_addr);
Bit16u pci_rc_cap_msix_init(Bit8u cap_addr);
Bit16u pci_rc_cap_pm_init(Bit8u cap_addr);
Bit16u pci_rc_cap_pcie_init(Bit8u cap_addr);
Bit16u pci_find_capabilities(void);
Bit32u pcie_verify_id(void);
Bit32u pci_rc_init_configspace(void);
Bit32u pci_ep_init(void);
Bit32u pci_rc_init(void);

Bit32u pci_altera_ep_test();

#endif /* __PCI_H__ */
