/*
 *
 *
 **/

#include <stdio.h>
#include <ctype.h>
#include "include/io.h"
#include "dma.h"

Bit32u dma_get_id(struct dw_dma_common_reg *reg){
	Bit64u id;
	id = reg_readl(reg, ID);
	return id;
}

Bit32u dma_get_comp_ver(struct dw_dma_common_reg *reg){
	Bit64u comp_rev;
	comp_rev = reg_readl(reg, CompVer);
	return comp_rev;
}

void dma_gloabe_int_enable(struct dw_dma_common_reg *reg){
	reg_writel(DMA_CFG_INT_EN, reg, CFG);
}

void dma_gloabe_int_disable(struct dw_dma_common_reg *reg){
	reg_clearl(DMA_CFG_INT_EN, reg, CFG);
}

void dma_dmac_enable(struct dw_dma_common_reg *reg){
	reg_writel(DMA_CFG_DMAC_EN, reg, CFG);
}

/* Disable dmac
 * return val:
 * 		0: dmac is disabled.
 *		1: dmac is still active, you should disable channel first.
 **/
Bit32u dma_dmac_disable(struct dw_dma_common_reg *reg){
	Bit32u val;
	reg_clearl(DMA_CFG_DMAC_EN, reg, CFG);
	val = reg_readl(reg, CFG);
	if(val & DMA_CFG_DMAC_EN)
		return 1;
	else
		return 0;
}

/*
 * DMA is enable?
 * @return value 
 *		0: Disable
 *		1: Enable
 **/
Bit32u dma_dmac_is_enable(struct dw_dma_common_reg *reg){
	return reg_readl(reg, CFG) & 1;
}

Bit32u dma_dmac_gloabe_int_is_enable(struct dw_dma_common_reg *reg){
	return reg_readl(reg, CFG) & 2;
}
/*
 * Enable special channel.
 * @ch: channel number, from 1 to DW_DMA_MAX_NR_CHANNELS
 **/
void dma_ch_enable(struct dw_dma_common_reg *reg, Bit8u ch){
	Bit32u channel = ch - 1;
	reg_writel((DMA_CH_EN << channel)|(DMA_CH_EN_WE << channel), reg, ChEn_LO);
}

/*
 * Disable special channel.
 * @ch: channel number, from 1 to DW_DMA_MAX_NR_CHANNELS
 **/
void dma_ch_disable(struct dw_dma_common_reg *reg, Bit8u ch){
	void* addr;
	Bit32u channel = ch - 1;
	Bit32u value;
	addr =  (void *)&(reg->ChEn_LO);
	value =readl(addr);
	value |= (DMA_CH_EN_WE << channel);
	value &= ~(DMA_CH_EN << channel);
	writel(value, addr);
}

/*
 * DMA special channel is enable?
 * @ch: channel number, from 1 to DW_DMA_MAX_NR_CHANNELS
 * @return value 
 *		0: 		free channel(this channel is not enable).
 *	  	other: 	busy channel.
 **/
Bit32u dma_ch_is_free(struct dw_dma_common_reg *reg, Bit8u ch){
	Bit32u channel = ch-1;
	return reg_readl(reg, ChEn_LO) & (1 << channel);
}
/*
 * Enable special channel suspend request.
 * @ch: channel number, from 1 to DW_DMA_MAX_NR_CHANNELS
 **/
void dma_ch_susp_enable(struct dw_dma_common_reg *reg, Bit8u ch){
	Bit32u channel = ch - 1;
	reg_writel((DMA_CH_SUSP << channel)|(DMA_CH_SUSP_WE << channel), reg, ChEn_LO);
}

/*
 * Disable special channel suspend request.
 * @ch: channel number, from 1 to DW_DMA_MAX_NR_CHANNELS
 **/
void dma_ch_susp_disable(struct dw_dma_common_reg *reg, Bit8u ch){
	void* addr;
	Bit32u channel = ch - 1;
	Bit32u value;
	addr =  (void *)&(reg->ChEn_LO);
	value =readl(addr);
	value |= (DMA_CH_SUSP_WE << channel);
	value &= ~(DMA_CH_SUSP << channel);
	writel(value, addr);
}

/*
 * Enable special channel abort request.
 * @ch: channel number, from 1 to DW_DMA_MAX_NR_CHANNELS
 **/
void dma_ch_abort_enable(struct dw_dma_common_reg *reg, Bit8u ch){
	Bit32u channel = ch - 1;
	reg_writel((DMA_CH_ABORT << channel)|(DMA_CH_ABORT_WE << channel), reg, ChEn_HI);
}

/*
 * Disable special channel abort request.
 * @ch: channel number, from 1 to DW_DMA_MAX_NR_CHANNELS
 **/
void dma_ch_abort_disable(struct dw_dma_common_reg *reg, Bit8u ch){
	void* addr;
	Bit32u channel = ch - 1;
	Bit32u value;

	addr =  (void *)&(reg->ChEn_HI);
	value =readl(addr);
	value |= (DMA_CH_ABORT_WE << channel);
	value &= ~(DMA_CH_ABORT << channel);
	writel(value, addr);
}

/*
 * Read Channel Intterupt status.
 **/
Bit32u dma_int_status_read(struct dw_dma_common_reg *reg){
	return reg_readl(reg, IntStat);
}

/*
 * Read special Channel Intterupt status.
 * @ch: channel number, from 1 to DW_DMA_MAX_NR_CHANNELS
 * return 0: no interrupt
 * return other value,  @channel has interrupt
 **/
Bit8u dma_chx_int_status_read(struct dw_dma_common_reg *reg, Bit8u ch){
	Bit32u channel = ch - 1;
	return reg_readl(reg, IntStat) & (DMA_CH1_INT_STAT << channel);
}

/*
 * Clear Common Register Interrupt status. 
 **/
void dma_com_int_status_clear(struct dw_dma_common_reg *reg, Bit32u val){
	//reg->ComIntClr=val;
	reg_direct_writel(val, reg, ComIntClr);
	//reg_writel(val, reg, ComIntClr);
}

/*
 * Enable Common Register Interrupt Status. 
 **/
void dma_com_int_status_enable(struct dw_dma_common_reg *reg, Bit32u val){
	reg_writel(val, reg, ComIntStatEn);
}

/*
 * Disable Common Register Interrupt Status. 
 **/
void dma_com_int_status_diaable(struct dw_dma_common_reg *reg, Bit32u val){
	reg_clearl(val, reg, ComIntStatEn);
}

/*
 * Enable Common Register Interrupt Signal. 
 **/
void dma_com_int_signal_enable(struct dw_dma_common_reg *reg, Bit32u val){
	reg_writel(val, reg, ComIntSigEn);
}

/*
 * Disable Common Register Interrupt Signal. 
 **/
void dma_com_int_signal_disable(struct dw_dma_common_reg *reg, Bit32u val){
	reg_clearl(val, reg, ComIntSigEn);
}

/*
 * Read common register Interrupt status.
 **/
Bit32u dma_com_int_status_read(struct dw_dma_common_reg *reg){
	return reg_readl(reg, ComIntStat);
}

/*
 * Reset dma.
 **/
void dma_reset(struct dw_dma_common_reg *reg){
	reg_writel(1, reg, Reset);
}

/*
 * Read Reset dma.
 **/
Bit32u dma_reset_read(struct dw_dma_common_reg *reg){
	return reg_readl(reg, Reset);
}

/*
 * Read Source Address for Channelx.
 **/
Bit32u dma_chx_sar_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, SAR);
}

/*
 * Write Source Address for Channelx.
 **/
void dma_chx_sar_write(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_direct_writel(val, reg, SAR);
}

/*
 * Read Distination Address for Channelx.
 **/
Bit32u dma_chx_dar_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, DAR);
}

/*
 * Write Distination Address for Channelx.
 **/
void dma_chx_dar_write(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_direct_writel(val, reg, DAR);
}

/*
 * Read Block Transfer Size Register for Channelx.
 **/
Bit32u dma_chx_block_ts_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, BLOCK_TS);
}

/*
 * Write Block Transfer Size Register for Channelx.
 **/
void dma_chx_block_ts_write(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_direct_writel(val, reg, BLOCK_TS);
}

/*
 * Read Control Register Low for Channelx.
 **/
Bit32u dma_chx_ctl_lo_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, CTL_LO);
}

/*
 * Write Control Register Low for Channelx.
 **/
void dma_chx_ctl_lo_write(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_writel(val, reg, CTL_LO);
}

/*
 *  Direct Write Control Register Low for Channelx.
 **/
void dma_chx_ctl_lo_direct_write(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_direct_writel(val, reg, CTL_LO);
}

/*
 * Read Control Register High for Channelx.
 **/
Bit32u dma_chx_ctl_hi_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, CTL_HI);
}

/*
 * Write Control Register High for Channelx.
 **/
void dma_chx_ctl_hi_write(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_writel(val, reg, CTL_HI);
}

/*
 * Direct Write Control Register High for Channelx.
 **/
void dma_chx_ctl_hi_direct_write(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_direct_writel(val, reg, CTL_HI);
}

/*
 * Read Config Register Low for Channelx.
 **/
Bit32u dma_chx_cfg_lo_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, CFG_LO);
}

/*
 * Write Config Register Low for Channelx.
 **/
void dma_chx_cfg_lo_write(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_writel(val, reg, CFG_LO);
}

/*
 * Direct Write Config Register Low for Channelx.
 **/
void dma_chx_cfg_lo_direct_write(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_direct_writel(val, reg, CFG_LO);
}

/*
 * Read Config Register High for Channelx.
 **/
Bit32u dma_chx_cfg_hi_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, CFG_HI);
}

/*
 * Write Config Register High for Channelx.
 **/
void dma_chx_cfg_hi_write(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_writel(val, reg, CFG_HI);
}

/*
 * direct Write Config Register High for Channelx.
 **/
void dma_chx_cfg_hi_direct_write(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_direct_writel(val, reg, CFG_HI);
}

/*
 * Read LLP register.
 **/
Bit32u dma_chx_llp_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, LLP);
}

/*
 * Write LLP register.
 **/
void dma_chx_llp_write(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_writel(val, reg, LLP);
}

/*
 * Read completed Block Transfer size.
 * Return val: no any errors, the value will be equal to BLOCK_TS 
 * of BLOCK_TS register.
 **/
Bit32u dma_chx_compled_blk_transfer_size_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, STATUSREG_LO);
}

/*
 * Read Date Left in FIFO.
 * Return val: no any errors, this value should be 0.
 * of BLOCK_TS register.
 **/
Bit32u dma_chx_data_left_in_fifo_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, STATUSREG_HI);
}

/*
 * Read Software Handshake Source Register
 **/
Bit32u dma_chx_swhs_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, SWHSSRCREG);
}

/*
 * Write Software Handshake Source Register
 **/
void dma_chx_swhs_write(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_writel(val, reg, SWHSSRCREG);
}

/*
 * Read Software Handshake Destination Register
 **/
Bit32u dma_chx_swhd_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, SWHSDSTREG);
}

/*
 * Write Software Handshake Destination Register
 **/
void dma_chx_swhd_write(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_writel(val, reg, SWHSDSTREG);
}

/*
 * Block Tranfer Reseum Request Tranfer.
 * This used for ShadowReg_Or_LLI_Invalid_ERR is generate, and
 * we want to resume transfer is as follows:
 **/
void dma_blk_tfr_resume_req(struct dw_dma_chan_reg *reg){
	reg_writel(1, reg, BLK_TFR_RESUME_REQ);
}

/*
 * Read AXI ID(ReadID/WriteID) Register
 **/
Bit32u dma_chx_axi_id_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, AXI_IDREG);
}

/*
 * Write AXI ID(ReadID/WriteID) Register
 **/
void dma_chx_axi_id_write(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_writel(val, reg, AXI_IDREG);
}

/*
 * Read AXI QOS Register
 **/
Bit32u dma_chx_axi_qos_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, AXI_QOSREG);
}

/*
 * Write AXI QOS Register
 **/
void dma_chx_axi_qos_write(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_writel(val, reg, AXI_QOSREG);
}

/*
 * Read Source Status Register
 **/
Bit32u dma_chx_sstat_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, SSTAT);
}

/*
 * Read Destination Status Register
 **/
Bit32u dma_chx_dstat_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, DSTAT);
}

/*
 * Read Source Status Fetch Address Register
 **/
Bit32u dma_chx_sstatar_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, SSTATAR);
}

/*
 * Write Source Status Fetch Address Register
 **/
void dma_chx_sstatar_write(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_writel(val, reg, SSTATAR);
}

/*
 * Read Destination Status Fetch Address Register
 **/
Bit32u dma_chx_dstatar_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, DSTATAR);
}

/*
 * Write Destination Status Fetch Address Register
 **/
void dma_chx_dstatar_write(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_writel(val, reg, DSTATAR);
}

/*
 * Enable CHx Interrupt Status Register
 **/
void dma_chx_int_status_eanble(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_writel(val, reg, INT_STAT_EN);
}

/*
 * Disable CHx Interrupt Status Register
 **/
void dma_chx_int_status_diable(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_clearl(val, reg, INT_STAT_EN);
}

/*
 * Read CHx Interrupt Status Register
 **/
Bit32u dma_chx_spec_int_status_read(struct dw_dma_chan_reg *reg){
	return reg_readl(reg, INT_STAT);
}


/*
 * Enable CHx Interrupt Signal Register
 **/
void dma_chx_int_signal_eanble(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_writel(val, reg, INTSIG_EN);
}

/*
 * Disable CHx Interrupt Status Register
 **/
void dma_chx_int_signal_diable(struct dw_dma_chan_reg *reg, Bit32u val){
	reg_clearl(val, reg, INTSIG_EN);
}

/*
 * Clear CHx Interrupt status Register
 **/
void dma_chx_int_status_clear(struct dw_dma_chan_reg *reg, Bit32u val){
	//reg->INTCLR=val;
	reg_direct_writel(val, reg, INTCLR);
	//return reg_writel(val, reg, INTCLR);
}
