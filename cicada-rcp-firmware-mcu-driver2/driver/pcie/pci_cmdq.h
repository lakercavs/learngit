#ifndef __PCIE_CMDQ_H__
#define __PCIE_CMDQ_H__
#define PCIE_CMDQ_MEM	0x0000 /* 0x0000~0x3FFF	16KB memory for MCU to access.*/
#define PCIE_CMDQ_MEM_LEN 0x4000 /*16KB memory */
#define PCIE_CMDQ_MTA0	0x4000 /* 0x4000 Trigger address, writing to the address will cause the PCIe MSI 0Address LSB 1:0 are ignored for the register is 32bit accessed.*/
#define PCIE_CMDQ_MTA1	0x4004 /* 0x4004 Trigger address, writing to the address will cause the PCIe MSI 1*/
#define PCIE_CMDQ_MTA2	0x4008 /* 0x4008 Trigger address, writing to the address will cause the PCIe MSI 2*/
#define PCIE_CMDQ_MTA3	0x400C /* 0x400C Trigger address, writing to the address will cause the PCIe MSI 3*/
#define PCIE_CMDQ_MTA4	0x4010 /* 0x4010 Trigger address, writing to the address will cause the PCIe MSI 4*/
#define PCIE_CMDQ_MTA5	0x4014 /* 0x4014 Trigger address, writing to the address will cause the PCIe MSI 5*/
#define PCIE_CMDQ_MTA6	0x4018 /* 0x4018 Trigger address, writing to the address will cause the PCIe MSI 6*/
#define PCIE_CMDQ_MTA7	0x401C /* 0x401C Trigger address, writing to the address will cause the PCIe MSI 7*/
#define PCIE_CMDQ_MIE	0x4020 /* 0x4020 1: MSI0~7 enable for PCIe, other bits are reserved.*/
#define PCIE_CMDQ_MIS	0x4024 /* 0x4024 1: MSI0~7 status for PCIe, other bits are reserved.*/
#define PCIE_CMDQ_ITA0	0x4040 /* 0x4040 Trigger address, writing to the address will cause interrupt0 to MCU*/
#define PCIE_CMDQ_ITA1	0x4044 /* 0x4044 Trigger address, writing to the address will cause interrupt1 to MCU*/
#define PCIE_CMDQ_ITA2	0x4048 /* 0x4048 Trigger address, writing to the address will cause interrupt2 to MCU*/
#define PCIE_CMDQ_ITA3	0x404C /* 0x404C Trigger address, writing to the address will cause interrupt3 to MCU*/
#define PCIE_CMDQ_ITA4	0x4050 /* 0x4050 Trigger address, writing to the address will cause interrupt4 to MCU*/
#define PCIE_CMDQ_ITA5	0x4054 /* 0x4054 Trigger address, writing to the address will cause interrupt5 to MCU*/
#define PCIE_CMDQ_ITA6	0x4058 /* 0x4058 Trigger address, writing to the address will cause interrupt6 to MCU*/
#define PCIE_CMDQ_ITA7	0x405C /* 0x405C Trigger address, writing to the address will cause interrupt7 to MCU*/
#define PCIE_CMDQ_IIE	0x4060 /* 0x4060 1: IRQ0~7 enable for MCU, other bits are reserved.*/
#define PCIE_CMDQ_IIS	0x4064 /* 0x4064 1: IRQ0~7 status for MCU, other bits are reserved.*/
							   /* 0x5000	Debug register for MSI port status/ internal FSM бн. TBD*/
#endif /* __PCIE_CMDQ_H__ */
