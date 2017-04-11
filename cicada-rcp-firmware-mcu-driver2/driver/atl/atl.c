#include "atl.h"

Bit32u ATL_Init() // return 0 if no error, otherwiths non 0
{
     *(Bit32u *)ATL_ACR_OFFSET = 0x7;
     return 0;
}
Bit32u ATL_Completion_Check()// check whether the ATL work has complete.
{
	Bit32u ater = *(Bit32u *)ATL_ATER_OFFSET;
     return (ater >> 10) & 0x1;
}
Bit32u ATL_Interrupt_handler_Register(void*func) // return 0 if no error, otherwiths non 0
{
    //register handler to interrupt;
	return 0;
}

Bit32u ATL_CPU_State_set(Bit32u Reg_id, Bit64u value)
{
	 *(Bit64u *)(ATL_CPU_STATE_ADDR(Reg_id)) = value;
	 return 0;
}
Bit32u ATL_CPU_State_get(Bit32u Reg_id, Bit64u *value)
{
	*value = *(Bit64u *)(ATL_CPU_STATE_ADDR(Reg_id)) ;
    return 0;
}

Bit32u ATL_FMT_Table_Set(Bit32u fmt_index, Bit64u gpa_base, Bit64u gpa_top, Bit64u hpa_offset)
{
	*(Bit64u*)(ATL_MMT_ENTRY_ADDR(fmt_index)) = gpa_base;
	*(Bit64u*)(ATL_MMT_ENTRY_ADDR(fmt_index) + ATL_MMT_ENTRY_SIZE) = gpa_top;
	*(Bit64u*)(ATL_MMT_ENTRY_ADDR(fmt_index) + ATL_MMT_ENTRY_SIZE * 2) = hpa_offset;
	*(Bit32u*)(ATL_MMT_ENTRY_ADDR(fmt_index) + ATL_MMT_ENTRY_SIZE * 3) = 1;
	return 0;
}
Bit32u ATL_FMT_Table_Get(Bit32u fmt_index, Bit64u *gpa_base, Bit64u *gpa_top, Bit64u *hpa_offset)
{
	*gpa_base = *(Bit64u*)(ATL_MMT_ENTRY_ADDR(fmt_index));
	*gpa_top  =	*(Bit64u*)(ATL_MMT_ENTRY_ADDR(fmt_index) + ATL_MMT_ENTRY_SIZE);
	*hpa_offset = *(Bit64u*)(ATL_MMT_ENTRY_ADDR(fmt_index) + ATL_MMT_ENTRY_SIZE * 2);

	return *(Bit32u*)(ATL_MMT_ENTRY_ADDR(fmt_index) + ATL_MMT_ENTRY_SIZE * 3);
}

Bit32u ATL_MM_Set(Bit32u base_addr, Bit32u mask)
{
	*(Bit32u *)(ATL_MMAAR_OFFSET) = base_addr;
	*(Bit32u *)(ATL_MMAMR_OFFSET) = mask;
	return 0;
}

Bit32u ATL_MMT_Set(Bit32u index, Bit64u x86_base)
{
	Bit64u wr_data = (x86_base & 0xFFFFFFFFFFFFF000) | 0x1;
	*(Bit64u *)(ATL_MMT_ENTRY_ADDR(index)) = wr_data;
	return 0;
}
Bit32u ATL_MMT_Get(Bit32u index, Bit64u *x86_base)
{
	Bit64u data = *(Bit64u *)(ATL_MMT_ENTRY_ADDR(index));
	*x86_base = (data & 0xFFFFFFFFFFFFF000);
	return (Bit32u)(data & 0x1);
}

Bit32u ATL_AAL_Set(Bit32u len)
{
	*(Bit32u *)(ATL_AALR_OFFSET) = len;
	return 0;
}

Bit32u ATL_GLA_Translation(Bit64u Gla, Bit2u Access_Type, Bit1u us, Bit4u len, Bit1u Mod, Bit1u explicita)
{
    Bit32u data_h = (((Bit32u)len << 22) & 0xF) | (((Bit32u)explicita << 21)& 0x1) | (((Bit32u)Mod << 20)& 0x1) | (((Bit32u)us << 19)& 0x1) |
    		        (((Bit32u)Access_Type << 17)& 0x3) | (((Bit32u)Gla > 31)& 0x1FFFF);
    Bit32u data_l = (((Bit32u)Gla << 1)& 0x7FFFFFFF) | 0x1;
    *(Bit32u *) ATL_GLARH_OFFSET = data_h;
    *(Bit32u *) ATL_GLARL_OFFSET = data_l;
    return 0;
}
Bit32u ATL_GPA_Translation(Bit64u Gpa, Bit2u Access_Type, Bit4u len, Bit1u Mod)
{
    Bit32u data_h = (((Bit32u)len << 24) & 0xF) |  (((Bit32u)Mod << 23)& 0x1) |
    		        (((Bit32u)Access_Type << 21)& 0x3) | (((Bit32u)Gpa > 31)& 0x1FFFF);
    Bit32u data_l = (((Bit32u)Gpa << 1)& 0x7FFFFFFF) | 0x1;
    *(Bit32u *) ATL_EPPTR_OFFSET = data_h;
    *(Bit32u *) ATL_EPPTR_OFFSET = data_l;
    return 0;
}
Bit32u ATL_HPA_Translation(Bit64u Hpa,  Bit2u Access_Type,Bit4u len)
{
	Bit32u data_h = (((Bit32u)Hpa > 20)& 0xFFFFFFF);
	Bit32u data_l = (((Bit32u)Hpa << 12)& 0xFFFFF) | (((Bit32u)Access_Type << 5)& 0x1) |(((Bit32u)len << 1) & 0xF)  | 0x1;
	*(Bit32u *) ATL_MARL_OFFSET = data_h;
    *(Bit32u *) ATL_MARH_OFFSET = data_l;
    return 0;
}

Bit32u ATL_Exception_Check()
{
	return *(Bit32u *)ATL_ATER_OFFSET & 0xFF;
}
Bit32u ATL_MMIO_Check() //return 1 is mmio, 0 is not mmio.
{
	return ((*(Bit32u *)ATL_ATER_OFFSET) >> 8) & 0x1;
}

Bit32u ATL_GPA_Read(Bit64u *Gpa, Bit2u *len)
{
	*Gpa = (((*(Bit64u *)ATL_GPARH_OFFSET) & 0xFFFFFFFF) << 32) | (((*(Bit64u *)ATL_GPARL_OFFSET)) & 0xFFFFF000);
    *len = ((*(Bit32u *)ATL_GPARL_OFFSET) >> 1) & 0x3;
	return 0;
}

Bit32u ATL_HPA_Read(Bit64u *Hpa)
{
	*Hpa =  (((*(Bit64u *)ATL_HPARH_OFFSET) & 0xFFFFFFFF) << 32) | ((*(Bit64u *)ATL_HPARL_OFFSET)) & 0xFFFFF000;
	return 0;
}

Bit32u ATL_TLB_Flush_MMT_Invalid()
{
	 *(Bit32u *)ATL_ACR_OFFSET = 0x3;
     return 0;
}

Bit32u ATL_Data_Read(Bit32u len, void *data)
{
   memcpy(data, ATL_MADR_OFFSET, len);
   return 0;
}
Bit32u ATL_Data_Write(Bit32u len, void *data)
{
	memcpy(ATL_MADR_OFFSET,data, len);
	return 0;
}

Bit32u ATL_BDF_Table_Set(Bit32u bdf_index, Bit64u gpa_base, Bit64u gpa_top, Bit16u bdf)
{
	*(Bit64u*)(ATL_IOBDF_ENTRY_ADDR(bdf_index)) = gpa_base;
    *(Bit64u*)(ATL_IOBDF_ENTRY_ADDR(bdf_index) + 8) = gpa_top;
	*(Bit16u*)(ATL_IOBDF_ENTRY_ADDR(bdf_index) + 16) = bdf;
	*(Bit32u*)(ATL_IOBDF_ENTRY_ADDR(bdf_index) + 24) = 1;
	return 0;
}
Bit32u ATL_BDF_Table_Get(Bit32u bdf_index, Bit64u *gpa_base, Bit64u *gpa_top, Bit16u *bdf)
{
	*gpa_base = *(Bit64u*)(ATL_IOBDF_ENTRY_ADDR(bdf_index));
	*gpa_top =  *(Bit64u*)(ATL_IOBDF_ENTRY_ADDR(bdf_index) + 8);
	*bdf = 	*(Bit16u*)(ATL_IOBDF_ENTRY_ADDR(bdf_index) + 16);
	return  *(Bit32u*)(ATL_IOBDF_ENTRY_ADDR(bdf_index) + 24);
}

Bit32u ATL_GPA_BDF_Translation(Bit64u gpa, Bit16u *bdf)
{
    Bit32u datah,datal;
    datah = (gpa >> 31) & 0x1FFFFF;
    datal = (gpa & 0x7FFFFFFF) << 1 | 0x1;
    *(Bit32u*)(ATL_IOTRH_OFFSET) =datah;
    *(Bit32u*)(ATL_IOTRL_OFFSET) =datal;

    while(!(*(Bit32u*)(ATL_IOTRL_OFFSET) & 0x1));

    return *(Bit32u*)(ATL_IOTRL_OFFSET);
}
