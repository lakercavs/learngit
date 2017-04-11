#include "dma.h"
#include "misc.h"

#if 0

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

Bit32u dma_singal_prepare_lli(struct dw_dma* dma){
	struct dw_dma_chan_reg * chan_reg;
	Bit32u cfg_lo=0, cfg_hi=0, ctl_lo=0, ctl_hi=0;
	struct dma_chx_config_t* config = &dma->config;
	Bit32u period;
	Bit32u i, val;
	Bit32u channel;

	/* Get free channel */
	/* channel 1 is used for spi transfer, ignore it */
	for(channel=2; channel<9; channel++){
		val = dma_ch_is_free(dma->common_reg, channel);
		printf("channel:%d val:%d\n", channel, val);
		if(val == 0){
			printf("Find free channel:%d\n", channel);
			chan_reg = (struct dw_dma_chan_reg*)(APB_DMA_CH(channel));
			dma->chan_reg = chan_reg;
			config->channel = channel;
			break;
		}
	}
	if(channel == 9){
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

	dma_chx_cfg_lo_direct_write(dma->chan_reg, cfg_lo);
	dma_chx_cfg_hi_direct_write(dma->chan_reg, cfg_hi);

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

Bit32u dma_singal_start(struct dw_dma* dma){

	/*First Block */
	dma_chx_sar_write(dma->chan_reg, dma->config.lli->sar);
	dma_chx_dar_write(dma->chan_reg, dma->config.lli->dar);
	dma_chx_ctl_lo_direct_write(dma->chan_reg, dma->config.lli->ctl_lo);
	dma_chx_ctl_hi_direct_write(dma->chan_reg, dma->config.lli->ctl_hi);

	//dma_chx_block_ts_write(dma->chan_reg, dma->config.lli->block_ts/128);
	dma_chx_block_ts_write(dma->chan_reg, 0x40);

	/* enable channel*/
	dma_ch_enable(dma->common_reg, dma->config.channel);
	
	return 0;
}

Bit32u dma_singal_stop(struct dw_dma* dma){
	dma_ch_disable(dma->common_reg, dma->config.channel);
	return 0;
}

int test_dma_singal()
{
	struct dma_chx_config_t* config;
	Bit32u val;
	Bit32u* srcaddr = (Bit32u*)0x300000;
	Bit32u* dstaddr = (Bit32u*)0x340000;
	Bit32u i;
	Bit32u* psrcaddr, *pdstaddr;

	printf("dma signal test begins.\n");
	config = &dma.config;
	val = dma_singal_init();
	if(val < 0){
		printf("dma init error.\n");
		return -1;
	}

    config->dir = DMA_MEM_TO_MEM; 
    config->data_size = 0x400;			/* total size to be transfer */
	config->sms = DMX_AXI_MASTER_1;
	config->dms = DMX_AXI_MASTER_2;
    config->src_addr = (Bit32u)srcaddr;
    config->dst_addr = (Bit32u)dstaddr;
    config->src_addr_mode = DMA_ADDR_INC;
    config->dst_addr_mode = DMA_ADDR_INC;
    config->trf_type = DMA_CFG_MEM2MEM_WITH_DMA_CTRL;      
    config->src_trf_mode = DMA_SINGLE_BLOCK;
    config->dst_trf_mode = DMA_SINGLE_BLOCK;

    psrcaddr = srcaddr;
    pdstaddr = dstaddr;
    for(i=0; i<config->data_size; i++){
		*psrcaddr++ = i;
		*pdstaddr++ = 0;
    }

    dma_intc_init();
	dma_singal_prepare_lli(&dma);
	dma_singal_start(&dma);
	while(!dma_chx_compled_blk_transfer_size_read(dma.chan_reg));
	while(dma_chx_compled_blk_transfer_size_read(dma.chan_reg) < dma_chx_block_ts_read(dma.chan_reg))
		printf("transfer size:%x completed size:%x\n", dma_chx_block_ts_read(dma.chan_reg),
				dma_chx_compled_blk_transfer_size_read(dma.chan_reg));
	printf("Finished DMA transfer.\n");
	dma_singal_stop(&dma);
	CK_INTC_FreeIrq(CK_INTC_DMAC);
	printf("dma signal test sucess.\n");
	return 0;
}
#endif
