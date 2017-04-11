#include "datatype.h"
#include "dma.h"
#include "misc.h"
#include "intc.h"

#if 0
/* Read lli status from register */
void LLI_DBG(void){
	printf("LLI debug:\n");
	printf("lli->llp:0x%x\n", dma_chx_llp_read(dma.chan_reg));
	printf("lli->sar:0x%x\t", dma_chx_sar_read(dma.chan_reg));
	printf("lli->dar:0x%x\n", dma_chx_dar_read(dma.chan_reg));
	printf("lli->ctl_hi:0x%x\t", dma_chx_ctl_hi_read(dma.chan_reg));
	printf("lli->ctl_lo:0x%x\n", dma_chx_ctl_lo_read(dma.chan_reg));
	printf("lli->cfg_hi:0x%x\t", dma_chx_cfg_hi_read(dma.chan_reg));
	printf("lli->cfg_lo:0x%x\n", dma_chx_cfg_lo_read(dma.chan_reg));
}

Bit32u dma_lli_init(){
	Bit32u val;
	struct dw_dma_common_reg* common_reg;

	common_reg = (struct dw_dma_common_reg*)APB_DMA_BASEADDR;
	dma.common_reg = common_reg;

	/* Disable DMA IRQ */
	dma_gloabe_int_disable(common_reg);

#if 0
	/* Verify DMA id number */
	val = dma_get_id(common_reg);
	printf("DMA ID:0x%x\n", val);
	//verify dma id and version.
	if(val != DMA_ID){
		printf("Wrong DMA ID, exit\n");
		return DMA_ERROR;
	}
#endif

	/* Verify DMA version number */
	val = dma_get_comp_ver(common_reg);
	printf("DMA VERSION:0x%x\n", val);
	if(val != DMA_VERSION){
		printf("Wrong DMA VERSION, exit\n");
		return DMA_ERROR;
	}

	dma_reset(common_reg);
	while(dma_reset_read(common_reg));

	/* Enable DMA */
	if(!dma_dmac_is_enable(common_reg))
		dma_dmac_enable(common_reg);

	return 0;
}

Bit32u dma_prepare_lli(struct dw_dma* dma){
	struct dw_dma_chan_reg * chan_reg;
	Bit32u cfg_lo=0, cfg_hi=0, ctl_lo=0, ctl_hi=0;
	struct dma_chx_config_t* config = &dma->config;
	Bit32u period;
	Bit32u i, val;
	Bit32u channel;

	/* Get free channel */
	/* channel 1 is used for spi transfer, ignore it */
	for(channel=2; channel<4; channel++){
		val = dma_ch_is_free(dma->common_reg, channel);
		//printf("channel:%d val:%d\n", channel, val);
		if(val == 0){
			//printf("Find free channel:%d\n", channel);
			chan_reg = (struct dw_dma_chan_reg*)(APB_DMA_CH(channel));
			dma->chan_reg = chan_reg;
			config->channel = channel;
			break;
		}
	}
	if(channel == 4){
		printf("Cannot find free channel.\n");
		return DMA_NO_FREE_CHANNEL;
	}

	/* Clear chx CFG to 0 */
	dma_chx_cfg_lo_direct_write(chan_reg, 0);
	dma_chx_cfg_hi_direct_write(chan_reg, 0);

	/* set chx cfg Low */
	cfg_lo |= DMA_CHX_CFG_SRC_MULTBLK_TYPE(config->src_trf_mode);
	cfg_lo |= DMA_CHX_CFG_DST_MULTBLK_TYPE(config->dst_trf_mode);
	/* set chx cfg High */
	cfg_hi |= DMA_CHX_CFG_TT_FC(config->trf_type);
	cfg_hi |= DMA_CHX_CFG_HS_SEL_SRC;	/*src software handshark */
	cfg_hi |= DMA_CHX_CFG_HS_SEL_DST;	/*dst software handshark */

	dma_chx_cfg_lo_direct_write(chan_reg, cfg_lo);
	dma_chx_cfg_hi_direct_write(chan_reg, cfg_hi);

	/* set chx CTL Low */
	ctl_lo |= DMA_CHX_CTL_SMS(config->sms);
	ctl_lo |= DMA_CHX_CTL_DMS(config->dms);
	ctl_lo |= DMA_CHX_CTL_SING(config->src_addr_mode);
	ctl_lo |= DMA_CHX_CTL_DING(config->dst_addr_mode);
	ctl_lo |= DMA_CHX_CTL_SRC_TR_WIDTH(DMA_CTL_TR_WIDTH_128);	/* src transfer width */
	ctl_lo |= DMA_CHX_CTL_DST_TR_WIDTH(DMA_CTL_TR_WIDTH_128);	/* dst transfer width */
	ctl_lo |= DMA_CHX_CTL_SRC_MSIZE(DMA_CTL_BURST_LENGTH_512);	/* src burst transaction length */
	ctl_lo |= DMA_CHX_CTL_DST_MSIZE(DMA_CTL_BURST_LENGTH_512);	/* dst burst transaction length */
	/* set chx CTL High */
	ctl_hi |= DMA_CHX_CTL_ARLEN_EN;							/* Src Burst Lenght Enable */
	ctl_hi |= DMA_CHX_CTL_AWLEN_EN;							/* Dst Burst Lenght Enable */
	ctl_hi |= DMA_CHX_CTL_ARLEN(DMA_CTL_BURST_LENGTH_4);		/* Src Burst Lenght Size */
	ctl_hi |= DMA_CHX_CTL_AWLEN(DMA_CTL_BURST_LENGTH_4);		/* Dst Burst Lenght Size */
	ctl_hi |= DMA_CHX_CTL_SHADOWREG_OR_LLI_VALID;
	//ctl_hi |= DMA_CHX_CTL_SHADOWREG_OR_LLI_LAST;
	ctl_hi |= DMA_CHX_CTL_SRC_STAT_EN;
	ctl_hi |= DMA_CHX_CTL_DST_STAT_EN;

	/* dma_chx_ctl_hi_write(chan_reg, ctl_hi); */
	/* dma_chx_ctl_lo_write(chan_reg, ctl_lo); */

	period =  config->data_size/64;
	config->lli = lli;

	for(i=0; i<period; i++){
		lli[i].block_ts = config->data_size * period;

		switch (config->dir) {
			case DMA_MEM_TO_DEV:
				lli[i].sar = config->src_addr + 0x200 * i;
				lli[i].dar = config->dst_addr;
				lli[i].ctl_lo = ctl_lo;
				lli[i].ctl_hi = ctl_hi;
				break;
			case DMA_DEV_TO_MEM:
				lli[i].sar = config->src_addr;
				lli[i].dar = config->dst_addr + 0x2000 * i;
				lli[i].ctl_lo = ctl_lo;
				lli[i].ctl_hi = ctl_hi;
				break;
			case DMA_MEM_TO_MEM:
							lli[i].sar = config->src_addr + 0x200 * i;
							lli[i].dar = config->dst_addr + 0x200 * i;
							lli[i].ctl_lo = ctl_lo;
							lli[i].ctl_hi = ctl_hi;
							break;
			default:
				break;
		}
		if(i + 1 < period)
			lli[i].llp = (Bit32u)&lli[i+1];
		else{
			lli[i].llp = (Bit32u)NULL;
			lli[i].ctl_hi |= DMA_CHX_CTL_SHADOWREG_OR_LLI_LAST;
		}
		printf("lli[%d]:0x%x\n", i, lli[i].llp);
	}
	return 0;
}

Bit32u dma_lli_start(struct dw_dma* dma){

	/*First Block */
	dma_chx_llp_write(dma->chan_reg, (Bit32u)&lli[0]);

	/* Clean Interrupt */
	//dma_chx_int_status_clear(dma->chan_reg, 0xFFFFFFFF);
	//dma_chx_int_signal_eanble(dma->chan_reg, 0xFFFFFFFF);
	//dma_chx_int_status_eanble(dma->chan_reg, 0xFFFFFFFF);

#if 0
	Bit32u val;

	/* Enable Software Handshake for SRC and DST */
	val =  DMA_CHX_SWHS_REQ_SRC | DMA_CHX_SWHS_REQ_SRC_WE;	
	//val |= DMA_CHX_SWHS_SIGREQ_SRC | DMA_CHX_SWHS_SIGREQ_SRC_WE;
	val |= DMA_CHX_SWHS_LAST_SRC | DMA_CHX_SWHS_LAST_SRC;
	dma_chx_swhs_write(dma->chan_reg, val);
	dma_chx_swhd_write(dma->chan_reg, val);
#endif

	/* enable channel*/
	dma_ch_enable(dma->common_reg, dma->config.channel);
	
	/* Software Handshake request */
	//dma_blk_tfr_resume_req(dma->chan_reg);

	return 0;
}

Bit32u dma_lli_stop(struct dw_dma* dma){
	dma_ch_disable(dma->common_reg, dma->config.channel);
	return 0;
}


Bit32u dma_intc_init(void)
{

	/*enable CommonReg irq*/
	dma_com_int_signal_enable(dma.common_reg, DMA_COM_INT_ALL);
	dma_com_int_status_enable(dma.common_reg, DMA_COM_INT_ALL);
	/*enable channel irq*/
	dma_chx_int_signal_eanble(dma.chan_reg,DMA_CHX_CH_ALL_INT);
	dma_chx_int_status_eanble(dma.chan_reg,DMA_CHX_CH_ALL_INT);

#if 0
	dma_chx_int_status_eanble(dma->chan_reg, DMA_CHX_SR_OR_LLI_INV_ERR_INT|
			DMA_CHX_BLOCK_TFR_DONE_INT|
			DMA_CHX_DMA_TFR_DONE_INT);
	dma_chx_int_signal_eanble(dma->chan_reg, DMA_CHX_SR_OR_LLI_INV_ERR_INT|
			DMA_CHX_BLOCK_TFR_DONE_INT|
			DMA_CHX_DMA_TFR_DONE_INT);
#endif

	/* enable dmac interrupt */
	if(!dma_dmac_gloabe_int_is_enable(dma.common_reg))
		dma_gloabe_int_enable(dma.common_reg);

	/* register dma isr */
    CK_INTC_RequestIrq(CK_INTC_DMAC, CK_DMAC_Handler);

	return 0;
}

int test_dma_lli()
{
	printf("dma lli test begins.\n");
	struct dma_chx_config_t* config;
	Bit32u val;
	Bit32u* srcaddr = (Bit32u*)0x300000;
	Bit32u* dstaddr = (Bit32u*)0x380000;
	Bit32u i;
	Bit32u* psrcaddr;
	Bit32u* pdstaddr;

	config = &dma.config;
	val = dma_lli_init();
	if(val < 0){
		printf("dma init error.\n");
		return -1;
	}

    config->dir = DMA_MEM_TO_MEM;
    config->data_size = 0x600;			/* total size to be transfer */
	config->sms = DMX_AXI_MASTER_1;
	config->dms = DMX_AXI_MASTER_2;
    config->src_addr = (Bit32u)srcaddr;
    config->dst_addr = (Bit32u)dstaddr;
    config->src_addr_mode = DMA_ADDR_INC;
    config->dst_addr_mode = DMA_ADDR_INC;
    config->trf_type = DMA_CFG_MEM2MEM_WITH_DMA_CTRL;
    config->src_trf_mode = DMA_MULTI_BLOCK_LINKEDLIST;
    config->dst_trf_mode = DMA_MULTI_BLOCK_LINKEDLIST;

    psrcaddr = srcaddr;
    pdstaddr = dstaddr;
    for(i=0; i<config->data_size; i++){
		*psrcaddr++ = i;
		*pdstaddr++ = 0;
    }

    dma_intc_init();
	dma_prepare_lli(&dma);
	dma_lli_start(&dma);

	dma_lli_stop(&dma);
	CK_INTC_FreeIrq(CK_INTC_DMAC);
	printf("dma lli test sucess.\n");
	return 0;
}
void CK_DMAC_Handler(CK_UINT32 irqid)
{
    printf("dmac have interrupt\n");
    Bit32u temp,temp1,i;
    temp = dma_int_status_read(dma.common_reg)&0x1FFFF;
    if(temp&DMA_COMREG_INT_STAT)   /*Common*/
    {
    	 printf(" dmac CommonReg interrupt\n");
    	 temp1 = dma_com_int_status_read(dma.common_reg)&0x1FF;
    	 switch (temp1)
    	 {
			 case DMA_COM_INT_DEC_ERR:
				 printf("  Enable_SLVIF_CommonReg_DEC_ERR_IntStat\n");
				 dma_com_int_status_clear(dma.common_reg,DMA_COM_INT_DEC_ERR);
				 break; /* clear DMA_COM_INT_DEC_ERR interrupt */
			 case DMA_COM_INT_WR2RO_ERR:
				 printf("  Enable_SLVIF_CommonReg_WR2RO_ERR_IntStat\n");
				 dma_com_int_status_clear(dma.common_reg,DMA_COM_INT_WR2RO_ERR);
				break; /* clear DMA_COM_INT_WR2RO_ERR interrupt */
			 case DMA_COM_INT_RD2WO_ERR:
				 printf("  Enable_SLVIF_CommonReg_RD2WO_ERR_IntStat\n");
				 dma_com_int_status_clear(dma.common_reg,DMA_COM_INT_RD2WO_ERR);
				 break; /* clear DMA_COM_INT_RD2WO_ERR interrupt */
			 case DMA_COM_INT_WRONHOLD_ERR:
				 printf("  Enable_SLVIF_CommonReg_WrOnHold_ERR_IntStat\n");
				 dma_com_int_status_clear(dma.common_reg,DMA_COM_INT_WRONHOLD_ERR);
				 break; /* clear DMA_COM_INT_WRONHOLD_ERR interrupt */
			 case DMA_COM_INT_UN_DEC_ERR:
				 printf("  Enable_SLVIF_UndefinedReg_DEC_ERR_IntStat\n");
				 dma_com_int_status_clear(dma.common_reg,DMA_COM_INT_UN_DEC_ERR);
				 break; /* clear DMA_COM_INT_UN_DEC_ERR interrupt */
			 default:break;
    	 }
    }
    else if(temp&0xFF)        /*Chx IT*/
    {
    	for(i=0;i<8;i++)
    	{
    		if(temp&i);
				printf(" dmac CH%d interrupt\n",i);
				temp1=dma_chx_spec_int_status_read(dma.chan_reg);
				switch (temp1)
				{
					case DMA_CHX_BLOCK_TFR_DONE_INT:
						 printf("  BLOCK_TFR_DONE_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_BLOCK_TFR_DONE_INT);
						 break; /* clear DMA_CHX_BLOCK_TFR_DONE_INT interrupt */
					case DMA_CHX_DMA_TFR_DONE_INT:
						 printf("  DMA_TFR_DONE_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_DMA_TFR_DONE_INT);
						 break; /* clear DMA_CHX_DMA_TFR_DONE_INT interrupt */
					case DMA_CHX_SRC_TRC_INT:
						 printf("  DMA_CHX_SRC_TRC_INT\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_SRC_TRC_INT);
						 break; /* clear DMA_CHX_SRC_TRC_INT interrupt */
					case DMA_CHX_DST_TRC_INT:
						 printf("  DST_TransComp_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_DST_TRC_INT);
						 break; /* clear DMA_CHX_DST_TRC_INT interrupt */
					case DMA_CHX_SRC_DEC_ERR_INT:
						 printf("  SRC_DEC_ERR_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_SRC_DEC_ERR_INT);
						 break; /* clear DMA_CHX_SRC_DEC_ERR_INT interrupt */
					case DMA_CHX_DST_DEC_ERR_INT:
						 printf("  DST_DEC_ERR_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_DST_DEC_ERR_INT);
						 break; /* clear DMA_CHX_DST_DEC_ERR_INT interrupt */
					case DMA_CHX_SRC_SLV_ERR_INT:
						 printf("  SRC_SLV_ERR_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_SRC_SLV_ERR_INT);
						 break; /* clear DMA_CHX_SRC_SLV_ERR_INT interrupt */
					case DMA_CHX_DST_SLV_ERR_INT:
						 printf("  DST_SLV_ERR_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_DST_SLV_ERR_INT);
						 break; /* clear DMA_CHX_DST_SLV_ERR_INT interrupt */
					case DMA_CHX_RD_DEC_ERR_INT:
						 printf("  LLI_RD_DEC_ERR_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_RD_DEC_ERR_INT);
						 break; /* clear DMA_CHX_RD_DEC_ERR_INT interrupt */
					case DMA_CHX_WR_DEC_ERR_INT:
						 printf("  LLI_WR_DEC_ERR_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_WR_DEC_ERR_INT);
						 break; /* clear DMA_CHX_WR_DEC_ERR_INT interrupt */
					case DMA_CHX_RD_SLV_ERR_INT:
						 printf("  LLI_RD_SLV_ERR_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_RD_SLV_ERR_INT);
						 break; /* clear DMA_CHX_RD_SLV_ERR_INT interrupt */
					case DMA_CHX_WR_SLV_ERR_INT:
						 printf("  LLI_WR_SLV_ERR_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_WR_SLV_ERR_INT);
						 break; /* clear DMA_CHX_WR_SLV_ERR_INT interrupt */
					case DMA_CHX_SR_OR_LLI_INV_ERR_INT:
						 printf("  ShadowReg_Or_LLI_Invalid_ERR_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_SR_OR_LLI_INV_ERR_INT);
						 break; /* clear DMA_CHX_SR_OR_LLI_INV_ERR_INT interrupt */
					case DMA_CHX_SLVIF_MBT_ERR_INT:
						 printf("  SLVIF_MultiBlkType_ERR_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_SLVIF_MBT_ERR_INT);
						 break; /* clear DMA_CHX_SLVIF_MBT_ERR_INT interrupt */
					case DMA_CHX_SLVIF_DEC_ERR_INT:
						 printf("  SLVIF_DEC_ERR_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_SLVIF_DEC_ERR_INT);
						 break; /* clear DMA_CHX_SLVIF_DEC_ERR_INT interrupt */
					case DMA_CHX_SLVIF_WR2RO_ERR_INT:
						 printf("  SLVIF_WR2RO_ERR_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_SLVIF_WR2RO_ERR_INT);
						 break; /* clear DMA_CHX_SLVIF_WR2RO_ERR_INT interrupt */
					case DMA_CHX_SLVIF_RD2RWO_ERR_INT:
						 printf("  SLVIF_RD2WO_ERR_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_SLVIF_RD2RWO_ERR_INT);
						 break; /* clear DMA_CHX_SLVIF_RD2RWO_ERR_INT interrupt */
					case DMA_CHX_SLVIF_WRONCHEN_ERR_INT:
						 printf("  SLVIF_WrOnChEn_ERR_ntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_SLVIF_WRONCHEN_ERR_INT);
						 break; /* clear DMA_CHX_SLVIF_WRONCHEN_ERR_INT interrupt */
					case DMA_CHX_SLVIF_SR_WRON_VALID_ERR_INT:
						 printf("  SLVIF_ShadowReg_WrOnValid_ERR_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_SLVIF_SR_WRON_VALID_ERR_INT);
						 break; /* clear DMA_CHX_SLVIF_SR_WRON_VALID_ERR_INT interrupt */
					case DMA_CHX_SLVIF_WRONHODL_ERR_INT:
						 printf("  SLVIF_WrOnHold_ERR_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_SLVIF_WRONHODL_ERR_INT);
						 break; /* clear DMA_CHX_SLVIF_WRONHODL_ERR_INT interrupt */
					case DMA_CHX_CHLOCK_CLEARED_INT:
						 printf("  ChLock_Cleared_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_CHLOCK_CLEARED_INT);
						 break; /* clear DMA_CHX_CHLOCK_CLEARED_INT interrupt */
					case DMA_CHX_CH_SRC_SUSPENDED_INT:
						 printf("  CH_SRC_SUSPENDED_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_CH_SRC_SUSPENDED_INT);
						 break; /* clear DMA_CHX_CH_SRC_SUSPENDED_INT interrupt */
					case DMA_CHX_CH_SUSPENDED_INT:
						 printf("  CH_SUSPENDED_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_CH_SUSPENDED_INT);
						 break; /* clear DMA_CHX_CH_SUSPENDED_INT interrupt */
					case DMA_CHX_CH_DISABLED_INT:
						 printf("  CH_DISABLED_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_CH_DISABLED_INT);
						 break; /* clear DMA_CHX_CH_DISABLED_INT interrupt */
					case DMA_CHX_CH_ABORTERD_INT:
						 printf("  CH_ABORTED_IntStat\n");
						 dma_chx_int_status_clear(dma.chan_reg,DMA_CHX_CH_ABORTERD_INT);
						 break; /* clear DMA_CHX_CH_ABORTERD_INT interrupt */
					default:break;
				}
    	}
    }
    	else
    		return;
}
#endif
