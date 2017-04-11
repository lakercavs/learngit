#ifndef __ITR_H__
#define __ITR_H__
#include "types.h"


/***************************************************************************************************
* Trace mode control. when replay, controled by PCM. but for BIOS fetch, DMI configed by RCP firmware
*****************************************************************************************************/
#define ITR_TRACE_MODE_DRAIN			0
#define ITR_TRACE_MODE_ACTIVE			1
#define ITR_TRACE_MODE_PAUSE			2
#define ITR_TRACE_MODE_FLUSH_INT		3
#define ITR_TRACE_MODE_FLUSH			4
#define ITR_TRACE_MODE_REPLAY			5

/***************************************************************************************************
* Working memory and final memory access through HTL 
*****************************************************************************************************/
#define ITR_CSR_HTL_DATA_LOW    0x48000020 /* htl_table_data_low LSB 32B of table data for writing */
#define ITR_CSR_HTL_DATA_HI     0x48000040 /* htl_table_data_high MSB 32B of table data for writing */
#define ITR_CSR_HTL_BYTES_VALID 0x48000060  /* htl_byte_valid 64b of bytes valid to be written to the hash entry */
#define ITR_CSR_HTL_ACCESS_ADDR 0x48000080  /* htl_access_address 48b of address for the table access */
#define ITR_CSR_HTL_ACCESS_CTRL 0x480000A0  /* htl_table_access Table access control, read:2, write: 3 */

/*Access type*/
#define ITR_HTL_ACCESS_CTRL_FILL_WRITE	1
#define ITR_HTL_ACCESS_CTRL_WM_READ     2
#define ITR_HTL_ACCESS_CTRL_WM_WRITE    3
#define ITR_HTL_ACCESS_CTRL_FM_READ		4

/*Alist*/
#define ITR_CSR_HTL_ALIST_CNT	0x480000C0 /*32b*/
#define ITR_CSR_HTL_ALIST_BASE  0xC8000010 /*32b*/

/*replay again control*/
//TBD

/***************************************************************************************************
* DMI mode control: link setup & release 
*****************************************************************************************************/
#define ITR_CSR_PCH_MODE		0x94000088

#define ITR_PCH_MODE_LINK_UP    0x1
#define ITR_PCH_MODE_LINK_RE	0x0

#define ITR_CSR_PTLD_MODE		0x94000004

//#define ITR_PTLD_MODE_UP		0x90000068


/***************************************************************************************************
* BIOS fetch csrs 
*****************************************************************************************************/
#define ITR_CSR_PCH_REQ_ADDR	0x94000084

#define ITR_CSR_PCH_DATA		0x14000140


/***************************************************************************************************
* MTR controls & status
*****************************************************************************************************/
#define ITR_CSR_MTRA_TRACE_START	0x3C000080
#define ITR_CSR_MTRB_TRACE_START	0x40000080

#define ITR_MTR_TRACE_START_IDX_SHIFT	0
#define ITR_MTR_TRACE_START_FULL_SHIFT	4
#define ITR_MTR_TRACE_START_READ		8
#define ITR_MTR_TRACE_START_WRITE		9

#define ITR_CSR_MTRA_TRACE_STOP			0x3C0000A0
#define ITR_CSR_MTRB_TRACE_STOP			0x400000A0
#define ITR_MTR_TRACE_STOP_IDX_SHIFT	0


#define ITR_CSR_MTRA_HSDIMM_NUM		0xBC240000
#define ITR_CSR_MTRB_HSDIMM_NUM		0xC0240000

#define ITR_CSR_MTRA_PCIE_HSDIMM_ID   0xBC240004
#define ITR_CSR_MTRB_PCIE_HSDIMM_ID   0xC0240004
#define ITR_CSR_MTR_PCIE_HSDIMM_SIZE  4
#define ITR_CSR_MTR_PCIE_HSDIMM_NUM   6


#define ITR_CSR_MTRA_TRACING_VEC		0xBC24002C
#define ITR_CSR_MTRB_TRACING_VEC		0xC024002C

/*FIXME: MTR overflow in HSDIMM SPEC*/

#define ITR_CSR_MTRA_CFG_ADDR               0x3C000100
#define ITR_CSR_MTRA_CFG_DATA               0x3C000120
#define ITR_CSR_MTRA_CFG_CTL                0x3C0000E0

#define ITR_CSR_MTRB_CFG_ADDR               0x40000100
#define ITR_CSR_MTRB_CFG_DATA               0x40000120
#define ITR_CSR_MTRB_CFG_CTL                0x400000E0

#define ITR_MTR_CFG_IDX_OFF                28
#define ITR_MTR_CFG_MASK_OFF               0xF

#define MTR_TRACE_MGR_OFF                  0x04
#define MTR_TRACE_FULL_OFF                 29
#define MTR_TRACE_FUll_MASK                0x1
#define MTR_TRACE_TRG_LEN_OFF              0
#define MTR_TRACE_TRG_LEN_MASK             0xF

/***************************************************************************************************
* ITR controls & status
*AS the ITR status is controlled by PCM,  RCP only check whether it is overflow, and re-initialize the CSR
*****************************************************************************************************/
#define ITR_CSR_PTLD_UP_CTRL		0x9400000C
#define ITR_CSR_PTLD_DOWN_CTRL		0x94000010

#define ITR_CSR_PTLT0_UP_CTRL		0xA000000C
#define ITR_CSR_PTLT0_DOWN_CTRL		0xA0000010

#define ITR_CSR_PTLT1_UP_CTRL		0xAC00000C
#define ITR_CSR_PTLT1_DOWN_CTRL		0xAC000010

#define ITR_CSR_PTLT2_UP_CTRL		0xB800000C
#define ITR_CSR_PTLT2_DOWN_CTRL		0xB8000010

#define ITR_LINK_OVERFLOW_BIT		31  //the 31th bit in 32bit CSR

#define ITR_LINK_RE_INIT_BIT		28  // bit 28 in 32bit CSR
/***************************************************************************************************
* IO data buffers
*****************************************************************************************************/
#define ITR_CSR_PTLD_IO_BUFFER_BASE			0x14000000
#define ITR_CSR_PTLD_DMA_REQ_UP				0x00000040
#define ITR_CSR_PTLD_MMIO_RESP_UP			0x00000080
#define ITR_CSR_PTLD_P2P_UP					0x000000C0
#define ITR_CSR_PTLD_MISC_UP				0x00000100
#define ITR_CSR_PTLD_DMA_RESP_DOWN			0x00010040
#define ITR_CSR_PTLD_MMIO_REQ_DOWN			0x00010080
#define ITR_CSR_PTLD_P2P_DOWN				0x000100C0
#define ITR_CSR_PTLD_MISC_DOWN				0x00010100


#define ITR_CSR_PTLT0_IO_BUFFER_BASE		0x20000000
#define ITR_CSR_PTLT1_IO_BUFFER_BASE		0x2C000000
#define ITR_CSR_PTLT2_IO_BUFFER_BASE		0x38000000

#define ITR_CSR_PTLT_LINKx_UP_OFFSET(x)		(0x10000 * x)
#define ITR_CSR_PTLT_LINKx_DMA_REQ_UP		0x00000040
#define ITR_CSR_PTLT_LINKx_MMIO_RESP_UP     0x00000080
#define ITR_CSR_PTLT_LINKx_P2P_UP			0x000000C0
#define ITR_CSR_PTLT_LINKx_MISC_UP			0x00000100

#define ITR_CSR_PTLT_LINKx_DOWN_OFFSET(x)   (0x10000 * x + 0x40000)
#define ITR_CSR_PTLT_LINKx_DMA_RESP_DOWN	0x00000040
#define ITR_CSR_PTLT_LINKx_MMIO_REQ_DOWN	0x00000080
#define ITR_CSR_PTLT_LINKx_P2P_DOWN			0x000000C0
#define ITR_CSR_PTLT_LINKx_MIOS_DOWN		0x00000100

//

//IO trace buffer id
#define ITR_QID_DMA_REQ_UP			0
#define ITR_QID_MMIO_RESP_UP		1
#define ITR_QID_P2P_UP				2
#define ITR_QID_MISC_UP				3
#define ITR_QID_DMA_RESP_DOWN		4
#define ITR_QID_MMIO_REQ_DOWN		5
#define ITR_QID_P2P_DOWN			6
#define ITR_QID_MISC_DOWN			7


/*********************************************************************************************************/

/*HTL Functions*/
Bit32u ITR_WM_HTL_Access(Bit48u HPA_Address, Bit3u Access_Type, void* data_buf, void* Byte_valid);

Bit32u ITR_Get_HTL_Alist_Count(void);
Bit32u ITR_Get_HTL_Alist_Base(void);

Bit32u ITR_HTL_Reinit(void);

/*PTLD/PTLT Functions*/
Bit32u ITR_TLP_Fetch(Bit8u Socket, Bit8u group_id, Bit8u link_id, Bit8u buf_id, Bit32u len, void* tlp_buffer);

/*ITR control & status*/
Bit32u ITR_Overflow_Check(void);
Bit32u ITR_IO_Trace_Reinit(void);

/*MTR control & status*/
Bit32u MTR_Trace_Start(void);
Bit32u MTR_Trace_Stop(void);

Bit32u MTR_Overflow_Check(void);


/*DMI control and BIOS fetch*/
Bit32u ITR_PCH_Link_Setup();
Bit32u ITR_PCH_Link_Release();

Bit32u ITR_BIOS_Fetch(Bit32u addr, Bit32u len, void* buffer);


#endif
