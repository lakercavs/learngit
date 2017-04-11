/*
 * dma-lli-allchan.c
 *
 *  Created on: 2016-12-1
 *      Author: i
 */
#include "datatype.h"
#include "dma.h"
#include "misc.h"
#include "intc.h"

/* Read lli status from register */
/*
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
*/

Bit8u channelid = 0;

Bit32u dma_lli_init(){
	Bit32u val;
	struct dw_dma_common_reg* common_reg;

	common_reg = (struct dw_dma_common_reg*)APB_DMA_BASEADDR;
	dma.common_reg = common_reg;

	/* Disable DMA IRQ */
	dma_gloabe_int_disable(common_reg);

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

Bit32u dma_prepare_lli_1(struct dw_dma* dma,Bit8u chanid)
{
	Bit32u cfg_lo=0, cfg_hi=0, ctl_lo=0, ctl_hi=0;
	Bit32u period;
	Bit32u i;
	struct dw_dma_chan_reg * chan_reg;
	struct dma_chx_config_t* config = &dma->config[chanid];

	chan_reg = (struct dw_dma_chan_reg*)(APB_DMA_CH(chanid+1));
	dma->chan_reg[chanid] = chan_reg;
	config->channel = chanid+1;

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

	 //period =  config->data_size/64;
	period =  config->data_size/128;
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
				lli[i].dar = config->dst_addr + 0x200 * i;
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

Bit32u dma_lli_start_1(struct dw_dma* dma,Bit8u chanid)
{
	/*First Block */
	dma_chx_llp_write(dma->chan_reg[chanid], (Bit32u)&lli[0]);

	/* enable channel*/
	dma_ch_enable(dma->common_reg, dma->config[chanid].channel);

	/* Software Handshake request */
	//dma_blk_tfr_resume_req(dma->chan_reg[chanid);

	return 0;
}

Bit32u dma_lli_stop_1(struct dw_dma* dma,Bit8u chanid)
{
	dma_ch_disable(dma->common_reg, dma->config[chanid].channel);
	return 0;
}

int test_dma_lli_allchan(Bit8u testnum)
{
	printf("dma lli test begins.\n");
	struct dma_chx_config_t* config;
	Bit32u val;
	Bit32u* srcaddr = (Bit32u*)0x200000;
	Bit32u* dstaddr = (Bit32u*)0x210000;
	Bit8u i,temp;
	Bit32u j;
	Bit32u* psrcaddr;
	Bit32u* pdstaddr;
	Bit32u transize=0x600;

	val = dma_lli_init();
	if(val < 0){
		printf("dma init error.\n");
		return -1;
	}

    psrcaddr = srcaddr;
    pdstaddr = dstaddr;
    for(j=0; j<transize; j++)
    {
		*psrcaddr++ = j;
		*pdstaddr++ = 0;
    }

	//for(i=1;i<=8;i++)
    for(i=1;i<(testnum+1);i++)
	{
		/*channel is free?*/
		temp=dma_channel_free_query(&dma,i);
		if(!temp)
		{
			channelid=i-1;
			config = &dma.config[channelid];
			config->dir = DMA_MEM_TO_MEM;
			config->data_size = transize;           /* total size to be transfer */
			config->sms = DMX_AXI_MASTER_1;
			config->dms = DMX_AXI_MASTER_2;
			config->src_addr = (Bit32u)(srcaddr)+0x600000;
		    config->dst_addr = (Bit32u)(dstaddr)+0x600000;;
		    config->src_addr_mode = DMA_ADDR_INC;
		    config->dst_addr_mode = DMA_ADDR_INC;
		    config->trf_type = DMA_CFG_MEM2MEM_WITH_DMA_CTRL;
		    config->src_trf_mode = DMA_MULTI_BLOCK_LINKEDLIST;
		    config->dst_trf_mode = DMA_MULTI_BLOCK_LINKEDLIST;
		    dma_prepare_lli_1(&dma,channelid);
		    dma_intc_init_1(&dma,channelid);
		    dma_lli_start_1(&dma,channelid);
		    delay(1);
		    dma_lli_stop_1(&dma,channelid);
		    printf(" channel%d dma lli test sucess.\n",i);
		    CK_INTC_FreeIrq(CK_INTC_DMAC);
		}
		psrcaddr = srcaddr;
		pdstaddr = dstaddr;
		printf("psrcaddr = %x, pdstaddr = %x, data_size = %x \n", psrcaddr, pdstaddr, config->data_size);

		for(j=0; j<config->data_size; j++){
			if (*psrcaddr != *pdstaddr)
				printf("DMA transfer %x data error!\n", psrcaddr);
			*pdstaddr = 0;
			psrcaddr++;
			pdstaddr++;
		}
	}
	printf("dma lli test for all channels sucess.\n");
	return 0;
}

void CK_DMAC_Handler(CK_UINT32 irqid)
{
	printf("dmac have interrupt, channelid = %d\n", channelid+1);
    Bit32u temp,temp1;
    temp = dma_int_status_read(dma.common_reg)&0x1FFFF;
    printf("dmac int status:%x\n", temp);
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
		if(temp&(1<<channelid))
		{
			printf(" dmac CH%d interrupt\n",channelid+1);
			temp1=dma_chx_spec_int_status_read(dma.chan_reg[channelid]);
			printf(" dmac CHx interrupt status:%x\n",temp1);
			switch (temp1)
			{
				case DMA_CHX_BLOCK_TFR_DONE_INT:
					 printf("  BLOCK_TFR_DONE_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_BLOCK_TFR_DONE_INT);
					 break; /* clear DMA_CHX_BLOCK_TFR_DONE_INT interrupt */
				case DMA_CHX_DMA_TFR_DONE_INT:
					 printf("  DMA_TFR_DONE_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_DMA_TFR_DONE_INT);
					 break; /* clear DMA_CHX_DMA_TFR_DONE_INT interrupt */
				case DMA_CHX_SRC_TRC_INT:
					 printf("  DMA_CHX_SRC_TRC_INT\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_SRC_TRC_INT);
					 break; /* clear DMA_CHX_SRC_TRC_INT interrupt */
				case DMA_CHX_DST_TRC_INT:
					 printf("  DST_TransComp_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_DST_TRC_INT);
					 break; /* clear DMA_CHX_DST_TRC_INT interrupt */
				case DMA_CHX_SRC_DEC_ERR_INT:
					 printf("  SRC_DEC_ERR_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_SRC_DEC_ERR_INT);
					 break; /* clear DMA_CHX_SRC_DEC_ERR_INT interrupt */
				case DMA_CHX_DST_DEC_ERR_INT:
					 printf("  DST_DEC_ERR_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_DST_DEC_ERR_INT);
					 break; /* clear DMA_CHX_DST_DEC_ERR_INT interrupt */
				case DMA_CHX_SRC_SLV_ERR_INT:
					 printf("  SRC_SLV_ERR_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_SRC_SLV_ERR_INT);
					 break; /* clear DMA_CHX_SRC_SLV_ERR_INT interrupt */
				case DMA_CHX_DST_SLV_ERR_INT:
					 printf("  DST_SLV_ERR_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_DST_SLV_ERR_INT);
					 break; /* clear DMA_CHX_DST_SLV_ERR_INT interrupt */
				case DMA_CHX_RD_DEC_ERR_INT:
					 printf("  LLI_RD_DEC_ERR_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_RD_DEC_ERR_INT);
					 break; /* clear DMA_CHX_RD_DEC_ERR_INT interrupt */
				case DMA_CHX_WR_DEC_ERR_INT:
					 printf("  LLI_WR_DEC_ERR_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_WR_DEC_ERR_INT);
					 break; /* clear DMA_CHX_WR_DEC_ERR_INT interrupt */
				case DMA_CHX_RD_SLV_ERR_INT:
					 printf("  LLI_RD_SLV_ERR_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_RD_SLV_ERR_INT);
					 break; /* clear DMA_CHX_RD_SLV_ERR_INT interrupt */
				case DMA_CHX_WR_SLV_ERR_INT:
					 printf("  LLI_WR_SLV_ERR_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_WR_SLV_ERR_INT);
					 break; /* clear DMA_CHX_WR_SLV_ERR_INT interrupt */
				case DMA_CHX_SR_OR_LLI_INV_ERR_INT:
					 printf("  ShadowReg_Or_LLI_Invalid_ERR_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_SR_OR_LLI_INV_ERR_INT);
					 break; /* clear DMA_CHX_SR_OR_LLI_INV_ERR_INT interrupt */
				case DMA_CHX_SLVIF_MBT_ERR_INT:
					 printf("  SLVIF_MultiBlkType_ERR_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_SLVIF_MBT_ERR_INT);
					 break; /* clear DMA_CHX_SLVIF_MBT_ERR_INT interrupt */
				case DMA_CHX_SLVIF_DEC_ERR_INT:
					 printf("  SLVIF_DEC_ERR_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_SLVIF_DEC_ERR_INT);
					 break; /* clear DMA_CHX_SLVIF_DEC_ERR_INT interrupt */
				case DMA_CHX_SLVIF_WR2RO_ERR_INT:
					 printf("  SLVIF_WR2RO_ERR_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_SLVIF_WR2RO_ERR_INT);
					 break; /* clear DMA_CHX_SLVIF_WR2RO_ERR_INT interrupt */
				case DMA_CHX_SLVIF_RD2RWO_ERR_INT:
					 printf("  SLVIF_RD2WO_ERR_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_SLVIF_RD2RWO_ERR_INT);
					 break; /* clear DMA_CHX_SLVIF_RD2RWO_ERR_INT interrupt */
				case DMA_CHX_SLVIF_WRONCHEN_ERR_INT:
					 printf("  SLVIF_WrOnChEn_ERR_ntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_SLVIF_WRONCHEN_ERR_INT);
					 break; /* clear DMA_CHX_SLVIF_WRONCHEN_ERR_INT interrupt */
				case DMA_CHX_SLVIF_SR_WRON_VALID_ERR_INT:
					 printf("  SLVIF_ShadowReg_WrOnValid_ERR_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_SLVIF_SR_WRON_VALID_ERR_INT);
					 break; /* clear DMA_CHX_SLVIF_SR_WRON_VALID_ERR_INT interrupt */
				case DMA_CHX_SLVIF_WRONHODL_ERR_INT:
					 printf("  SLVIF_WrOnHold_ERR_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_SLVIF_WRONHODL_ERR_INT);
					 break; /* clear DMA_CHX_SLVIF_WRONHODL_ERR_INT interrupt */
				case DMA_CHX_CHLOCK_CLEARED_INT:
					 printf("  ChLock_Cleared_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_CHLOCK_CLEARED_INT);
					 break; /* clear DMA_CHX_CHLOCK_CLEARED_INT interrupt */
				case DMA_CHX_CH_SRC_SUSPENDED_INT:
					 printf("  CH_SRC_SUSPENDED_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_CH_SRC_SUSPENDED_INT);
					 break; /* clear DMA_CHX_CH_SRC_SUSPENDED_INT interrupt */
				case DMA_CHX_CH_SUSPENDED_INT:
					 printf("  CH_SUSPENDED_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_CH_SUSPENDED_INT);
					 break; /* clear DMA_CHX_CH_SUSPENDED_INT interrupt */
				case DMA_CHX_CH_DISABLED_INT:
					 printf("  CH_DISABLED_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_CH_DISABLED_INT);
					 break; /* clear DMA_CHX_CH_DISABLED_INT interrupt */
				case DMA_CHX_CH_ABORTERD_INT:
					 printf("  CH_ABORTED_IntStat\n");
					 dma_chx_int_status_clear(dma.chan_reg[channelid],DMA_CHX_CH_ABORTERD_INT);
					 break; /* clear DMA_CHX_CH_ABORTERD_INT interrupt */
				default:break;
			}
    	}
		else
			return;
    }

}




