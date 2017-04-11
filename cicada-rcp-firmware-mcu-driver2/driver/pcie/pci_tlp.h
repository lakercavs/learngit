#ifndef __PCI_TLP_H__
#define __PCI_TLP_H__

#include "io.h"

/* tlp header defination */
/* 4 dw tlp header */
struct tlp_header_total_4dw{
	Bit32u dw1;
	Bit32u dw2;
	Bit32u dw3;
	Bit32u dw4;
};
/* 3 dw tlp header */
struct tlp_header_total_3dw{
	Bit32u dw1;
	Bit32u dw2;
	Bit32u dw3;
};

/* data length bit from 1 to 1024 dw, the min value is 0b'1=dw, and the max value is 0b'00 0000 0000=1024dw */

/* tlp header format */
#define FMT_3DW_NO_DATA   0  /* 000b 3 DW header, no data */
#define FMT_4DW_NO_DATA   1  /* 001b 4 DW header, no data */
#define FMT_3DW_WITH_DATA 2  /* 010b 3 DW header, with data */
#define FMT_4DW_WITH_DATA 3  /* 011b 4 DW header, with data */
#define FMT_TLP_PREFIX    4  /* 100b TLP Prefix */

/* tlp header type */
#define CFG_TYPE0 0x4 		/* b'00100 config type 0 */
#define CFG_TYPE1 0x5 		/* b'00101 config type 1 */
#define CFG_COMPL 0xA 		/* b'01010 config completion type */

#define FMT_TYPE_CFG_RD0 ((FMT_3DW_NO_DATA<<5) | CFG_TYPE0)  /* Configuration Read Type 0 */
#define FMT_TYPE_CFG_WD0 ((FMT_3DW_WITH_DATA<<5) | CFG_TYPE0)  /* Configuration Write Type 0 */
#define FMT_TYPE_CFG_RD1 ((FMT_3DW_NO_DATA<<5) | CFG_TYPE1)  /* Configuration Read Type 1 */
#define FMT_TYPE_CFG_WD1 ((FMT_3DW_WITH_DATA<<5) | CFG_TYPE1)  /* Configuration Write Type 1 */
#define FMT_TYPE_CFG_CPL ((FMT_3DW_NO_DATA<<5) | CFG_COMPL)  /* Completion without Data */
#define FMT_TYPE_CFG_CPLD ((FMT_3DW_WITH_DATA<<5)| CFG_COMPL)  /* Completion witData */
#define FMT_TYPE_MEM_RD_32 (0x00000000)  /* Memory Read 32bit */
#define FMT_TYPE_MEM_WD_32 (0x40000000)  /* Memroy Write 32bit */

/* altera fpga register */
//#define ALTERA_FPGA_BASE_ADDR 0x0e000000
#define ALTERA_FPGA_BASE_ADDR 0x1a100000
#define ALTERA_RP_TLP_DATA_ADDR (ALTERA_FPGA_BASE_ADDR + 0x2000)  /* Root Port TLP Data Registers, 0x2000–0x2FFF */

#define ALTERA_RP_TX_REG0 (ALTERA_RP_TLP_DATA_ADDR + 0x0)  /* Lower 32 bits of the TX TLP. */
#define ALTERA_RP_TX_REG1 (ALTERA_RP_TLP_DATA_ADDR + 0x4)  /* Upper 32 bits of the TX TLP. */
#define ALTERA_RP_TX_CTRL (ALTERA_RP_TLP_DATA_ADDR + 0x8)  /* Control register of the TX TLP. */
#define ALTERA_RP_TX_CTRL_EOP  (1 << 1) 	/* Write 1’b1 to specify the end of a packet. */
#define ALTERA_RP_TX_CTRL_SOP  (1 << 0)  	/* Write 1’b1 to specify the start of a packet. */

#define ALTERA_RP_RX_CTRL (ALTERA_RP_TLP_DATA_ADDR + 0x10)  /* Control register of the RX TLP. */
#define ALTERA_RP_RX_REG0 (ALTERA_RP_TLP_DATA_ADDR + 0x14)  /* Lower 32 bits of the RX TLP. */
#define ALTERA_RP_RX_REG1 (ALTERA_RP_TLP_DATA_ADDR + 0x18)  /* Upper 32 bits of the RX TLP. */
#define ALTERA_RP_RX_CTRL_EOP  (1 << 1) 	/* When 1’b1, indicates that the data for a Completion TLP is ready to be read by the Application Layer. */
#define ALTERA_RP_RX_CTRL_SOP  (1 << 0)  	/* When 1’b1, indicates that the final data for a Completion TLP is ready to be read by the Application Layer. */

#define ALTERA_ENTRY0_LO (ALTERA_RP_TLP_DATA_ADDR + 0x1000)  /* atu entry 0 */
#define ALTERA_ENTRY1_HI (ALTERA_RP_TLP_DATA_ADDR + 0x1004)  /* atu entry 1 */

#define ALTERA_STATUS (ALTERA_FPGA_BASE_ADDR + 0x3c64)  /* atu entry 1 */

struct cfg_status{
	Bit8u status; 	/* config satus */ 
	Bit8u reg; 			/* read or write register addr */
	Bit32u send_data; 		/* reg vale you want to write */
	Bit32u recv_data; 		/* reg vale you want to write */
};

Bit32u pcie_tlp_send(struct cfg_status *cfg);
Bit32u pci_altera_rc_test(void);
Bit32u ffs(Bit32u word);

#endif /* __PCI_TLP_H__ */
