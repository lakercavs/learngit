/*
 * dma-single-allchan.c
 *
 *  Created on: 2016-12-1
 *      Author: i
 */
#include "datatype.h"
#include "dma.h"
#include "misc.h"
#include "intc.h"

extern Bit8u channelid;

Bit32u dma_singal_init(){
	Bit32u val;
	struct dw_dma_common_reg* common_reg;

	common_reg = (struct dw_dma_common_reg*)APB_DMA_BASEADDR;
	dma.common_reg = common_reg;

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

	/* Disable DMA IRQ */

	/* Disable DMA IRQ */
	dma_gloabe_int_disable(common_reg);


	/* Enable DMA */
	if(!dma_dmac_is_enable(common_reg))
		dma_dmac_enable(common_reg);

	return 0;
}

/* judge the channel is free or not,
 * free,prepare
 * busy,do nothing
 * */
Bit8u dma_channel_free_query(struct dw_dma* dma,Bit8u chanid)
{
	Bit32u val;
	val = dma_ch_is_free(dma->common_reg, chanid);
	printf("channel:%d val:%d\n", chanid, val);
	if(val == 0)
		printf("channel%d is free\n", chanid);
	else
		printf("channel%d is not free\n", chanid);
	return val;
}

Bit32u dma_intc_init_1(struct dw_dma* dma,Bit8u chanid)
{
	/*enable CommonReg irq*/
	dma_com_int_signal_enable(dma->common_reg, DMA_COM_INT_ALL);
	dma_com_int_status_enable(dma->common_reg, DMA_COM_INT_ALL);
	/*enable channel irq*/
	dma_chx_int_signal_eanble(dma->chan_reg[chanid],DMA_CHX_CH_ALL_INT);
	dma_chx_int_status_eanble(dma->chan_reg[chanid],DMA_CHX_CH_ALL_INT);

	/* enable dmac interrupt */
	if(!dma_dmac_gloabe_int_is_enable(dma->common_reg))
		dma_gloabe_int_enable(dma->common_reg);

	/* register dma isr */
    CK_INTC_RequestIrq(CK_INTC_DMAC, CK_DMAC_Handler);
	return 0;
}

Bit32u dma_singal_prepare_lli_1(struct dw_dma* dma,Bit8u chanid)
{
	Bit32u period;
	Bit32u i;
	Bit32u cfg_lo=0, cfg_hi=0, ctl_lo=0, ctl_hi=0;
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

	dma_chx_cfg_lo_direct_write(chan_reg, cfg_lo);
	dma_chx_cfg_hi_direct_write(chan_reg, cfg_hi);

	/* set chx CTL Low */
	ctl_lo |= DMA_CHX_CTL_SMS(config->sms);
	ctl_lo |= DMA_CHX_CTL_DMS(config->dms);
	ctl_lo |= DMA_CHX_CTL_SING(config->src_addr_mode);
	ctl_lo |= DMA_CHX_CTL_DING(config->dst_addr_mode);

	/* Already hardcoded 128bit, ignored */
	ctl_lo |= DMA_CHX_CTL_SRC_TR_WIDTH(DMA_CTL_TR_WIDTH_128);	/* src transfer width */
	ctl_lo |= DMA_CHX_CTL_DST_TR_WIDTH(DMA_CTL_TR_WIDTH_128);	/* dst transfer width */

	/* reference chapter 3.9*/
	/* Burst transaction length = TR_WIDTH * MSIZE
	 * the total size is 0x200 2048bit
	 * so 2048 = 128 * 16
	 * SRC_TR_WIDTH = 128
	 * MSIZE = 16
	 *  */
	ctl_lo |= DMA_CHX_CTL_SRC_MSIZE(DMA_CTL_BURST_LENGTH_16);	/* src burst transaction length */
	ctl_lo |= DMA_CHX_CTL_DST_MSIZE(DMA_CTL_BURST_LENGTH_16);	/* dst burst transaction length */

	/* set chx CTL High */
	ctl_hi |= DMA_CHX_CTL_ARLEN_EN;								/* Src Burst Lenght Enable */
	ctl_hi |= DMA_CHX_CTL_AWLEN_EN;								/* Dst Burst Lenght Enable */

	/* Burst Length 1 for Single transaction
	 * Max Burst Length is 8
	 * */
	ctl_hi |= DMA_CHX_CTL_ARLEN(DMA_CTL_BURST_LENGTH_1);		/* Src Burst Lenght Size */
	ctl_hi |= DMA_CHX_CTL_AWLEN(DMA_CTL_BURST_LENGTH_1);		/* Dst Burst Lenght Size */
	ctl_hi |= DMA_CHX_CTL_SRC_STAT_EN;
	ctl_hi |= DMA_CHX_CTL_DST_STAT_EN;

    period = 1;
    config->lli = lli;

    for(i=0; i<period; i++){
        lli[i].block_ts = config->data_size;

        switch (config->dir) {
            case DMA_MEM_TO_MEM:
                            lli[i].sar = config->src_addr;
                            lli[i].dar = config->dst_addr;
                            lli[i].ctl_lo = ctl_lo;
                            lli[i].ctl_hi = ctl_hi;
                            break;
            default:
                break;
        }
    }
	return 0;
}

Bit32u dma_singal_start_1(struct dw_dma* dma,Bit8u chanid)
{
	//Bit32u temp;
	/*First Block */
	dma_chx_sar_write(dma->chan_reg[chanid], dma->config[chanid].lli->sar);
	dma_chx_dar_write(dma->chan_reg[chanid], dma->config[chanid].lli->dar);
	dma_chx_ctl_lo_direct_write(dma->chan_reg[chanid], dma->config[chanid].lli->ctl_lo);
	dma_chx_ctl_hi_direct_write(dma->chan_reg[chanid], dma->config[chanid].lli->ctl_hi);

	//dma_chx_block_ts_write(dma->chan_reg, dma->config.lli->block_ts/128);
	dma_chx_block_ts_write(dma->chan_reg[chanid], 0x40);

	/* enable channel*/
	dma_ch_enable(dma->common_reg, dma->config[chanid].channel);
	//temp=dma->common_reg->ChEn_LO;
	//printf("channel enable status:%x\n",temp);

	return 0;
}

Bit32u dma_singal_stop_1(struct dw_dma* dma,Bit8u chanid){
	dma_ch_disable(dma->common_reg, dma->config[chanid].channel);
	return 0;
}

int test_dma_singal_allchan(Bit8u testnum)
{
	struct dma_chx_config_t* config;
	Bit32u val;
	Bit32u* srcaddr = (Bit32u*)0x200000;
	Bit32u* dstaddr = (Bit32u*)0x210000;
	Bit8u i,temp;
	Bit32u j;
	Bit32u* psrcaddr, *pdstaddr;
	Bit32u transfernum=0x200;

	printf("dma signal test begins.\n");
	val = dma_singal_init();
	if(val < 0)
	{
		printf("dma init error.\n");
		return -1;
	}

	//for(i=1;i<=8;i++)
	for(i=1;i<(testnum+1);i++)
	{
	    psrcaddr = srcaddr;
	    pdstaddr = dstaddr;
	    for(j=0; j<transfernum; j+=4)
	    {
			*psrcaddr++ = j;
			*pdstaddr++ = 0;
		}

		/*channel1~8 is free?*/
		temp=dma_channel_free_query(&dma,i);
		if(!temp)
		{
			channelid=i-1;
			config = &dma.config[channelid];
			config->dir = DMA_MEM_TO_MEM;
			config->data_size = transfernum;			/* total size to be transfer */
			config->sms = DMX_AXI_MASTER_1;
			config->dms = DMX_AXI_MASTER_2;
			config->src_addr = (Bit32u)(srcaddr)+0x600000;
			config->dst_addr = (Bit32u)(dstaddr)+0x600000;
			config->src_addr_mode = DMA_ADDR_INC;
			config->dst_addr_mode = DMA_ADDR_INC;
			config->trf_type = DMA_CFG_MEM2MEM_WITH_DMA_CTRL;
			config->src_trf_mode = DMA_SINGLE_BLOCK;
			config->dst_trf_mode = DMA_SINGLE_BLOCK;

			dma_singal_prepare_lli_1(&dma,channelid);
			dma_intc_init_1(&dma,channelid);
			dma_singal_start_1(&dma,channelid);
			while(!dma_chx_compled_blk_transfer_size_read(dma.chan_reg[channelid]));
			while(dma_chx_compled_blk_transfer_size_read(dma.chan_reg[channelid]) < dma_chx_block_ts_read(dma.chan_reg[channelid]))
				printf("transfer size:%x completed size:%x\n", dma_chx_block_ts_read(dma.chan_reg[channelid]),
						dma_chx_compled_blk_transfer_size_read(dma.chan_reg[channelid]));
			printf("Channel%d finished DMA transfer.\n",channelid+1);
			dma_singal_stop_1(&dma,channelid);
			CK_INTC_FreeIrq(CK_INTC_DMAC);
		}
		psrcaddr = srcaddr;
		pdstaddr = dstaddr;
		printf("psrcaddr = %x, pdstaddr = %x, data_size = %x \n", psrcaddr, pdstaddr, config->data_size);
		//printf("psrcaddr = %x, pdstaddr = %x, data_size = %x \n", psrcaddr, pdstaddr, &dma);
		for(j=0; j<config->data_size; j+=4)
		{
			if (*psrcaddr != *pdstaddr)
				printf("DMA transfer %x data error!\n", psrcaddr);
			psrcaddr++;
			pdstaddr++;
		}
	}
	printf("dma signal for all channels test sucess.\n");
	return 0;
}




