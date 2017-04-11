#ifndef __PCIE_VENDOR_REG__
#define __PCIE_VENDOR_REG__

#define PCIE_BASSADDR 0x0 					/* PCIE AXI baseaddr, should be modified */

/* ckcore arguments */
#define CKCORE_DEVICE_ID 0x0
#define CKCORE_VENDOR_ID 0x7595

/* tlp header message type 
 * fmt with data, and fmt without data is hardcoded.
 * Mesage Read/Write Routing machine, page 60 Table 2-3: Fmt[2:0] and Type[4:0] Field Encodings
 * */
#define PCIE_TLP_HEADER_MSG_TYPE_MRD  	0x0 	/* Memory Read/Write Reqeust */
#define PCIE_TLP_HEADER_MSG_TYPE_MRD_LK 0x1 	/* Memory Read Request-lock */
#define PCIE_TLP_HEADER_MSG_TYPE_IORD  	0x2 	/* IO Read/Write Reqeust */
#define PCIE_TLP_HEADER_MSG_TYPE_CFGRD0 0x4 	/* Configuration Read/Write Reqeust, type 0 */
#define PCIE_TLP_HEADER_MSG_TYPE_CFGRD1 0x4 	/* Configuration Read/Write Reqeust, type 0 */
#define PCIE_TLP_HEADER_MSG_TYPE_MSG 	0x10 	/* Message Read/Write Reqeust, type 0, we only write message here */
#define PCIE_TLP_HEADER_MSG_ROUTE_RC    0x0 	/* Routed to Root Complex */
#define PCIE_TLP_HEADER_MSG_ROUTE_ADDR  0x1 	/* Routed by Address Address */
#define PCIE_TLP_HEADER_MSG_ROUTE_ID 	0x2 	/* Routed by ID See Section 2.2.4.2 */
#define PCIE_TLP_HEADER_MSG_BROAD_RC	0x3 	/* Broadcast from Root Complex */
#define PCIE_TLP_HEADER_MSG_LOCAL 	 	0x4 	/* Local - Terminate at Receiver */
#define PCIE_TLP_HEADER_MSG_TO_RC  		0x5 	/* Gathered and routed to Root Complex */

/* APB address map */
#define PCIE_BASEADDR_F0_LEA_CFG_SPACE 	(PCIE_BASSADDR + 0x00000) /* 0x00000~0x000FF PCIe function0 leagcy configuration space */
#define PCIE_BASEADDR_F0_EXT_CFG_SPACE  (PCIE_BASSADDR + 0x00100) /* 0x00100~0x00FFF PCIe function0 extended configuration space */
#define PCIE_BASEADDR_F1_LEA_CFG_SPACE 	(PCIE_BASSADDR + 0x01000) /* 0x01000~0x010FF PCIe function1 leagcy configuration Space */
#define PCIE_BASEADDR_F1_EXT_CFG_SPACE 	(PCIE_BASSADDR + 0x01100) /* 0x01100~0x01FFF PCIe function1 extended configuration space */
#define PCIE_BASEADDR_F2_LEA_CFG_SPACE 	(PCIE_BASSADDR + 0x02000) /* 0x02000~0x020FF PCIe function2 leagcy configuration space */
#define PCIE_BASEADDR_F2_EXT_CFG_SPACE 	(PCIE_BASSADDR + 0x02100) /* 0x02100~0x02FFF PCIe function2 extended configuration space */
#define PCIE_BASEADDR_F0_MSIX_TAB 		(PCIE_BASSADDR + 0x03000) /* 0x03000~0x030FF PCIe function0 MSI-X Table  */
#define PCIE_BASEADDR_F0_MSIX_PBA 		(PCIE_BASSADDR + 0x03100) /* 0x03100~0x031FF PCIe function0 MSI-X PBA */
#define PCIE_BASEADDR_F1_MSIX_TAB 	 	(PCIE_BASSADDR + 0x03200) /* 0x03200~0x032FF PCIe function1 MSI-X Table */
#define PCIE_BASEADDR_F1_MSIX_PBA 		(PCIE_BASSADDR + 0x03300) /* 0x03300~0x033FF PCIe function1 MSI-X PBA */
#define PCIE_BASEADDR_F2_MSIX_TAB 	 	(PCIE_BASSADDR + 0x03400) /* 0x03400~0x034FF PCIe function2 MSI-X Table */
#define PCIE_BASEADDR_F2_MSIX_PBA 		(PCIE_BASSADDR + 0x03500) /* 0x03500~0x035FF PCIe function2 MSI-X PBA */
										 						  /* 0x03600~0x0FFFF Reserved */
																  /* 0x10000~0x2FFFF Reserved */
#define PCIE_BASEADDR_EXT_APP 			(PCIE_BASSADDR + 0x30000) /* 0x30000~0x3FFFF External Application Register ELBI Bus */
#define PCIE_BASEADDR_F0_SHADOW 		(PCIE_BASSADDR + 0x40000) /* 0x40000~0x4FFFF PCIe function 0 Shadow Register(Port Logic) */
#define PCIE_BASEADDR_F1_SHADOW 		(PCIE_BASSADDR + 0x50000) /* 0x50000~0x5FFFF PCIe function 1 Shadow Register(Port Logic) */
#define PCIE_BASEADDR_F2_SHADOW 		(PCIE_BASSADDR + 0x60000) /* 0x60000~0x6FFFF PCIe function 2 Shadow Register(Port Logic) */
#define PCIE_BASEADDR_ATU_INBOUND 		(PCIE_BASSADDR + 0x70000) /* 0x70000~0x7FFFF PCIe iATU Inbound Register */
#define PCIE_BASEADDR_ATU_OUTBOUND 		(PCIE_BASSADDR + 0x80000) /* 0x80000~0x8FFFF PCIe iATU Outbound Register */
#define PCIE_BASEADDR_GLOBAL_CTL 		(PCIE_BASSADDR + 0x90000) /* 0x90000~0x9FFFF PCIe Global Control Register */
#define PCIE_BASEADDR_PHY0 				(PCIE_BASSADDR + 0xA0000) /* 0xA0000~0xBFFFF PCIe PHY0 register */
#define PCIE_BASEADDR_PHY1 				(PCIE_BASSADDR + 0xC0000) /* 0xC0000~0xDFFFF PCIe PHY1 register */
#define PCIE_BASEADDR_PCS_CTL 			(PCIE_BASSADDR + 0xE0000) /* 0xE0000~0xFFFFF PCIe PCS Control Register */

/* PCIe Function0 Bar4 Address Map */
#define PCIE_BASEADDR_BAR4_EXT_ADDR 	0x00000 /* 0x00000~0x03FFF External Application Register */
												/* 0x04000~0x05FFF Reserved */
												/* 0x06000~0x07FFF Reserved */
												/* 0x08000~0x0FFFF Reserved */
#define PCIE_BASEADDR_BAR4_F0_MSIX_TAB  0x10000 /* 0x10000~0x100FF PCIe Function0 MSI-X Table */
												/* 0x10100~0x11FFF Reserved */
#define PCIE_BASEADDR_BAR4_F0_MSIX_PBA  0x12000 /* 0x12000~0x120FF PCIe Function0 MSI-X PBA */
												/* 0x12100~0x1FFFF Reserved */

/* MSI-X Table Register Map(per Function0): */
#define PCIE_BASEADDR_MSIX_TAB_E0 		0x00 	/* 0x00~0x0F MSI-X Table Entry 0 */
#define PCIE_BASEADDR_MSIX_TAB_E1 		0x10 	/* 0x10~0x1F MSI-X Table Entry 1 */
#define PCIE_BASEADDR_MSIX_TAB_E2 		0x20 	/* 0x20~0x2F MSI-X Table Entry 2 */
#define PCIE_BASEADDR_MSIX_TAB_E3 		0x30 	/* 0x30~0x3F MSI-X Table Entry 3 */
#define PCIE_BASEADDR_MSIX_TAB_E4 		0x40 	/* 0x40~0x4F MSI-X Table Entry 4 */
#define PCIE_BASEADDR_MSIX_TAB_E5 		0x50 	/* 0x50~0x5F MSI-X Table Entry 5 */
#define PCIE_BASEADDR_MSIX_TAB_E6 		0x60 	/* 0x60~0x6F MSI-X Table Entry 6 */
#define PCIE_BASEADDR_MSIX_TAB_E7 		0x70 	/* 0x70~0x7F MSI-X Table Entry 7 */

/*
 * Cmments for ATU:
 * If we use PCIE as EP, then we can read bar address from config space.
 * We can translation address using ATU Inbound Bar Match Mode to tarnslation address directly.
 *
 * If we use PCIE as RC, then we must config bar addr to config space. The address should be 64 bit address
 * And then we use Outbound, as address mode to translation the 64 bit address to 32 bit.
 *
 * */

/* 
 * Inbound ATU Register Address Layout:
 * We can use IB ATU to remap bar0~bar2 to 32 bit addr.
 * 64 bit --> 32 bit transfer
 * Address Bits 	ADDR[31:20] 	ADDR[19:16] 	ADDR[15:8] 							ApplicationDDR[7:0]
 * Value 			PCIe APB Base 	Address 0x7 	Inbound Region Number 				Inbound 		Register Number
 * Address Label Description 
 * We should caculate 64bit address is not overlap for different regions.
 * */

#define PCIE_BASEADDR_IB_CTRL 			0x00 	/* 0x00 IB_CTRL_REGION_i IB Control Register for Region i */
												/* 0x04 IB_RESV0_REGION_i IB Reserved 0 Register for Region i */
												/* 0x08 IB_RESV1_REGION_i IB Reserved 1 Register for Region i */
												/* 0x0C IB_RESV2_REGION_i IB Reserved 2 Register for Region i */
												/* 0x10 IB_RESV3_REGION_i IB Reserved 3 Register for Region i */
												/* 0x14 IB_RESV4_REGION_i IB Reserved 4 Register for Region i */
#define PCIE_BASEADDR_IB_ST_LADDR 		0x18 	/* 0x18 IB_ST_LADDR_REGION_i IB Start Lower 32bits Address Register for Region i */
#define PCIE_BASEADDR_IB_ST_UADDR 		0x1C 	/* 0x1C IB_ST_UADDR_REGION_i IB Start Upper 32bits Address Register for Region i */
#define PCIE_BASEADDR_IB_END_LADDR 		0x20 	/* 0x20 IB_END_LADDR_REGION_i IB End Lower 32bits Address Register for Region i */
#define PCIE_BASEADDR_IB_END_UADDR 		0x24 	/* 0x24 IB_END_UADDR_REGION_i IB End Upper 32bits Address Register for Region i */
#define PCIE_BASEADDR_IB_TRGT_LADDR 	0x28 	/* 0x28 IB_TRGT_LADDR_REGION_i IB Target Lower 32bits Address Register for Region i */
#define PCIE_BASEADDR_IB_TRGT_UADDR 	0x2C 	/* 0x2C IB_TRGT_UADDR_REGION_iI nbound Target Upper 32bits Address Register for Region 	i, read always return 0 here */


/* Inbound ATU Control register details */
#define PCIE_IB_CTRL_ENABLE 			(1 << 0) /* Inbound ATU enable */
#define PCIE_IB_CTRL_MODE(x) 			((x) << 1) /* 0:address match, 1:bar match */
#define PCIE_IB_CTRL_MODE_ADDRESS 		(0) 	/* address match mode */
#define PCIE_IB_CTRL_MODE_BAR 			(1) 	/* bar match mode */
#define PCIE_IB_CTRL_BAR_NUM(x) 		((x) << 2) /* Valid using bar match mode, bar number */
#define PCIE_IB_CTRL_BAR_NUM_BAR1_BAR0  (0) 	 /* 64 bit Bar1~Bar0 is used for address translation */
#define PCIE_IB_CTRL_BAR_NUM_BAR3_BAR2  (1) 	 /* 64 bit Bar3~Bar2 is used for address translation */
#define PCIE_IB_CTRL_BAR_NUM_BAR5_BAR4  (2) 	 /* 64 bit Bar5~Bar4 is used for address translation */
#define PCIE_IB_CTRL_FUNC_NUM(x) 		((x) << 4) /* Valid using bar match mode, funciton number */
#define PCIE_IB_CTRL_FUNC_NUM_0 		(0) 	/* Function0 Bar is selected to do address translation. */
#define PCIE_IB_CTRL_FUNC_NUM_1 		(1) 	/* Function1 Bar is selected to do address translation. */
#define PCIE_IB_CTRL_FUNC_NUM_2 		(2) 	/* Function2 Bar is selected to do address translation. */

/* 
 * Outbound ATU Register Address Layout:
 * 32 bit --> 64bit transfer
 * We can use this to remap our 32bit memory address to itr.
 * Address Bits 	ADDR[31:20] 	ADDR[19:16] 	ADDR[15:8] 							ApplicationDDR[7:0]
 * Value 			PCIe APB Base 	Address 0x8 	Outbound Region Number 				Outbound 		Register Number
 * Address Label Description 
 * The Target address only userd memory access.
 *
 * */
#define PCIE_BASEADDR_OB_CTRL 			0x00 	/* 0x00	OB_CTRL_REGION_i		Outbound Control Register for Region i */
#define PCIE_BASEADDR_OB_WR_MSIC 		0x04 	/* 0x04	OB_WR_MSIC_REGION_i		Outbound Write Channel Misc Control Setting for Region i */
#define PCIE_BASEADDR_OB_WR_HDR_DW3 	0x08 	/* 0x08	OB_WR_HDR_DW3_REGION_i	Outbound Write Channel DW3 of Msg o MsgD TLP for Region i */
#define PCIE_BASEADDR_OB_WR_HDR_DW4 	0x0C 	/* 0x0C	OB_WR_HDR_DW4_REGION_i	Outbound Write Channel DW4 of Msg o MsgD TLP for Region i */
#define PCIE_BASEADDR_OB_RD_MSIC 		0x10 	/* 0x10	OB_RD_MSIC_REGION_i		Outbound Read Channel Misc Control Setting for Region i */
#define PCIE_BASEADDR_OB_CFG_ADDR 		0x14  	/* 0x14	OB_CFG_ADDR_REGION_i	Outbound Configuration Address for Region i */
#define PCIE_BASEADDR_OB_ST_LADDR 		0x18 	/* 0x18	OB_ST_LADDR_REGION_i	Outbound Start Lower 32bits Address Register for Region i */
#define PCIE_BASEADDR_OB_ST_UADDR 		0x1C 	/* 0x1C	OB_ST_UADDR_REGION_i	Outbound Start Upper 32bits Address Register for Region i */
#define PCIE_BASEADDR_OB_END_LADDR 		0x20 	/* 0x20	OB_END_LADDR_REGION_i	Outbound End Lower 32bits Address Register for Region i */
#define PCIE_BASEADDR_OB_END_UADDR 		0x24 	/* 0x24	OB_END_UADDR_REGION_i	Outbound End Upper 32bits Address Register for Region i */
#define PCIE_BASEADDR_OB_TRGT_LADDR 	0x28 	/* 0x28	OB_TRGT_LADDR_REGION_i	Outbound Target Lower 32bits Address Register for Region i */
#define PCIE_BASEADDR_OB_TRGT_UADDR 	0x2C 	/* 0x2C	OB_TRGT_UADDR_REGION_i	Outbound Target Upper 32bits Address Register for Region i */

/* Outbound ATU Control register details */
#define PCIE_OB_CTRL_ENABLE 			(1 << 0)  	/*Outbound ATU Enable */
#define PCIE_OB_CTRL_TRANS_TYPE(x) 		((x) << 1) 	/* Outbound ATU region translation type, we don't support IO type here  */
#define PCIE_OB_CTRL_TRANS_TYPE_MEM 	(0)  		/* 00: AXI to PCIe memory access. */
#define PCIE_OB_CTRL_TRANS_TYPE_CFG 	(1) 		/* 01: AXI to PCIe configuration access. */
#define PCIE_OB_CTRL_TRANS_TYPE_MSG 	(2) 		/* 10: AXI to PCIe Msg/MsgD access. */
#define PCIE_OB_CTRL_FUNC_SEL(x) 		((x) << 3)  /* Outbound TLP Function number source selection */
#define PCIE_OB_CTRL_FUNC_SEL_INT 		(0) 		/* 0: Outbound TLP Function number is from internal ATU register.  */
#define PCIE_OB_CTRL_FUNC_SEL_EXT 		(1) 		/* 1: Outbound TLP Function number is from external AXI interface. if this bit is set, PCIE_OB_WR_MSIC_TLP_FUNC is valid*/

/* 
 * Outbound ATU Write Misc INFO Register 
 * TLP format See: Section 2.2.1 PCI_Express_Base_3.0_Specification  
 * Page 84, msg code and TAG code, for message request rules.
 * We can ignore 3DW or 4DW
 * */

/* See more in Page 60 Table 2-3: Fmt[2:0] and Type[4:0] Field Encodings, PCI_Express_Base_3.0_Specification */
#define PCIE_OB_WR_MSIC_TLP_TYPE(x) 	((x) << 0)  /* Bit[4:0] TLP Header TYPE bit, for meory access, configuration */
#define PCIE_OB_WR_MSIC_TLP_EP 			(1 << 6) 	/* TLP Header EP bit */
#define PCIE_OB_WR_MSIC_TLP_NS 			(1 << 8) 	/* TLP Header NS bit */
#define PCIE_OB_WR_MSIC_TLP_RO 			(1 << 9) 	/* TLP Header RO bit */
#define PCIE_OB_WR_MSIC_TLP_TC(x) 		((x) << 10) /* Bit[12:10] TLP Header TC bit, 0 as default */
/* See more in Page 83 Section 2.2.8 Message Request Rules, PCI_Express_Base_3.0_Specification */
#define PCIE_OB_WR_MSIC_TLP_MSG(x) 		((x) << 13) /* Bit[20:13] TLP msg code bit, for PCIE standard message rules code */
#define PCIE_OB_WR_MSIC_TLP_FUNC(x) 	((x) << 22) /* Bit[23:22] TLP function code bit, this function number is requester id(B,D,F), function number */
/* The function bit only used when we are PCIE EndPoint, If we are PCIE RC, This bit always 0 */
#define PCIE_OB_WR_MSIC_TLP_FUNC_GRP0 	1 			/* Function0 for group1, only used to writing TLP  */
#define PCIE_OB_WR_MSIC_TLP_FUNC_GRP1 	0 			/* Function1 for group0, only used to writing TLP */
#define PCIE_OB_WR_MSIC_TLP_FUNC_GRP2 	2 			/* Function2 for group2, only used to writing TLP */
#define PCIE_OB_WR_MSIC_TLP_TAG(x) 		((x) << 24) /* Bit[31:24] TLP TAG bit, softeware only write 0 here, only used for memory writing, already hard-coded */

/* 
 * Outbound ATU Read Misc INFO Register 
 * TLP format See: Section 2.2.1 PCI_Express_Base_3.0_Specification  
 * */

#define PCIE_OB_RD_MSIC_TLP_TYPE(x) 	((x) << 0)  /* Bit[4:0] TLP Header TYPE bit */
#define PCIE_OB_RD_MSIC_TLP_EP 			(1 << 6) 	/* TLP Header EP bit */
#define PCIE_OB_RD_MSIC_TLP_NS 			(1 << 8) 	/* TLP Header NS bit */
#define PCIE_OB_RD_MSIC_TLP_RO 			(1 << 9) 	/* TLP Header RO bit */
#define PCIE_OB_RD_MSIC_TLP_TC(x) 		((x) << 10) /* Bit[12:10] TLP Header TC bit */
#define PCIE_OB_RD_MSIC_TLP_MSG(x) 		((x) << 13) /* Bit[20:13] TLP msg code bit */
#define PCIE_OB_RD_MSIC_TLP_FUNC(x) 	((x) << 22) /* Bit[23:22] TLP function code bit */
/* The function bit only used when we are PCIE EndPoint, If we are PCIE RC, This bit always 0 */
#define PCIE_OB_RD_MSIC_TLP_FUNC_GRP0 	1 			/* Function0 for group1, only used to writing TLP  */
#define PCIE_OB_RD_MSIC_TLP_FUNC_GRP1 	0 			/* Function1 for group0, only used to writing TLP */
#define PCIE_OB_RD_MSIC_TLP_FUNC_GRP2 	2 			/* Function2 for group2, only used to writing TLP */

/*
 * Outbound ATU Target Configruation Address Register
 * If the tlp type is configuration, we need config these bit.
 * */
#define PCIE_OB_CFG_FUNC_NUM(x) 		((x) << 16) /* Bit[18:16] Outbound ATU region configuration function number */
#define PCIE_OB_CFG_DEV_NUM(x) 			((x) << 19) /* Bit[23:19] Outbound ATU region configuration device number */
#define PCIE_OB_CFG_BUS_NUM(x) 			((x) << 24) /* Bit[31:24] Outbound ATU region configuration bus number */

#endif /* __PCIE_VENDOR_REG__*/
