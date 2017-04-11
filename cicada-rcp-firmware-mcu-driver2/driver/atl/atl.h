#ifndef __ATL_H__
#define __ATL_H__

#include "types.h"


#define  ATL_INTERFACE_REG_BASE_ADDRESS     0x197E0000     /*The base address of ATL interface register in CK810 address space*/

/*Control and Status Register*/
#define  ATL_GLAR_OFFSET                    0x00000000     /*Guest Linear Address Register(GLAR), 64bit input register*/
#define  ATL_GLARL_OFFSET                   0x00000000    /*lower 32bits*/
#define  ATL_GLARH_OFFSET                   0x00000004    /*upper 32bits*/

#define  ATL_GPAR_OFFSET                    0x00000008     /*Guest Physical Address Register(GPAR),64bit output register*/
#define  ATL_GPARL_OFFSET                   0x00000008
#define  ATL_GPARH_OFFSET                   0x0000000C

#define  ATL_EPPTR_OFFSET                   0x00000010     /*EPT Translation Register (EPTTR),64bit input register*/
#define  ATL_EPPTRL_OFFSET                  0x00000010
#define  ATL_EPPTRH_OFFSET                  0x00000014

#define  ATL_HPAR_OFFSET                    0x00000018     /*Host Physical Address Register (HPAR), 64bit output register*/
#define  ATL_HPARL_OFFSET                   0x00000018
#define  ATL_HPARH_OFFSET                   0x0000001C

#define  ATL_MAR_OFFSET                     0x00000020     /*Memory Access Register (MAR), 64bit input register*/
#define  ATL_MARL_OFFSET                    0x00000020
#define  ATL_MARH_OFFSET                    0x00000024

#define  ATL_ACR_OFFSET                     0x00000028     /*ATL Control Register (ACR), flush the TLB, 32bit input register*/

#define  ATL_ATCR_OFFSET                    0x0000002C     /*ATL Translation Control Register(ATCR),FMT or EPT selection, currently only FMT, 32bit in input*/

#define  ATL_ATER_OFFSET                    0x00000030     /*ATL Translation Error Register (ATER),error code for exception, 32bit output*/

#define  ATL_DVR_OFFSET                     0x00000034     /*Data Valid Register (DVR), 32bit for output*/

#define  ATL_IOTR_OFFSET                    0x00000038     /*IO translation register interface for GPA*/
#define  ATL_IOTRL_OFFSET                   0x00000038
#define  ATL_IOTRH_OFFSET                   0x0000003C

#define  ATL_MADR_OFFSET                    0x00000040     /*Memory Access Data Register (MADR), 512bit input/output*/

#define  ATL_BDFTR_OFFSET                   0x00000080     /*BDF for IO GPA translation*/

#define  ATL_MMAAR_OFFSET                   0x00000084
#define  ATL_MMAMR_OFFSET                   0x00000088

#define  ATL_AALR_OFFSET                    0x0000008C

/*Fixed Mapping Table*/
#define ATL_FMT_OFFSET                     0x00000400     /*FMT table for GPA to HPA translation*/
#define ATL_FMT_ENTRY_NUM				   16             /*each entry is 16B or 128bit*/
#define FMT_TABLE_SIZE					   32
#define FMT_TABLE_ADDR(index)              (ATL_FMT_OFFSET + FMT_TABLE_SIZE * index)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ATL_CPU_REG_ID_CR3             0
#define ATL_CPU_REG_ID_CR2             1
#define ATL_CPU_REG_ID_PKRU            2
#define ATL_CPU_REG_ID_CR0             3
#define ATL_CPU_REG_ID_PDPTE0          4
#define ATL_CPU_REG_ID_PDPTE1          5
#define ATL_CPU_REG_ID_PDPTE2          6
#define ATL_CPU_REG_ID_PDPTE3          7
#define ATL_CPU_REG_ID_CR4             8
#define ATL_CPU_REG_ID_EFER            9
#define ATL_CPU_REG_ID_RFLAGS          10

/*ATL Related CPU State*/
#define ATL_CPU_STATE_OFFSET                0x00000800     /*CPU state register for ATL*/
#define ATL_CPU_STATE_ADDR(id)              (ATL_CPU_STATE_OFFSET + (8 * id))

#define ATL_CPU_STATE_CR3L                  0x00000800     /*CR3*/
#define ATL_CPU_STATE_CR3H                  0x00000804     /*CR3*/

#define ATL_CPU_STATE_CR2L                  0x00000808     /*CR2*/
#define ATL_CPU_STATE_CR2H                  0x0000080C     /*CR2*/

#define ATL_CPU_STATE_PKRUL                 0x00000810     /*PKRU*/
#define ATL_CPU_STATE_PKRUH                 0x00000814     /*PKRU*/

#define ATL_CPU_STATE_CR0L                  0x00000818     /*CR0*/
#define ATL_CPU_STATE_CR0H                  0x0000081C     /*CR0*/

#define ATL_CPU_STATE_PDPTE_0L              0x00000820     /*PDPTE[0]*/
#define ATL_CPU_STATE_PDPTE_0H              0x00000824     /*PDPTE[0]*/
#define ATL_CPU_STATE_PDPTE_1L              0x00000828     /*PDPTE[1]*/
#define ATL_CPU_STATE_PDPTE_1H              0x0000082C     /*PDPTE[1]*/
#define ATL_CPU_STATE_PDPTE_2L              0x00000830     /*PDPTE[2]*/
#define ATL_CPU_STATE_PDPTE_2H              0x00000834     /*PDPTE[2]*/
#define ATL_CPU_STATE_PDPTE_3L              0x00000838     /*PDPTE[3]*/
#define ATL_CPU_STATE_PDPTE_3H              0x0000083C     /*PDPTE[3]*/

#define ATL_CPU_STATE_CR4L                  0x00000840     /*CR4*/
#define ATL_CPU_STATE_CR4H                  0x00000844     /*CR4*/

#define ATL_CPU_STATE_EFERL                 0x00000848     /*IA32_EFER*/
#define ATL_CPU_STATE_EFERH                 0x0000084C     /*IA32_EFER*/

#define ATL_CPU_STATE_RFLAGSL               0x00000850     /*RFLAGS*/
#define ATL_CPU_STATE_RFLAGSH               0x00000854     /*RFLAGS*/



/*Memory Mapping Table*/
#define ATL_MEM_MAP_TABLE_OFFSET          0x00001000    /*Memory mapping table registers*/
#define ATL_MMT_ENTRY_NUM		          32768
#define ATL_MMT_ENTRY_SIZE                8
#define ATL_MMT_ENTRY_ADDR(index)         (ATL_MEM_MAP_TABLE_OFFSET + (ATL_MMT_ENTRY_SIZE * index))

#define ATL_AXI_X86_MM_BASE               0x08000000

/*IO BDF translation Table*/

#define ATL_IOBDF_TABLE_OFFSET            0x00002000     /*IOBDF translation table*/
#define ATL_IOBDF_ENTRY_NUM               128
#define ATL_IOBDF_ENTRY_SZIE              32
#define ATL_IOBDF_ENTRY_ADDR(index)       (ATL_IOBDF_TABLE_OFFSET + (ATL_IOBDF_ENTRY_SZIE * index))


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Bit32u ATL_Init(); // return 0 if no error, otherwiths non 0
Bit32u ATL_Completion_Check();// check whether the ATL work has complete.
Bit32u ATL_Interrupt_handler_Register(void*func); // return 0 if no error, otherwiths non 0

Bit32u ATL_CPU_State_set(Bit32u Reg_id, Bit64u value);
Bit32u ATL_CPU_State_get(Bit32u Reg_id, Bit64u *value);

Bit32u ATL_FMT_Table_Set(Bit32u fmt_index, Bit64u gpa_base, Bit64u gpa_top, Bit64u hpa_offset);
Bit32u ATL_FMT_Table_Get(Bit32u fmt_index, Bit64u *gpa_base, Bit64u *gpa_top, Bit64u *hpa_offset);

Bit32u ATL_MM_Set(Bit32u base_addr, Bit32u mask);   //memory mapping AXI base address and length mask
Bit32u ATL_MMT_Set(Bit32u index, Bit64u x86_base);   //memory mapping table set
Bit32u ATL_MMT_Get(Bit32u index, Bit64u *x86_base);  //memory mapping table get

Bit32u ATL_AAL_Set(Bit32u len); //atl address length set, see MAXPHYADDR in SDMs.

Bit32u ATL_GLA_Translation(Bit64u Gla, Bit2u Access_Type, Bit1u us, Bit4u len, Bit1u Mod, Bit1u explicita);
Bit32u ATL_GPA_Translation(Bit64u Gpa, Bit2u Access_Type, Bit4u len, Bit1u Mod);
Bit32u ATL_HPA_Translation(Bit64u Hpa,  Bit2u Access_Type,Bit4u len);

Bit32u ATL_Exception_Check();
Bit32u ATL_MMIO_Check(); //return 1 is mmio, 0 is not mmio.

Bit32u ATL_GPA_Read(Bit64u *Gpa, Bit2u *len);

Bit32u ATL_HPA_Read(Bit64u *Hpa);

Bit32u ATL_TLB_Flush_MMT_Invalid();

Bit32u ATL_Data_Read(Bit32u len, void *data) ;
Bit32u ATL_Data_Write(Bit32u len, void *data);

Bit32u ATL_BDF_Table_Set(Bit32u bdf_index, Bit64u gpa_base, Bit64u gpa_top, Bit16u bdf);
Bit32u ATL_BDF_Table_Get(Bit32u bdf_index, Bit64u *gpa_base, Bit64u *gpa_top, Bit16u *bdf);

Bit32u ATL_GPA_BDF_Translation(Bit64u gpa, Bit16u* bdf);


#endif
