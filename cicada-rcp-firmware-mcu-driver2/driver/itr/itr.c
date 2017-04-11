#include "sim.h"
#include "itr.h"

/*HTL Functions*/
Bit32u ITR_WM_HTL_Access(Bit48u HPA_Address, Bit3u Access_Type, void* data_buf, void* Byte_valid)
{
	if(Access_Type == ITR_HTL_ACCESS_CTRL_WM_READ || Access_Type == ITR_HTL_ACCESS_CTRL_FM_READ)
	{
		pcie_mem_write(ITR_CSR_HTL_ACCESS_ADDR, 8, &HPA_Address);
		pcie_mem_write(ITR_CSR_HTL_ACCESS_CTRL, 4, &Access_Type);
		pcie_mem_read(ITR_CSR_HTL_BYTES_VALID, 8, Byte_valid);
		pcie_mem_read(ITR_CSR_HTL_DATA_LOW, 64, data_buf);
		return 1;
	}
	else if(Access_Type == ITR_HTL_ACCESS_CTRL_WM_WRITE || Access_Type == ITR_HTL_ACCESS_CTRL_FILL_WRITE)
	{
		pcie_mem_write(ITR_CSR_HTL_BYTES_VALID, 8, Byte_valid);
		pcie_mem_write(ITR_CSR_HTL_DATA_LOW, 64, data_buf);
		pcie_mem_write(ITR_CSR_HTL_ACCESS_ADDR, 8, &HPA_Address);
		pcie_mem_write(ITR_CSR_HTL_ACCESS_CTRL, 4, &Access_Type);
		return 1;
	}
	else 
	{
		/*FIXME: Error*/
		return 0;
	}
}

Bit32u ITR_Get_HTL_Alist_Count(void)
{
	Bit32u alist_cnt = 0;
	pcie_mem_read(ITR_CSR_HTL_ALIST_CNT, 4, &alist_cnt);
	return alist_cnt;
}

Bit32u ITR_Get_HTL_Alist_Base(void)
{
	Bit32u alist_base = 0;
	pcie_mem_read(ITR_CSR_HTL_ALIST_BASE, 4, &alist_base);
	return alist_base;
}

Bit32u ITR_HTL_Reinit(void)
{
	/*Waiting for ITR spec*/
	return 1;
}

/*PTLD/PTLT Functions*/
Bit32u ITR_TLP_Fetch(Bit8u Socket, Bit8u group_id, Bit8u link_id, Bit8u buf_id, Bit32u len, void* tlp_buffer)
{
	/*Waiting for PCM to give the mapping of BDF and link_id*/

	Bit32u len_left = len;
	Bit8u* buf_p = (Bit8u *)tlp_buffer;

	Bit64u io_tlp_buffer_lo = ITR_CSR_PTLD_IO_BUFFER_BASE + group_id*0x8000000 + buf_id * 0x40 + (buf_id >> 2) * 0x10000;
	Bit64u io_tlp_buffer_hi = Socket << 1;

	while(len_left > 32)
	{
		pcie_mem_read(io_tlp_buffer_hi<<32 | io_tlp_buffer_lo, 32, buf_p);
		len_left -= 32;
		buf_p += 32;
	}
	pcie_mem_read(io_tlp_buffer_hi<<32 | io_tlp_buffer_lo, len_left, buf_p);
	return 1;
}

/*ITR control & status*/
Bit32u ITR_Overflow_Check(void)
{
	Bit32u ctrl_val = 0;
	Bit32u overflow = 0;
	/*PTLD UP*/
	pcie_mem_read(ITR_CSR_PTLD_UP_CTRL, 4, &ctrl_val);
	overflow |= ctrl_val;
	/*PTLD Down*/
	pcie_mem_read(ITR_CSR_PTLD_DOWN_CTRL, 4, &ctrl_val);
	overflow |= ctrl_val;
	/*PTLT0 UP*/
	pcie_mem_read(ITR_CSR_PTLT0_UP_CTRL, 4, &ctrl_val);
	overflow |= ctrl_val;
	/*PTLT0 Down*/
	pcie_mem_read(ITR_CSR_PTLT0_DOWN_CTRL, 4, &ctrl_val);
	overflow |= ctrl_val;

	/*PTLT1 UP*/
	pcie_mem_read(ITR_CSR_PTLT1_UP_CTRL, 4, &ctrl_val);
	overflow |= ctrl_val;
	/*PTLT0 Down*/
	pcie_mem_read(ITR_CSR_PTLT1_DOWN_CTRL, 4, &ctrl_val);
	overflow |= ctrl_val;
	/*PTLT2 UP*/
	pcie_mem_read(ITR_CSR_PTLT2_UP_CTRL, 4, &ctrl_val);
	overflow |= ctrl_val;
	/*PTLT0 Down*/
	pcie_mem_read(ITR_CSR_PTLT2_DOWN_CTRL, 4, &ctrl_val);
	overflow |= ctrl_val;

	if(overflow & (1 << ITR_LINK_OVERFLOW_BIT))
		return 1;
	else
		return 0;

}
Bit32u ITR_IO_Trace_Reinit(void)
{
	Bit32u reinit = 0;
	/*PTLD UP*/
	pcie_mem_read(ITR_CSR_PTLD_UP_CTRL, 4, &reinit);
	reinit |= 1 << ITR_LINK_RE_INIT_BIT;
	pcie_mem_write(ITR_CSR_PTLD_UP_CTRL, 4, &reinit);
	/*PTLD Down*/
	pcie_mem_read(ITR_CSR_PTLD_DOWN_CTRL, 4, &reinit);
	reinit |= 1 << ITR_LINK_RE_INIT_BIT;
	pcie_mem_write(ITR_CSR_PTLD_DOWN_CTRL, 4, &reinit);

	/*PTLT0 UP*/
	pcie_mem_read(ITR_CSR_PTLT0_UP_CTRL, 4, &reinit);
	reinit |= 1 << ITR_LINK_RE_INIT_BIT;
	pcie_mem_write(ITR_CSR_PTLT0_UP_CTRL, 4, &reinit);
	/*PTLT0 Down*/
	pcie_mem_read(ITR_CSR_PTLT0_DOWN_CTRL, 4, &reinit);
	reinit |= 1 << ITR_LINK_RE_INIT_BIT;
	pcie_mem_write(ITR_CSR_PTLT0_DOWN_CTRL, 4, &reinit);

	/*PTLT1 UP*/
	pcie_mem_read(ITR_CSR_PTLT1_UP_CTRL, 4, &reinit);
	reinit |= 1 << ITR_LINK_RE_INIT_BIT;
	pcie_mem_write(ITR_CSR_PTLT1_UP_CTRL, 4, &reinit);
	/*PTLT0 Down*/
	pcie_mem_read(ITR_CSR_PTLT1_DOWN_CTRL, 4, &reinit);
	reinit |= 1 << ITR_LINK_RE_INIT_BIT;
	pcie_mem_write(ITR_CSR_PTLT1_DOWN_CTRL, 4, &reinit);

	/*PTLT2 UP*/
	pcie_mem_read(ITR_CSR_PTLT2_UP_CTRL, 4, &reinit);
	reinit |= 1 << ITR_LINK_RE_INIT_BIT;
	pcie_mem_write(ITR_CSR_PTLT2_UP_CTRL, 4, &reinit);
	/*PTLT0 Down*/
	pcie_mem_read(ITR_CSR_PTLT2_DOWN_CTRL, 4, &reinit);
	reinit |= 1 << ITR_LINK_RE_INIT_BIT;
	pcie_mem_write(ITR_CSR_PTLT2_DOWN_CTRL, 4, &reinit);

	return 1;
}

Bit8u hsdimma_num = 0, hsdimmb_num = 0;
Bit8u hsdimma[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
Bit8u hsdimmb[12] = {0,0,0,0,0,0,0,0,0,0,0,0};

Bit32u MTR_Init(void)
{
	Bit8u index = 0;
	Bit32u hsdimm_idx;
	pcie_mem_read(ITR_CSR_MTRA_HSDIMM_NUM, 4, &hsdimma_num);
	pcie_mem_read(ITR_CSR_MTRB_HSDIMM_NUM, 4, &hsdimmb_num);

    for(index = 0; index < ITR_CSR_MTR_PCIE_HSDIMM_NUM; index++)
    {
    	pcie_mem_read(ITR_CSR_MTRA_PCIE_HSDIMM_ID + index * 4, 4, &hsdimm_idx);
    	hsdimma[hsdimm_idx & 0xF] = 1;
    }
    for(index = 0; index < ITR_CSR_MTR_PCIE_HSDIMM_NUM; index++)
    {
    	pcie_mem_read(ITR_CSR_MTRB_PCIE_HSDIMM_ID + index * 4, 4, &hsdimm_idx);
    	hsdimmb[hsdimm_idx & 0xF] = 1;
    }
}


/*MTR control & status*/
Bit32u MTR_Trace_Start(void)
{

	Bit8u index = 0;
	Bit32u mtr_start = 0;
	Bit32u mtr_trace = 0;

	for(index = 0; index < 12; index++)
	{
		if(!hsdimma[index]) continue;
		mtr_start = (1 << ITR_MTR_TRACE_START_READ) | (1 << ITR_MTR_TRACE_START_WRITE) | (index << ITR_MTR_TRACE_START_IDX_SHIFT);
		pcie_mem_write(ITR_CSR_MTRA_TRACE_START, 4, &mtr_start);
		mtr_trace = 0;
		pcie_mem_read(ITR_CSR_MTRA_TRACING_VEC, 4, &mtr_trace);
		while(!(mtr_trace & (1 << index)));
	}


	for(index = 0; index < 12; index++)
	{
		if(!hsdimmb[index]) continue;
		mtr_start = (1 << ITR_MTR_TRACE_START_READ) | (1 << ITR_MTR_TRACE_START_WRITE) | (index << ITR_MTR_TRACE_START_IDX_SHIFT);
		pcie_mem_write(ITR_CSR_MTRB_TRACE_START, 4, &mtr_start);
		mtr_trace = 0;
		pcie_mem_read(ITR_CSR_MTRB_TRACING_VEC, 4, &mtr_trace);
		while(!(mtr_trace & (1 << index)));
	}
	return 1;
}
Bit32u MTR_Trace_Stop(void)
{
	Bit8u index = 0;
	Bit32u mtr_start = 0;
	Bit32u mtr_end = 0;
	Bit32u mtr_trace = 0;

	for(index = 0; index < 12; index++)
	{
		if(!hsdimmb[index]) continue;
		mtr_end = (index << ITR_MTR_TRACE_STOP_IDX_SHIFT);
		pcie_mem_write(ITR_CSR_MTRA_TRACE_STOP, 4, &mtr_end);
		mtr_trace = 0xFFF;
		pcie_mem_read(ITR_CSR_MTRA_TRACING_VEC, 4, &mtr_trace);
		while(mtr_trace & (1 << index));
	}

	for(index = 0; index < 12; index++)
	{
		if(!hsdimmb[index]) continue;
		mtr_end = (index << ITR_MTR_TRACE_STOP_IDX_SHIFT);
		pcie_mem_write(ITR_CSR_MTRB_TRACE_STOP, 4, &mtr_end);
		mtr_trace = 0xFFF;
		pcie_mem_read(ITR_CSR_MTRB_TRACING_VEC, 4, &mtr_trace);
		while(mtr_trace & (1 << index));
	}
	return 1;
}

Bit32u MTR_Overflow_Check(void)
{
	Bit32u index = 0;
    Bit32u addr = MTR_TRACE_MGR_OFF;
    Bit32u data = 0;
    Bit32u hsdimm = 0;
	for(index = 0; index < 12; index++)
	{
			if(!hsdimma[index]) continue;
			hsdimm = (index & ITR_MTR_CFG_MASK_OFF) << ITR_MTR_CFG_IDX_OFF;
			pcie_mem_write(ITR_CSR_MTRA_CFG_CTL, 4, &hsdimm);
			pcie_mem_write(ITR_CSR_MTRA_CFG_ADDR, 4, &addr);
			pcie_mem_read(ITR_CSR_MTRA_CFG_DATA, 4, &data);
			if((data >> MTR_TRACE_FULL_OFF) & MTR_TRACE_FUll_MASK)
				return 1;
	}
	for(index = 0; index < 12; index++)
	{
			if(!hsdimmb[index]) continue;
			hsdimm = (index & ITR_MTR_CFG_MASK_OFF) << ITR_MTR_CFG_IDX_OFF;
			pcie_mem_write(ITR_CSR_MTRB_CFG_CTL, 4, &hsdimm);
			pcie_mem_write(ITR_CSR_MTRB_CFG_ADDR, 4, &addr);
			pcie_mem_read(ITR_CSR_MTRB_CFG_DATA, 4, &data);
			if((data >> MTR_TRACE_FULL_OFF) & MTR_TRACE_FUll_MASK)
				return 1;
	}
	return 0;
}


/*DMI control and BIOS fetch*/
Bit32u ITR_PCH_Link_Setup();
Bit32u ITR_PCH_Link_Release();

Bit32u ITR_BIOS_Fetch(Bit32u addr, Bit32u len, void* buffer)
{
	Bit32u len_left = len;
	Bit8u* buf_p = (Bit8u *)buffer;
	Bit32u addr_p = addr;
	while(len_left > 32)
	{
		pcie_mem_write(ITR_CSR_PCH_REQ_ADDR, 4, &addr_p);
		pcie_mem_read(ITR_CSR_PCH_DATA, 32, buf_p);
		len_left -= 32;
		buf_p += 32;
		addr_p += 32;
	}
	pcie_mem_write(ITR_CSR_PCH_REQ_ADDR, 4, &addr_p);
	pcie_mem_read(ITR_CSR_PCH_DATA, len_left, buf_p);
	return len;
}
