#include <stdio.h>
#include <ctype.h>
#include "include/io.h"
//#include "include/list.h"
#include "i2c.h"

static Bit32u IC_DATA_CMD_Data=0;     //no restart bit[10],no stop bit[9],write bit[8]
static IC_Mode_Typedef Mode = MASTER;
static Bit32u I2C_DYNAMIC_TAR_UPDATE = 1;
static Bit32u SPKLEN_FS = 13;
static Bit32u SPKLEN_HS = 3;
static Bit32u I2C_CLOCK = 25000;  //Khz

/*Function1-1:set IC mode ,set bit0,bit6 of IC_CON reg*/
Bit32u I2C_Set_Mode(IC_Mode_Typedef ICmode)
{
	//Mode=ICmode;
	if(ICmode==MASTER)
	{
		//I2C->IC_CON |= IC_CON_SLAVE_DISABLE;
		//I2C->IC_CON |= IC_CON_MASTER_MODE;
		reg_writel(IC_CON_SLAVE_DISABLE,I2C,IC_CON);
		reg_writel(IC_CON_MASTER_MODE,I2C,IC_CON);
		return 0;
	}
	else if(ICmode==SLAVE)
	{
		//I2C->IC_CON &= ~IC_CON_SLAVE_DISABLE;
		//I2C->IC_CON &= ~IC_CON_MASTER_MODE;
		reg_clearl(IC_CON_SLAVE_DISABLE,I2C,IC_CON);
		reg_clearl(IC_CON_MASTER_MODE,I2C,IC_CON);
		return 0;
	}
	else
		return 1;
}

/*Function1-2:set IC speed ,set bit2,bit1 of IC_CON reg*/
Bit32u I2C_Set_SpeedMode(IC_SpeedMode_Typedef SpeedMode)
{
	Bit32u temp;
	//temp=I2C->IC_CON;      // get the origin value of IC_CON reg
	temp=reg_readl(I2C,IC_CON);  // get the origin value of IC_CON reg
	temp &= ~0x06;               // clear bit[2:1]
	temp |= SpeedMode;           // get the speed value
	//I2C->IC_CON = temp;
	reg_direct_writel(temp,I2C,IC_CON);
	return 0;
}

/*Function1-3:set IC addrmode
 *  if I2C_DYNAMIC_TAR_UPDATE =0,set bit3(slave ) or bit4(master) of IC_CON reg
 *  if I2C_DYNAMIC_TAR_UPDATE =1,set bit12(master) of IC_TAR reg,bit4(master) of IC_CON reg is read only
 * */
Bit32u I2C_Set_AddrMode(IC_AddrMode_Typedef AddrMode)
{
	if(Mode==MASTER)
	{
		if(I2C_DYNAMIC_TAR_UPDATE==0)
		{
			if(AddrMode==address_bit_10)
			{
				//I2C->IC_CON |= IC_CON_10BITADDR_MASTER;
				reg_writel(IC_CON_10BITADDR_MASTER,I2C,IC_CON);
				return 0;
			}
			else if (AddrMode==address_bit_7)
			{
				//I2C->IC_CON &= ~IC_CON_10BITADDR_MASTER;
				reg_clearl(IC_CON_10BITADDR_MASTER,I2C,IC_CON);
				return 0;
			}
			else
				return 1;
		}
		else
		{
			if(AddrMode==address_bit_10)
			{
				//I2C->IC_TAR |= IC_TAR_10BITADDR_MASTER;
				reg_writel(IC_CON_10BITADDR_MASTER,I2C,IC_TAR);
				return 0;
			}
			else if (AddrMode==address_bit_7)
			{
				//I2C->IC_TAR &= ~IC_TAR_10BITADDR_MASTER;
				reg_clearl(IC_CON_10BITADDR_MASTER,I2C,IC_TAR);
				return 0;
			}
			else
				return 1;
		}

	}
	else if(Mode==SLAVE)
	{
		if(AddrMode==address_bit_10)
		{
			//I2C->IC_CON |= IC_CON_10BITADDR_SLAVE;
			reg_writel(IC_CON_10BITADDR_SLAVE,I2C,IC_CON);
			return 0;
		}
		else if (AddrMode==address_bit_7)
		{
			//I2C->IC_CON &= ~IC_CON_10BITADDR_SLAVE;
			reg_clearl(IC_CON_10BITADDR_SLAVE,I2C,IC_CON);
			return 0;
		}
		else
			return 1;
	}
	else
		return 1;
}

/*Function1-4:enable/disable IC restart ,set bit5 of IC_CON reg(only in master mode)*/
Bit32u I2C_Restart_EN(State_TypeDef NewState)
{
	if (NewState == ENABLE)
	{
		//I2C->IC_CON |= IC_CON_RESTART_EN;
		reg_writel(IC_CON_RESTART_EN,I2C,IC_CON);
		return 0;
	}
	else if(NewState == DISABLE)
	{
		//I2C->IC_CON &= ~IC_CON_RESTART_EN;
		reg_clearl(IC_CON_RESTART_EN,I2C,IC_CON);
		return 0;
	}
	else
		return 1;
}

/*Function1-5:Control TX empty IT  ,set bit8 of IC_CON reg(only in master mode)*/
Bit32u I2C_TXEMP_CTRL(BOOL NewState)
{
	if (NewState == TRUE)
	{
		//I2C->IC_CON |= IC_CON_TX_EMPTY_CTRL;
		reg_writel(IC_CON_TX_EMPTY_CTRL,I2C,IC_CON);
		return 0;
	}
	else
	{
		//I2C->IC_CON &= ~IC_CON_TX_EMPTY_CTRL;
		reg_clearl(IC_CON_TX_EMPTY_CTRL,I2C,IC_CON);
		return 0;
	}
}

/*Function2-1:set IC target address ,set bit[9:0] of IC_TAR reg*/
Bit32u I2C_Set_TAR_Addr(Bit32u Address)
{
	Bit32u temp;
	//temp=I2C->IC_TAR;        // get the origin value of IC_TAR reg
	temp=reg_readl(I2C,IC_TAR);   // get the origin value of IC_TAR reg
	temp &= ~0x03FF;              // clear bit[9:0]
	temp |= Address;              // get the address
	//I2C->IC_TAR = temp;
	reg_direct_writel(temp,I2C,IC_TAR);
	return 0;
}

/*Function3-1:set IC slave address ,set bit[9:0] of IC_SAR reg*/
Bit32u I2C_Set_SLA_Addr(Bit32u Address)
{
	Bit32u temp;
	//temp=I2C->IC_SAR;        // get the origin value of IC_SAR reg
	temp=reg_readl(I2C,IC_SAR);   // get the origin value of IC_SAR reg
	temp &= ~0x03FF;         // clear bit[9:0]
	temp |= Address;         // get the address
	//I2C->IC_SAR = temp;
	reg_direct_writel(temp,I2C,IC_SAR);
	return 0;
}

/*Function5-1:send IC data ,set bit[8] to write ,set bit[7:0] of IC_DATA_CMD*/
Bit32u I2C_Send_Data(Bit8u Data)
{
	Bit32u temp=IC_DATA_CMD_Data;
	temp &= ~0x1FF;                           // clear bit[8:0],set bit[8] to write
	temp |= Data;
	//I2C->IC_DATA_CMD = temp;
	reg_direct_writel(temp,I2C,IC_DATA_CMD);
	return 0;
}

/*Function5-6:set bit[8] to Read,read bit[10:8] of IC_DATA_CMD*/
void I2C_Receive_Set(void)
{
	Bit32u temp=IC_DATA_CMD_Data;
	temp |=0x100;                              //set bit[8] to Read
	//I2C->IC_DATA_CMD = temp1;
	reg_direct_writel(temp,I2C,IC_DATA_CMD);
}
/*Function5-2:receive IC data read/write ,read bit[7:0] of IC_DATA_CMD*/
Bit8u I2C_Receive_Data(void)
{
	Bit32u temp=0;
	//temp = I2C->IC_DATA_CMD;
	temp =reg_readl(I2C,IC_DATA_CMD);
	temp &= 0xFF;
	return temp;
}

/*Function5-3:set restart transfer or not ,set bit[10] of static IC_DATA_CMD_Data*/
Bit32u I2C_Set_Restart(State_TypeDef NewState)
{
	if(NewState==ENABLE)
	{
		IC_DATA_CMD_Data |= IC_DB_RESTART;
		return 0;
	}
	else if(NewState==DISABLE)
	{
		IC_DATA_CMD_Data &= ~IC_DB_RESTART;
		return 0;
	}
	else
	return 1;
}

/*Function5-4:stop transfer or not ,set bit[9] of static IC_DATA_CMD_Data*/
Bit32u I2C_Set_Stop(State_TypeDef NewState)
{
	if(NewState==ENABLE)
	{
		IC_DATA_CMD_Data |= IC_DB_STOP;
		return 0;
	}
	else if(NewState==DISABLE)
	{
		IC_DATA_CMD_Data &= ~IC_DB_STOP;
		return 0;
	}
	else
	return 1;
}

/*Function6&7-1:set IC Standard Speed I2C Clock SCL High and Low Counts ,set bit[15:0] of IC_SS_SCL_HCNT & IC_SS_SCL_LCNT reg
 * unit:KHz
 * Duty ratio:50%
 * */
Bit32u I2C_Set_SS_SCL_FREQ(Bit8u Frequeny)   //frequency<=100
{
	Bit32u HighCNT,LowCNT,temp=0;
	temp=((I2C_CLOCK/Frequeny)-SPKLEN_FS-8)%2;
	if(Frequeny>100)
		return 1;
	else                                     //SS_FREQ<=100KHz
	{
		if(temp)
		{
			HighCNT=LowCNT=((I2C_CLOCK/Frequeny)-SPKLEN_FS-8)/2+1;
		}
		else
			HighCNT=LowCNT=((I2C_CLOCK/Frequeny)-SPKLEN_FS-8)/2;
	reg_direct_writel(HighCNT, I2C, IC_SS_SCL_HCNT);
	reg_direct_writel(LowCNT, I2C, IC_SS_SCL_LCNT);
	return 0;
	}
}

/*Function8&9-1:set Fast Mode or Fast Mode Plus I2C Clock SCL High and Low Counts ,set bit[15:0] of IC_FS_SCL_HCNT & IC_FS_SCL_LCNT reg*/
Bit32u I2C_Set_FS_SCL_FREQ(Bit32u Frequeny)
{
	Bit32u HighCNT,LowCNT,temp=0;
	temp=((I2C_CLOCK/Frequeny)-SPKLEN_FS-8)%2;
	if(Frequeny>1000)
		return 1;
	else                                     //FS_FREQ<=1000KHz
	{
		if(temp)
		{
			HighCNT=LowCNT=((I2C_CLOCK/Frequeny)-SPKLEN_FS-8)/2+1;
		}
		else
			HighCNT=LowCNT=((I2C_CLOCK/Frequeny)-SPKLEN_FS-8)/2;
	reg_direct_writel(HighCNT, I2C, IC_FS_SCL_HCNT);
	reg_direct_writel(LowCNT, I2C, IC_FS_SCL_LCNT);
	return 0;
	}
}

/*Function10&11-1:set High Speed I2C Clock SCL High and Low Counts ,set bit[15:0] of IC_HS_SCL_HCNT & IC_HS_SCL_LCNT reg*/
Bit32u I2C_Set_HS_SCL_FREQ(Bit32u Frequeny)
{
	Bit32u HighCNT,LowCNT,temp=0;
	temp=((I2C_CLOCK/Frequeny)-SPKLEN_HS-8)%2;
	if(Frequeny>3400)
		return 1;
	else                                   //SS_FREQ<=3.4M
	{
		if(temp)
		{
			HighCNT=LowCNT=((I2C_CLOCK/Frequeny)-SPKLEN_HS-8)/2+1;
		}
		else
			HighCNT=LowCNT=((I2C_CLOCK/Frequeny)-SPKLEN_HS-8)/2;
	reg_direct_writel(HighCNT, I2C, IC_HS_SCL_HCNT);
	reg_direct_writel(LowCNT, I2C, IC_HS_SCL_LCNT);
	return 0;
	}
}

/*Function12-1:Query interrupt status,read bit[14:0] of IC_INRT_STAT reg*/
ITStatus_TypeDef I2C_Get_ITStat(IC_INTR_Typedef INRT_Item)
{
	ITStatus_TypeDef bitstatus=INACTIVE;
	//if (!(I2C->IC_INTR_STAT & INRT_Item))
	if (!(reg_readl(I2C,IC_INTR_STAT) & INRT_Item))
	{
		bitstatus = INACTIVE;             /* Interrupt is inactive */
	}
	else
	{
		bitstatus = ACTIVE;               /* Interrupt is active */
	}
	return bitstatus;
}

/*Function13-1:Mask raw interrupt status,clear bit[15:0] of IC_RAW_INRT_MASK reg */
Bit32u I2C_Mask_IT(IC_INTR_Typedef INTR_Item)
{
	switch(INTR_Item)
	{
#if 0
		case IC_INT_ALL:            I2C->IC_CLR_INTR &= ~IC_INT_ALL;           return 0;break; /* Mask all interrupt */
		case IC_INT_NONE:           I2C->IC_CLR_INTR |= ~IC_INT_NONE;          return 0;break; /* Mask all interrupt */
		case IC_INT_RX_UNDE:        I2C->IC_CLR_INTR &= ~IC_INT_RX_UNDE;       return 0;break; /* Mask IC_INT_RX_UNDE */
		case IC_INT_RX_OVER:        I2C->IC_CLR_INTR &= ~IC_INT_RX_OVER;       return 0;break; /* Mask IC_INT_RX_OVER */
		case IC_INT_RX_FULL:        I2C->IC_CLR_INTR &= ~IC_INT_RX_FULL;       return 0;break; /* Mask IC_INT_RX_FULL */
		case IC_INT_TX_OVER:        I2C->IC_CLR_INTR &= ~IC_INT_TX_OVER;       return 0;break; /* Mask IC_INT_TX_OVER */
		case IC_INT_TX_EMPTY:       I2C->IC_CLR_INTR &= ~IC_INT_TX_EMPTY;      return 0;break; /* Mask IC_INT_TX_EMPTY */
		case IC_INT_RD_REQ:         I2C->IC_CLR_INTR &= ~IC_INT_RD_REQ;        return 0;break; /* Mask IC_INT_RD_REQ */
		case IC_INT_TX_ABRT:        I2C->IC_CLR_INTR &= ~IC_INT_TX_ABRT;       return 0;break; /* Mask IC_INT_TX_ABRT */
		case IC_INT_RX_DONE:        I2C->IC_CLR_INTR &= ~IC_INT_RX_DONE;       return 0;break; /* Mask IC_INT_RX_DONE */
		case IC_INT_ACTIVITY:       I2C->IC_CLR_INTR &= ~IC_INT_ACTIVITY;      return 0;break; /* Mask IC_INT_ACTIVITY */
		case IC_INT_STOP_DET:       I2C->IC_CLR_INTR &= ~IC_INT_STOP_DET;      return 0;break; /* Mask IC_INT_STOP_DET */
		case IC_INT_START_DET:      I2C->IC_CLR_INTR &= ~IC_INT_START_DET;     return 0;break; /* Mask IC_INT_START_DET */
		case IC_INT_GEN_CALL:       I2C->IC_CLR_INTR &= ~IC_INT_GEN_CALL;      return 0;break; /* Mask IC_INT_GEN_CALL */
		case IC_INT_RESTART_DET:    I2C->IC_CLR_INTR &= ~IC_INT_RESTART_DET;   return 0;break; /* Mask IC_INT_RESTART_DET */
		case IC_INT_MST_ON_HOLD:    I2C->IC_CLR_INTR &= ~IC_INT_MST_ON_HOLD;   return 0;break; /* Mask IC_INT_MST_ON_HOLD */
		case IC_INT_SCL_STUCK_AT_LOW:    I2C->IC_CLR_INTR |= IC_INT_SCL_STUCK_AT_LOW;       return 0;break; /* Mask IC_INT_SCL_STUCK_AT_LOW */
#endif
		case IC_INT_ALL:            reg_clearl(IC_INT_ALL,I2C,IC_CLR_INTR);             return 0;break; /* Mask all interrupt */
		case IC_INT_NONE:           reg_direct_writel(IC_INT_ALL, I2C, IC_CLR_INTR);   return 0;break; /* Mask none interrupt */
		case IC_INT_RX_UNDE:        reg_clearl(IC_INT_RX_UNDE, I2C, IC_CLR_INTR);       return 0;break; /* Mask IC_INT_RX_UNDE */
		case IC_INT_RX_OVER:        reg_clearl(IC_INT_RX_OVER, I2C, IC_CLR_INTR);       return 0;break; /* Mask IC_INT_RX_OVER */
		case IC_INT_RX_FULL:        reg_clearl(IC_INT_RX_FULL, I2C, IC_CLR_INTR);       return 0;break; /* Mask IC_INT_RX_FULL */
		case IC_INT_TX_OVER:        reg_clearl(IC_INT_TX_OVER, I2C, IC_CLR_INTR);       return 0;break; /* Mask IC_INT_TX_OVER */
		case IC_INT_TX_EMPTY:       reg_clearl(IC_INT_TX_EMPTY, I2C, IC_CLR_INTR);      return 0;break; /* Mask IC_INT_TX_EMPTY */
		case IC_INT_RD_REQ:         reg_clearl(IC_INT_RD_REQ, I2C, IC_CLR_INTR);        return 0;break; /* Mask IC_INT_RD_REQ */
		case IC_INT_TX_ABRT:        reg_clearl(IC_INT_TX_ABRT, I2C, IC_CLR_INTR);       return 0;break; /* Mask IC_INT_TX_ABRT */
		case IC_INT_RX_DONE:        reg_clearl(IC_INT_RX_DONE, I2C, IC_CLR_INTR);       return 0;break; /* Mask IC_INT_RX_DONE */
		case IC_INT_ACTIVITY:       reg_clearl(IC_INT_ACTIVITY, I2C, IC_CLR_INTR);      return 0;break; /* Mask IC_INT_ACTIVITY */
		case IC_INT_STOP_DET:       reg_clearl(IC_INT_STOP_DET, I2C, IC_CLR_INTR);      return 0;break; /* Mask IC_INT_STOP_DET */
		case IC_INT_START_DET:      reg_clearl(IC_INT_START_DET, I2C, IC_CLR_INTR);     return 0;break; /* Mask IC_INT_START_DET */
		case IC_INT_GEN_CALL:       reg_clearl(IC_INT_GEN_CALL, I2C, IC_CLR_INTR);      return 0;break; /* Mask IC_INT_GEN_CALL */
		case IC_INT_RESTART_DET:    reg_clearl(IC_INT_RESTART_DET, I2C, IC_CLR_INTR);   return 0;break; /* Mask IC_INT_RESTART_DET */
		case IC_INT_MST_ON_HOLD:    reg_clearl(IC_INT_MST_ON_HOLD, I2C, IC_CLR_INTR);   return 0;break; /* Mask IC_INT_MST_ON_HOLD */
		case IC_INT_SCL_STUCK_AT_LOW:    reg_clearl(IC_INT_SCL_STUCK_AT_LOW, I2C, IC_CLR_INTR);       return 0;break; /* Mask IC_INT_SCL_STUCK_AT_LOW */
		default:return 1;break;
	}
}

/*Function13-2:Umask raw interrupt status,set bit[15:0] of IC_RAW_INRT_MASK reg */
Bit32u I2C_UMask_IT(IC_INTR_Typedef INTR_Item)
{
	switch(INTR_Item)
	{
#if 0
		case IC_INT_ALL:            I2C->IC_CLR_INTR |= IC_INT_ALL;           return 0;break; /* Mask all interrupt */
		case IC_INT_NONE:           I2C->IC_CLR_INTR &= IC_INT_NONE;          return 0;break; /* Mask all interrupt */
		case IC_INT_RX_UNDE:        I2C->IC_CLR_INTR |= IC_INT_RX_UNDE;       return 0;break; /* Mask IC_INT_RX_UNDE */
		case IC_INT_RX_OVER:        I2C->IC_CLR_INTR |= IC_INT_RX_OVER;       return 0;break; /* Mask IC_INT_RX_OVER */
		case IC_INT_RX_FULL:        I2C->IC_CLR_INTR |= IC_INT_RX_FULL;       return 0;break; /* Mask IC_INT_RX_FULL */
		case IC_INT_TX_OVER:        I2C->IC_CLR_INTR |= IC_INT_TX_OVER;       return 0;break; /* Mask IC_INT_TX_OVER */
		case IC_INT_TX_EMPTY:       I2C->IC_CLR_INTR |= IC_INT_TX_EMPTY;      return 0;break; /* Mask IC_INT_TX_EMPTY */
		case IC_INT_RD_REQ:         I2C->IC_CLR_INTR |= IC_INT_RD_REQ;        return 0;break; /* Mask IC_INT_RD_REQ */
		case IC_INT_TX_ABRT:        I2C->IC_CLR_INTR |= IC_INT_TX_ABRT;       return 0;break; /* Mask IC_INT_TX_ABRT */
		case IC_INT_RX_DONE:        I2C->IC_CLR_INTR |= IC_INT_RX_DONE;       return 0;break; /* Mask IC_INT_RX_DONE */
		case IC_INT_ACTIVITY:       I2C->IC_CLR_INTR |= IC_INT_ACTIVITY;      return 0;break; /* Mask IC_INT_ACTIVITY */
		case IC_INT_STOP_DET:       I2C->IC_CLR_INTR |= IC_INT_STOP_DET;      return 0;break; /* Mask IC_INT_STOP_DET */
		case IC_INT_START_DET:      I2C->IC_CLR_INTR |= IC_INT_START_DET;     return 0;break; /* Mask IC_INT_START_DET */
		case IC_INT_GEN_CALL:       I2C->IC_CLR_INTR |= IC_INT_GEN_CALL;      return 0;break; /* Mask IC_INT_GEN_CALL */
		case IC_INT_RESTART_DET:    I2C->IC_CLR_INTR |= IC_INT_RESTART_DET;   return 0;break; /* Mask IC_INT_RESTART_DET */
		case IC_INT_MST_ON_HOLD:    I2C->IC_CLR_INTR |= IC_INT_MST_ON_HOLD;   return 0;break; /* Mask IC_INT_MST_ON_HOLD */
		case IC_INT_SCL_STUCK_AT_LOW:    I2C->IC_CLR_INTR |= IC_INT_SCL_STUCK_AT_LOW;       return 0;break; /* Mask IC_INT_SCL_STUCK_AT_LOW */
#endif
		case IC_INT_ALL:            reg_writel(IC_INT_ALL,I2C,IC_CLR_INTR);             return 0;break; /* Mask all interrupt */
		case IC_INT_NONE:           reg_clearl(IC_INT_ALL, I2C, IC_CLR_INTR);           return 0;break; /* Mask none interrupt */
		case IC_INT_RX_UNDE:        reg_writel(IC_INT_RX_UNDE, I2C, IC_CLR_INTR);       return 0;break; /* Mask IC_INT_RX_UNDE */
		case IC_INT_RX_OVER:        reg_writel(IC_INT_RX_OVER, I2C, IC_CLR_INTR);       return 0;break; /* Mask IC_INT_RX_OVER */
		case IC_INT_RX_FULL:        reg_writel(IC_INT_RX_FULL, I2C, IC_CLR_INTR);       return 0;break; /* Mask IC_INT_RX_FULL */
		case IC_INT_TX_OVER:        reg_writel(IC_INT_TX_OVER, I2C, IC_CLR_INTR);       return 0;break; /* Mask IC_INT_TX_OVER */
		case IC_INT_TX_EMPTY:       reg_writel(IC_INT_TX_EMPTY, I2C, IC_CLR_INTR);      return 0;break; /* Mask IC_INT_TX_EMPTY */
		case IC_INT_RD_REQ:         reg_writel(IC_INT_RD_REQ, I2C, IC_CLR_INTR);        return 0;break; /* Mask IC_INT_RD_REQ */
		case IC_INT_TX_ABRT:        reg_writel(IC_INT_TX_ABRT, I2C, IC_CLR_INTR);       return 0;break; /* Mask IC_INT_TX_ABRT */
		case IC_INT_RX_DONE:        reg_writel(IC_INT_RX_DONE, I2C, IC_CLR_INTR);       return 0;break; /* Mask IC_INT_RX_DONE */
		case IC_INT_ACTIVITY:       reg_writel(IC_INT_ACTIVITY, I2C, IC_CLR_INTR);      return 0;break; /* Mask IC_INT_ACTIVITY */
		case IC_INT_STOP_DET:       reg_writel(IC_INT_STOP_DET, I2C, IC_CLR_INTR);      return 0;break; /* Mask IC_INT_STOP_DET */
		case IC_INT_START_DET:      reg_writel(IC_INT_START_DET, I2C, IC_CLR_INTR);     return 0;break; /* Mask IC_INT_START_DET */
		case IC_INT_GEN_CALL:       reg_writel(IC_INT_GEN_CALL, I2C, IC_CLR_INTR);      return 0;break; /* Mask IC_INT_GEN_CALL */
		case IC_INT_RESTART_DET:    reg_writel(IC_INT_RESTART_DET, I2C, IC_CLR_INTR);   return 0;break; /* Mask IC_INT_RESTART_DET */
		case IC_INT_MST_ON_HOLD:    reg_writel(IC_INT_MST_ON_HOLD, I2C, IC_CLR_INTR);   return 0;break; /* Mask IC_INT_MST_ON_HOLD */
		case IC_INT_SCL_STUCK_AT_LOW:    reg_writel(IC_INT_SCL_STUCK_AT_LOW, I2C, IC_CLR_INTR);       return 0;break; /* Mask IC_INT_SCL_STUCK_AT_LOW */
		default:return 1;break;
	}
}
/*Function13-3:set INTR mask or umask,read bit[15:0] of IC_RAW_INRT_STAT reg
 *INRT_Item could be one item or multiple ,IC_INT_RX_UNDE or IC_INT_RX_UNDE|IC_INT_RX_OVER|....
 * */
Bit32u I2C_Mask_IT_CMD(IC_INTR_Typedef INTR_Item,IC_ITMask_TypeDef MaskState)
{
	if(MaskState==IC_IT_MASK)
	{
		//I2C->IC_CLR_INTR &= ~INTR_Item;
		reg_clearl(INTR_Item,I2C,IC_CLR_INTR);
		return 0;
	}
	else if(MaskState==IC_IT_UMASK)
	{
		//I2C->IC_CLR_INTR |= INTR_Item;
		reg_writel(INTR_Item,I2C,IC_CLR_INTR);
		return 0;
	}
	else
		return 1;
}

/*Function14-1:Query raw interrupt status,read bit[14:0] of IC_RAW_INRT_STAT reg*/
ITStatus_TypeDef I2C_Get_ITRStat(IC_INTR_Typedef INRT_Item)
{
	ITStatus_TypeDef bitstatus=INACTIVE;
	//if (!(I2C->IC_RAW_INTR_STAT & INRT_Item))
	if (!(reg_readl(I2C,IC_RAW_INTR_STAT) & INRT_Item))
	{
		bitstatus = INACTIVE;             /* Interrupt is inactive */
	}
	else
	{
		bitstatus = ACTIVE;               /* Interrupt is active */
	}
	return bitstatus;
}

/*Function15-1:set RX FIFO Threshold ,set bit[7:0] of IC_RX_TL reg*/
Bit32u I2C_Set_RXThreshold(Bit32u ThresholdValue)
{
	//I2C->IC_RX_TL = ThresholdValue;
	reg_direct_writel(ThresholdValue, I2C, IC_RX_TL);
	return 0;
}

/*Function16-2:set TX FIFO Threshold ,set bit[7:0] of IC_TX_TL reg*/
Bit32u I2C_Set_TXThreshold(Bit32u ThresholdValue)
{
	//I2C->IC_TX_TL = ThresholdValue;
	reg_direct_writel(ThresholdValue, I2C, IC_TX_TL);
	return 0;
}

/*Function17-1:Clear interrupt status,read IC_CLR_INTR or individua interrupt clear reg :
 * !!MST_ON_HOLD/TX_EMPTY/RX_FULL Set and Cleared by Hardware*/
Bit32u I2C_Clear_ITStat(IC_INTR_Typedef INRT_Item)
{
	Bit32u temp;
	switch(INRT_Item)
	{
#if 0
		case IC_INT_ALL:            temp=I2C->IC_CLR_INTR;       return 0;break; /* clear all interrupt */
		case IC_INT_RX_UNDE:        temp=I2C->IC_CLR_RX_UNDER;   return 0;break; /* clear RX_UNDER*/
		case IC_INT_RX_OVER:        temp=I2C->IC_CLR_RX_OVER;    return 0;break; /* clear RX_OVER*/
		case IC_INT_TX_OVER:        temp=I2C->IC_CLR_TX_OVER;    return 0;break; /* clear TX_OVER*/
		case IC_INT_RD_REQ:         temp=I2C->IC_CLR_RD_REQ;     return 0;break; /* clear RD_REQ*/
		case IC_INT_TX_ABRT:        temp=I2C->IC_CLR_TX_ABRT;    return 0;break; /* clear TX_ABRT*/
		case IC_INT_RX_DONE:        temp=I2C->IC_CLR_RX_DONE;    return 0;break; /* clear RX_DONE*/
		case IC_INT_ACTIVITY:       temp=I2C->IC_CLR_ACTIVITY;   return 0;break; /* clear ACTIVITY*/
		case IC_INT_STOP_DET:       temp=I2C->IC_CLR_STOP_DET;   return 0;break; /* clear STOP_DET*/
		case IC_INT_START_DET:      temp=I2C->IC_CLR_START_DET;  return 0;break; /* clear START_DET*/
		case IC_INT_GEN_CALL:       temp=I2C->IC_CLR_GEN_CALL;   return 0;break; /* clear GEN_CALL*/
		case IC_INT_RESTART_DET:    temp=I2C->IC_CLR_RESTART_DET;return 0;break; /* clear RESTART_DET*/
		case IC_INT_SCL_STUCK_AT_LOW:    temp=I2C->IC_CLR_SCL_STUCK_DET;return 0;break; /* clear RESTART_DET*/
#endif
		case IC_INT_ALL:            temp=reg_readl(I2C,IC_CLR_INTR);       return 0;break; /* clear all interrupt */
		case IC_INT_RX_UNDE:        temp=reg_readl(I2C,IC_CLR_RX_UNDER);   return 0;break; /* clear RX_UNDER*/
		case IC_INT_RX_OVER:        temp=reg_readl(I2C,IC_CLR_RX_OVER);    return 0;break; /* clear RX_OVER*/
		case IC_INT_TX_OVER:        temp=reg_readl(I2C,IC_CLR_TX_OVER);    return 0;break; /* clear TX_OVER*/
		case IC_INT_RD_REQ:         temp=reg_readl(I2C,IC_CLR_RD_REQ);     return 0;break; /* clear RD_REQ*/
		case IC_INT_TX_ABRT:        temp=reg_readl(I2C,IC_CLR_TX_ABRT);    return 0;break; /* clear TX_ABRT*/
		case IC_INT_RX_DONE:        temp=reg_readl(I2C,IC_CLR_RX_DONE);    return 0;break; /* clear RX_DONE*/
		case IC_INT_ACTIVITY:       temp=reg_readl(I2C,IC_CLR_ACTIVITY);   return 0;break; /* clear ACTIVITY*/
		case IC_INT_STOP_DET:       temp=reg_readl(I2C,IC_CLR_STOP_DET);   return 0;break; /* clear STOP_DET*/
		case IC_INT_START_DET:      temp=reg_readl(I2C,IC_CLR_START_DET);  return 0;break; /* clear START_DET*/
		case IC_INT_GEN_CALL:       temp=reg_readl(I2C,IC_CLR_GEN_CALL);   return 0;break; /* clear GEN_CALL*/
		case IC_INT_RESTART_DET:    temp=reg_readl(I2C,IC_CLR_RESTART_DET);return 0;break; /* clear RESTART_DET*/
		case IC_INT_SCL_STUCK_AT_LOW:    temp=reg_readl(I2C,IC_CLR_SCL_STUCK_DET);return 0;break; /* clear RESTART_DET*/
		default:return 1;break;
	}
}

/*Function28-1:SDA stuck recovery ,set bit3 of IC_ENABLE reg */
Bit32u I2C_SDA_Reco_CMD(State_TypeDef NewState)
{
	if (NewState == ENABLE)
	{
		//I2C->IC_ENABLE |= IC_ENABLE_SDA_STUCK_RECOVERY_ENABLE;
		reg_writel(IC_ENABLE_SDA_STUCK_RECOVERY_ENABLE,I2C,IC_ENABLE);
		return 0;
	}
	else if(NewState == DISABLE)
	{
		//I2C->IC_ENABLE &= ~IC_ENABLE_SDA_STUCK_RECOVERY_ENABLE;
		reg_clearl(IC_ENABLE_SDA_STUCK_RECOVERY_ENABLE,I2C,IC_ENABLE);
		return 0;
	}
	else
		return 1;
}

/*Function28-2:I2C block transmission enable,set bit2 of IC_ENABLE reg */
Bit32u I2C_Block_Trans_CMD(State_TypeDef NewState)
{
	if (NewState == ENABLE)
	{
		//I2C->IC_ENABLE |= IC_ENABLE_TX_CMD_BLOCK;
		reg_writel(IC_ENABLE_TX_CMD_BLOCK,I2C,IC_ENABLE);
		return 0;
	}
	else if(NewState == DISABLE)
	{
		//I2C->IC_ENABLE &= ~IC_ENABLE_TX_CMD_BLOCK;
		reg_clearl(IC_ENABLE_TX_CMD_BLOCK,I2C,IC_ENABLE);
		return 0;
	}
	else
		return 1;
}

/*Function28-3:I2C abort enable,set bit1 of IC_ENABLE reg */
Bit32u I2C_Abort_CMD(State_TypeDef NewState)
{
	if (NewState == ENABLE)
	{
		//I2C->IC_ENABLE |= IC_ENABLE_ABORT;
		reg_writel(IC_ENABLE_ABORT,I2C,IC_ENABLE);
		return 0;
	}
	else if(NewState == DISABLE)
	{
		//I2C->IC_ENABLE &= ~IC_ENABLE_ABORT;
		reg_clearl(IC_ENABLE_ABORT,I2C,IC_ENABLE);
		return 0;
	}
	else
		return 1;
}

/*Function28-4:I2C enable,set bit0 of IC_ENABLE reg */
Bit32u I2C_CMD(State_TypeDef NewState)
{
	if (NewState == ENABLE)
	{
		//I2C->IC_ENABLE |= IC_ENABLE_ENABLE;
		reg_writel(IC_ENABLE_ENABLE,I2C,IC_ENABLE);

		return 0;
	}
	else if(NewState == DISABLE)
	{
		//I2C->IC_ENABLE &= ~IC_ENABLE_ENABLE;
		reg_clearl(IC_ENABLE_ENABLE,I2C,IC_ENABLE);
		return 0;
	}
	else
		return 1;
}

/*Function29-1:Query I2C status,read bit[11:0] of IC_STATUS reg*/
Bit32u I2C_Get_Stat(IC_STATUS_Typedef StatusItem)
{
	//if ((I2C->IC_STATUS & StatusItem) != 0)
	if ((reg_readl(I2C,IC_STATUS) & StatusItem) != 0)
	{
		return 1;
 	}
	else
		return 0;
}

/*Function32-1:set I2C RX and TX SDA hold time ,set bit[23:16] /bit[15:0]of IC_SDA_HOLD reg*/
Bit32u I2C_Set_SDA_Hold(Bit32u RX_Holdtime,Bit32u TX_Holdtime)
{
	Bit32u temp=0;
	temp=(IC_SDA_RX_HODL(RX_Holdtime)&0xFF0000)+(IC_SDA_TX_HODL(TX_Holdtime)&0xFFFF);
	//I2C->IC_SDA_HOLD = temp;
	reg_direct_writel(temp,I2C,IC_SDA_HOLD);
	return 0;
}

/*Function42-1:set IC_FS_SPKLEN ,set bit[7:0] of IC_FS_SPKLEN reg*/
Bit32u I2C_Set_FS_SPKLEN(Bit8u SpikeLength)
{
	//I2C->IC_FS_SPKLEN = SpikeLength;
	reg_direct_writel(SpikeLength,I2C,IC_FS_SPKLEN);
	SPKLEN_FS=SpikeLength;
	return 0;
}

/*Function43-1:set IC_HS_SPKLEN ,set bit[7:0] of IC_HS_SPKLEN reg*/
Bit32u I2C_Set_HS_SPKLEN(Bit8u SpikeLength)
{
	//I2C->IC_HS_SPKLEN = SpikeLength;
	reg_direct_writel(SpikeLength,I2C,IC_HS_SPKLEN);
	SPKLEN_HS=SpikeLength;
	return 0;
}

/*Function45-1:Query CompVersion,read  IC_COMP_VERSION reg*/
Bit32u I2C_Get_CompVer(void)
{
	//Bit32u temp=I2C->IC_COMP_VERSION;
	Bit32u temp=reg_readl(I2C,IC_COMP_VERSION);
	return temp;
}

/*Function47-1:set SCL_STUCK_AT_LOW_TIMEOUT ,set bit[31:0] of IC_SCL_STUCK_AT_LOW_TIMEOUT reg*/
Bit32u I2C_Set_SCL_STUCK_AT_LOW_TIMEOUT(Bit32u Timeout)
{
	//I2C->IC_SCL_STUCK_AT_LOW_TIMEOUT = Timeout;
	reg_direct_writel(Timeout, I2C, IC_SCL_STUCK_AT_LOW_TIMEOUT);
	return 0;
}

/*Function48-1:set SDA_STUCK_AT_LOW_TIMEOUT ,set bit[31:0] of IC_SDA_STUCK_AT_LOW_TIMEOUT reg*/
Bit32u I2C_Set_SDA_STUCK_AT_LOW_TIMEOUT(Bit32u Timeout)
{
	//I2C->IC_SDA_STUCK_AT_LOW_TIMEOUT = Timeout;
	reg_direct_writel(Timeout, I2C, IC_SDA_STUCK_AT_LOW_TIMEOUT);
	return 0;
}

/*Function50-1:Query Device ID,read  IC_DEVICE_ID reg*/
Bit32u I2C_Get_DeviceID(void)
{
	//Bit32u temp=I2C->IC_DEVICE_ID;
	Bit32u temp=reg_readl(I2C,IC_DEVICE_ID);
	return temp;
}

/**********Test functions*************/

void I2C_Deactive(void)
{
	I2C_CMD(DISABLE);
}

/*I2C Master mode work flow*/
void I2C_Config(IC_Mode_Typedef Mode,IC_AddrMode_Typedef AddrMode,IC_SpeedMode_Typedef SpeedMode)
{
	I2C_Set_Mode(Mode);
	I2C_Set_AddrMode(AddrMode);
	I2C_Set_SpeedMode(SpeedMode);
	I2C_Restart_EN(ENABLE);
}

/* config intc,register i2c isq  */
Bit32u I2C_IT_Register(void)
{
	/* clear interrupt flag */
	I2C_Clear_ITStat(IC_INT_ALL);

    CK_INTC_RequestIrq(CK_IIC_IRQ, CK_I2C_Handler);
    return 0;
}

void CK_I2C_Handler(Bit32u irqid)
{
	if(I2C_Get_ITRStat(IC_INT_TX_EMPTY))
	{
		//if(*p!=20)
			//I2C_Send_Data(p++);
		//else
			//I2C_Stop_CMD(ENABLE);
	}
	else if(I2C_Get_ITRStat(IC_INT_RD_REQ))
	{
		I2C_Clear_ITStat(IC_INT_ALL);
	}
	else if(I2C_Get_ITRStat(IC_INT_TX_ABRT))
	{
		I2C_Clear_ITStat(IC_INT_ALL);
	}
	else if(I2C_Get_ITRStat(IC_INT_RX_DONE))
	{
		I2C_Clear_ITStat(IC_INT_ALL);
	}
	else if(I2C_Get_ITRStat(IC_INT_ACTIVITY))
	{
		I2C_Clear_ITStat(IC_INT_ALL);
	}
	else if(I2C_Get_ITRStat(IC_INT_STOP_DET))
	{
		I2C_Clear_ITStat(IC_INT_ALL);
	}
	else if(I2C_Get_ITRStat(IC_INT_START_DET))
	{
		I2C_Clear_ITStat(IC_INT_ALL);
	}
	else if(I2C_Get_ITRStat(IC_INT_GEN_CALL))
	{
		I2C_Clear_ITStat(IC_INT_ALL);
	}
	else if(I2C_Get_ITRStat(IC_INT_RESTART_DET))
	{
		I2C_Clear_ITStat(IC_INT_ALL);
	}
	else if(I2C_Get_ITRStat(IC_INT_MST_ON_HOLD))
	{
		I2C_Clear_ITStat(IC_INT_ALL);
	}
	else if(I2C_Get_ITRStat(IC_INT_SCL_STUCK_AT_LOW))
	{
		I2C_Clear_ITStat(IC_INT_ALL);
	}
	else
		I2C_Clear_ITStat(IC_INT_ALL);
}







