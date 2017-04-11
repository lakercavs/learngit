/*
 * uart_debug.c
 *
 *  Created on: 2017-1-13
 *      Author: i
 */
#include "ck810.h"
#include "uart.h"
#include "misc.h"
#include <string.h>
#include "circlebuffer.h"
#include "ckuart.h"
#include "intc.h"
#include <math.h>
#include <stdio.h>
#include "i2c.h"
#include "gpio.h"
#include "timer.h"
#include "wdt.h"

extern CK_Uart_Device consoleuart;
extern Bit8u a[];
static int getnum = 0;

CK_UINT32 Getchar_Deal(const char *source,char *temp1,char *temp2,char *temp3)
{
	CK_UINT8 i=2;
	CK_UINT8 j=0;
	BOOL flag1=FALSE;
	BOOL flag2=FALSE;
	BOOL flag3=FALSE;

	while(!flag1)
	{
		if((*(source+i)!=' ')&&(*(source+i)!='\n')&&(*(source+i)!='\r')&&(*(source+i)!='\0'))
		{
			*(temp1+j)=*(source+i);
			j++;
		}
		else
		{
			*(temp1+j)='\0';
			j=0;
			flag1=TRUE;
		}
		i++;
	}
	//printf("receive char1: %s\n",temp1);
	while(!flag2)
	{
		if((*(source+i)!=' ')&&(*(source+i)!='\n')&&(*(source+i)!='\r')&&(*(source+i)!='\0'))
		{
			*(temp2+j)=*(source+i);
			j++;
		}
		else
		{
			*(temp2+j)='\0';
			j=0;
			flag2=TRUE;
		}
		i++;
	}
	//printf("receive char2: %s\n",temp2);
	while(!flag3)
	{
		if((*(source+i)!=' ')&&(*(source+i)!='\n')&&(*(source+i)!='\r')&&(*(source+i)!='\0'))
		{
			*(temp3+j)=*(source+i);
			j++;
		}
		else
		{
			*(temp3+j)='\0';
			j=0;
			flag3=TRUE;
		}
		i++;
	}

/*judge command is correct*/
	if (((*temp1) != '\0') && ((*temp2) != '\0') && ((*temp3) != '\0'))                      //r&i 3 temp cannot be empty
	{
		if ((*temp1 == '0') && (*(temp1 + 1) == 'x') && (*temp3 == '0') && (*(temp3 + 1) == 'x')) //temp1 and temp3 must begin with 0x
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if((*source == 't')&&((*temp1) != '\0')&&((*temp2) == '\0')&&((*temp3) == '\0'))    //t command
	{
		return 1;
	}
	else if((*source == 't')&&((*temp1) != '\0')&&((*temp2) != '\0')&&((*temp3) == '\0'))    //t m 0xxx command
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

CK_UINT32 Hextemp_deal(char *temp)
{
	CK_UINT8 i=0;
	CK_UINT8 j=0;
	CK_UINT8 l=0;
	char asic_char[100]={0};
	CK_INT32 value=0;
	CK_INT32 stringlenth=strlen(temp);
	//for(l=0;l<stringlenth;l++)
	//	printf("before deal:%x!\n",*(temp+l));
	for(i=2;i<stringlenth;i++)
	{
		if((*(temp+i)>='a')&&(*(temp+i)<='f'))  //char a~f
		{
			*(asic_char+j)=*(temp+i)-87;               //ASCII Transformation
			j++;
		}
		else if((*(temp+i)>='0')&&(*(temp+i)<='9'))// char 0~9
		{
			*(asic_char+j)=*(temp+i)-48;
			j++;
		}
		else if(*(temp+i)=='\0')                   //end char
		{
			//printf("Deal end!\n");
		}
		else                                     //wrong char
		{
			printf("Addr input Error!\n");
			return -1;
		}
	}
	for(l=0;l<j;l++)
	{
		value=value+(*(asic_char+l)*(CK_INT32)pow(16.0,j-1-l));
	}
	return value;
}

CK_UINT32 Dectemp_deal(char *temp)
{
	CK_UINT8 i=0;
	CK_UINT8 j=0;
	CK_UINT8 l=0;
	char  asic_char[100]={0};
	CK_INT32 value=0;
	CK_UINT8 stringlenth=strlen(temp);
	for(i=0;i<stringlenth;i++)
	{
		if((*(temp+i)>='0')&&(*(temp+i)<='9'))    // char 0~9
		{
			*(asic_char+j)=*(temp+i)-48;
			j++;
		}
		else if(*(temp+i)=='\0')                   //end char
		{

		}
		else                                     //wrong char
		{
			printf("Data num input Error!\n");
			return -1;
		}
	}
	for(l=0;l<j;l++)
	{
		value=value+(*(asic_char+l)*(CK_INT32)pow(10.0,j-1-l));
	}
	//printf("value:%d\n",value);
	return value;
}


/*****************************************************************************/
void CK_UART_Debug(void)
{
	CK_UINT8 i=0;
	CK_UINT8 j=0;
	CK_UINT8 m=0;
	CK_UINT32 l=0;
	CK_UINT8 ch=0;
	CK_UINT32 getvalue1=0;
	CK_UINT32 getvalue2=0;
	CK_UINT32 getflag=0;
	CK_UINT32 rdatanum=0;
	CK_UINT32 ptemp=0;
	CK_UINT32 Startaddr=0x2000000;  //Mem pool start addr 32M
	//CK_UINT32 Stopaddr=0x4000000;   //Mem pool start addr 32M
	CK_UINT32 Banksize=0x400000;    //Mem pool BANK size = (stopaddr -startaddr)/8
	CK_UINT32 Cellsize=0x80000;     //Mem pool Cell size = (Banksize)/8
	CK_UINT32 Bankavail=0x20000;    //Mem pool available addr size of every cell,the left cell is not available
	CK_UINT32 Addrstep=0x04;           //Mem pool test step,4 Byte is the least size
	CK_UINT32 availnum = 0;
	CK_UINT8 memtest_status=1;

	CK_UINT32 *rsrcaddr=NULL;
	CK_UINT32 *rdstaddr=NULL;

    char RXdata[100]={0};
    char srcaddr[100]={0};
    char dstaddr[100]={0};
    char datanum[100]={0};

    while(1)
    {
    	/*clear Rxdata[]*/
        for(i = 0; i < 100; i++)
        {
          *(RXdata+i) = '\0';
        }
        i = 0;
    	/*get input char,*/
    	while(1)
		{
			if(SUCCESS == CK_Uart_GetChar(consoleuart, &ch))
			{
				if( (ch == '\n')||(ch == '\r'))                // get enter
				{
					*(RXdata+i) = '\0';
					getnum=i;
					if( ch == '\n')
					{
						printf("Receive Command: %s\n",RXdata);
						i=0;
						break;
					}

				}
				else if(ch == '\b')              // get back
				{
					i--;
				}
				else                             //get normal char and \r
				{
					*(RXdata+i) = ch;
					i++;
				}
			}
		}

    	/*get end,judge command */
        if(*RXdata=='r')              //get r 0x19000000 100 0x190000200
        {
        	getflag=Getchar_Deal(RXdata,srcaddr,datanum,dstaddr);
        	if(getflag==1)               //command correct then operate the command
        	{
        		getvalue1 = Hextemp_deal(srcaddr);
        		getvalue2 = Hextemp_deal(dstaddr);
        		rdatanum =  Dectemp_deal(datanum);
				if ((getvalue1 != -1) && (getvalue2!= -1) && (rdatanum != -1))
				{
					printf("Get Bus read command!\n");
					printf("src addr: %x;dst addr: %x;read num: %d\n", getvalue1, getvalue2, rdatanum);
	        		rsrcaddr = (CK_UINT32 *)getvalue1;
	        		rdstaddr = (CK_UINT32 *)getvalue2;
	        		for(j=0;j<rdatanum;j++)
	        		{
	        			ptemp=*rsrcaddr;
	        			*rdstaddr=ptemp;
	        			printf("Read data%d:%x;Addr:%x\n",j,ptemp,rsrcaddr);
	        			rsrcaddr++;
	        			rdstaddr++;
	        		}
				}
				else
				{
					printf("Command input Error!\n");
				}
        	}
        	else
        	{
        		printf("Command input Error!\n");
        	}
        }

        else if(*RXdata=='i')                    //get i 0x0000 100 0x00200
        {
        	getflag=Getchar_Deal(RXdata,srcaddr,datanum,dstaddr);
        	if(getflag==1)               //command correct then operate the command
        	{
        		getvalue1 = Hextemp_deal(srcaddr);
        		getvalue2 = Hextemp_deal(dstaddr);
        		rdatanum =  Dectemp_deal(datanum);
        		if ((getvalue1 != -1) && (getvalue2 != -1) && (rdatanum != -1))
				{
					printf("Get IIC read/write command!\n");
					printf("src addr: %x;dst addr: %x;read num: %d\n", getvalue1, getvalue2, rdatanum);
					I2C_Test_For_Uartdedug(getvalue1,getvalue2,rdatanum);
				}
				else
				{
					printf("Command input Error!\n");
				}
        	}
        	else
        	{
        		printf("Command input Error!\n");
        	}
        }
        else if((*RXdata=='g')&&(*(RXdata+1)=='a')&&(*(RXdata+2)=='\0')) //get ga
        {
        	printf("Get GPIO read command!\n");
        	getvalue1=GPIOA_Get_Ext_Data(GPIO_Pin_0);
        	printf("Pin0 of PortA :%x \n", getvalue1);
        	getvalue1=GPIOA_Get_Ext_Data(GPIO_Pin_1);
        	printf("Pin1 of PortA :%x \n", getvalue1);
        	getvalue1=GPIOB_Get_Ext_Data(GPIO_Pin_1);
        	printf("Pin0 of PortB :%x \n", getvalue1);
        	getvalue1=GPIOC_Get_Ext_Data(GPIO_Pin_1);
        	printf("Pin0 of PortC :%x \n", getvalue1);
        	getvalue1=GPIOD_Get_Ext_Data(GPIO_Pin_1);
        	printf("Pin0 of PortD :%x \n", getvalue1);
        }
        else if((*RXdata=='t')&&(*(RXdata+1)=='a')&&(*(RXdata+2)=='\0'))//get ta
        {
        	printf("Get Timers test command!\n");
        	CK_Timer_Test();
        }
        else if((*RXdata=='w')&&(*(RXdata+1)=='a')&&(*(RXdata+2)=='\0'))//get wa
        {
        	printf("Get WDT test command!\n");
        	WDT_Test1();
        }

        else if((*RXdata=='t')&&(*(RXdata+1)==' ')&&(*(RXdata+2)=='0'))                   //get t 0x19000000
        {
        	getflag=Getchar_Deal(RXdata,srcaddr,datanum,dstaddr);
        	 if(getflag==1)               //command correct then operate the command
        	 {
 				getvalue1 = Hextemp_deal(srcaddr);
 				if (getvalue1 != -1)
 				{
 					printf("Get addr test command!\n");
 					printf("test addr: %x;\n", getvalue1);
 	        		rsrcaddr = (CK_UINT32 *)getvalue1;
 	        		*rsrcaddr = 0x55;
 	        		ptemp = *rsrcaddr;
 	        		if(ptemp==0x55)
						printf("step1:write data 0x55,read data :%x;\n",ptemp);
 	        		*rsrcaddr = 0xaa;
 	        		ptemp = *rsrcaddr;
 	        		if(ptemp==0xaa)
						printf("step2:write data 0xaa,read data :%x;\n",ptemp);
 				}
 				else
 				{
 					printf("Command input Error!\n");
 				}
 			}
 			else
 			{
 				printf("Command input Error!\n");
 			}
        }

#if defined (CONFIG_CKCPU_DEBUG_MEM)
        else if((*RXdata=='t')&&(*(RXdata+1)==' ')&&(*(RXdata+2)=='m'))             //get t m 0x04 command
        {
        	getflag=Getchar_Deal(RXdata,srcaddr,datanum,dstaddr);
        	if(getflag==1)               //command correct then operate the command
        	{
        		Addrstep = Hextemp_deal(datanum);
        		availnum=Bankavail/Addrstep;
				printf("Get Mem pool test command!\n");
				printf("Addrstep:%x;\n", Addrstep);
        		if(Addrstep%4)           //Addrstep must be in multiples of four
        		{
        			printf("Command input Error!\n");
        			printf("Addrstep must be in multiples of 4!\n");
        		}
        		else
        		{
        			j=0;
                	for(j=0;j<8;j++)           //8 banks
                	{
            			m=0;
            			if(memtest_status)
            			{
                        	//printf("Test bank:%x!\n",j);
                        	for(m=0;m<8;m++)       //each bank has 8 cells
                        	{
                        		l=0;
                        		if(memtest_status)
                        		{
                                	//printf("Test cell:%x!\n",m);
                                	for(l=0;l<availnum;l++)//in each cell,only first 128K available
                                	{
                            			getvalue1=(CK_UINT32)(Startaddr+j*Banksize+m*Cellsize+l*Addrstep);
    									printf("Test addr:%x!\n",getvalue1);
                            			//delay(5);
                            			rsrcaddr=(CK_UINT32 *)getvalue1;
                            			ptemp=*rsrcaddr;
                            			*rsrcaddr=ptemp;
                            			asm("sync 0");
                            			if(ptemp!=*rsrcaddr)      //write data !=read data
                            			{
                            				printf("Mem pool test Error!\n");
                            				printf("Test addr:%x!\n",rsrcaddr);
                            				memtest_status=0;
                            				break;
                            			}
                                	}
                        		}
                        		else break;
                    		}
            			}
            			else break;
                    }
                	if(memtest_status)
                		printf("Mem pool test success!\n");
        		}
        	}
 			else
 			{
 				printf("Command input Error!\n");
 			}
        }
#endif
        else                                   //other command,error
        {
    		printf("Command input Error!\n");
        }
    }
}


