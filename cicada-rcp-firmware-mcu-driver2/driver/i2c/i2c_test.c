#include "misc.h"
#include "intc.h"
#include "i2c.h"
#include "misc.h"

static Bit8u Device_Addr=0x50;

//static Bit32u IC_EMPTYFIFO_HOLD_MASTER_EN = 1;
static Bit8u temp[20]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
Bit8u *p=temp;
void I2C_Test1(void)
{
	I2C_Deactive();
	I2C_Config(MASTER,address_bit_7,SPEED_STANDARD_MODE);
	I2C_Set_TAR_Addr(0x01);
	I2C_Set_SS_SCL_FREQ(100);
    I2C_IT_Register();
	I2C_UMask_IT(IC_INT_ALL);
	I2C_Set_RXThreshold(0);
	I2C_Set_TXThreshold(0);
	I2C_CMD(ENABLE);
	I2C_Send_Data(*p++);
	while(1);
}

void I2C_Init_For_EEPROM(void)
{
	I2C_Deactive();
	I2C_Config(MASTER,address_bit_7,SPEED_STANDARD_MODE);
	I2C_Set_TAR_Addr(Device_Addr);
	I2C_Set_SS_SCL_FREQ(100);
    //I2C_INTC_Register(void);
	I2C_UMask_IT(IC_INT_ALL);
	I2C_Set_RXThreshold(0);
	I2C_Set_TXThreshold(0);
	I2C_CMD(ENABLE);
}

/*EEPROM Byte write*/
void EEPROM_Byte_Write(Bit32u Address,Bit8u Data)
{
	Bit32u a=2000;
	Bit8u Addr_H,Addr_L=0;
	ITStatus_TypeDef ITstatus=INACTIVE;
	Addr_L=Address&0xFF;
	Addr_H=(Address>>8)&0xFF;
	I2C_Set_Restart(DISABLE);
	I2C_Set_Stop(DISABLE);

	I2C_Send_Data(Addr_H);

	/*while(!ITstatus)
	{
		ITstatus=I2C_Get_ITStat(IC_INT_TX_EMPTY);
	}
	*/
	/*ACK???*/
	I2C_Send_Data(Addr_L);

	ITstatus=INACTIVE;
	while(!ITstatus)
	{
		ITstatus=I2C_Get_ITStat(IC_INT_TX_EMPTY);
	}
	/*ACK???*/
	I2C_Set_Stop(ENABLE);
	I2C_Send_Data(Data);
	while(a--);
}

/*EEPROM Byte Read*/
Bit8u a[]={0};
void EEPROM_Byte_Read(Bit32u Startaddr,Bit32u Datasize)
{
	Bit32u i=0;
	Bit8u Addr_H,Addr_L=0;
	ITStatus_TypeDef ITstatus=INACTIVE;
	Addr_L=Startaddr&0xFF;
	Addr_H=(Startaddr>>8)&0xFF;
	I2C_Set_Restart(DISABLE);
	I2C_Set_Stop(DISABLE);

	I2C_Send_Data(Addr_H);

	ITstatus=INACTIVE;
	while(!I2C_Get_ITStat(IC_INT_TX_EMPTY));
	I2C_Send_Data(Addr_L);

	I2C_Set_Restart(ENABLE);    //Restart
	I2C_Receive_Set();
	I2C_Set_Restart(DISABLE);    //Restart
#if 0
	for(i=0;i<Datasize;i++)    // receive data
	{
		ITstatus=INACTIVE;
		while(!ITstatus)
		{
			ITstatus=I2C_Get_ITStat(IC_INT_RX_FULL);
		}
		a[i]=I2C_Receive_Data();
		I2C_Receive_Set();
	}
	I2C_Set_Restart(DISABLE);    //stop
	I2C_Set_Stop(ENABLE);
	I2C_Receive_Set();
#endif
	for(i=0;i<Datasize-1;i++)    // receive data
	{
		ITstatus=INACTIVE;
		while(!ITstatus)
		{
			ITstatus=I2C_Get_ITStat(IC_INT_RX_FULL);
		}
		//while(!I2C_Get_ITStat(IC_INT_RX_FULL));
		a[i]=I2C_Receive_Data();
		if(i<Datasize-2)
			I2C_Receive_Set();
	}
	I2C_Set_Restart(DISABLE);    //stop
	I2C_Set_Stop(ENABLE);
	I2C_Receive_Set();

	ITstatus=INACTIVE;
	while(!ITstatus)
	{
		ITstatus=I2C_Get_ITStat(IC_INT_RX_FULL);
	}
	//while(!I2C_Get_ITStat(IC_INT_RX_FULL));
	a[Datasize-1]=I2C_Receive_Data();
}

void I2C_Test_For_EEPROM(void)
{
	Bit8u tmp[80]={0};
	//Bit8u temp1[50]={0};
	Bit16u i = 0;
	Bit16u j = 10000;

	printf("IIC test begins! \n");
	I2C_Init_For_EEPROM();
	printf(" Init:Compelte! \n");

	for(i=0;i<80;i++)
	{
		tmp[i]=i+1;
		EEPROM_Byte_Write(i,tmp[i]);
	}
	printf(" Write:Compelte! \n");

	while(j--);    //delay 10ms?
	EEPROM_Byte_Read(0,50);
	printf(" Read:Compelte! \n");

	for(i=0;i<50;i++)
	{
		printf(" Send data:%x; Receive data :%x \n", tmp[i],a[i]);
		if(tmp[i]!=a[i])
		{
			printf(" ERROR \n");
			return;
		}
	}
	I2C_CMD(DISABLE);    ///close IIC
	printf("IIC test success! \n");

}

void I2C_Test_For_Uartdedug(Bit32u Readaddr,Bit32u Writeaddr,Bit32u Readnum)
{
	Bit32u i = 0;
	Bit32u j = 10000;

	printf("IIC test begins! \n");
	I2C_Init_For_EEPROM();
	while(j--);    //delay 10ms?
	EEPROM_Byte_Read(Readaddr,Readnum);

	printf(" Read:Compelte! \n");
	j = 10000;
	while(j--);    //delay 10ms?
	for(i=0;i<Readnum;i++)
	{
		printf("Readed Addr:%x; Readed data:%x \n", Readaddr++,a[i]);
		EEPROM_Byte_Write(Writeaddr,a[i]);
	}
	printf(" Write:Compelte! \n");
	I2C_CMD(DISABLE);    ///close IIC
	printf("IIC test success! \n");

}
