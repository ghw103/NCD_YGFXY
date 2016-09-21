/***************************************************************************************************
*FileName:
*Description:
*Author: xsx_kair
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"RTC_Driver.h"
#include 	"usbd_cdc_vcp.h"

#include	"stm32f4xx_gpio.h"

#include	"MyMem.h"
#include	"Define.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

#include 	"delay.h"
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static void RX_SDA_IN(void);
static void RX_SDA_OUT(void);
static void RX_IIC_Start(void);
static void RX_IIC_Stop(void);
static unsigned char RX8025_Write(unsigned char addr, unsigned char *pdata, unsigned char len);
static unsigned char RX8025_Read(unsigned char addr, unsigned char *pdata, unsigned char len);
static unsigned char BCD2HEX(unsigned char bcd_data);
static unsigned char HEX2BCD(unsigned char hex_data);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

void RTC_BSPInit(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RX_SCK_Rcc | RX_SDA_Rcc, ENABLE);

	GPIO_InitStructure.GPIO_Pin = RX_SCK_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(RX_SCK_Group, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = RX_SDA_Pin;
    GPIO_Init(RX_SDA_Group, &GPIO_InitStructure);
}

/***************************************************************************************************
*FunctionName£ºRX_SDA_IN
*Description£ºÊ±ÖÓÐ¾Æ¬Êý¾ÝÏß¸ü¸ÄÅäÖÃÎªÊä³ö
*Input£ºNone
*Output£ºNone
*Author£ºxsx
*Data£2016Äê9ÔÂ18ÈÕ10:10:12
***************************************************************************************************/
static void RX_SDA_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = RX_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(RX_SDA_Group, &GPIO_InitStructure);
}
/***************************************************************************************************
*FunctionName£ºRX_SDA_OUT
*Description£ºÊ±ÖÓÐ¾Æ¬Êý¾ÝÏß¸ü¸ÄÅäÖÃÎªÊä³ö
*Input£ºNone
*Output£ºNone
*Author£ºxsx
*Data£º2016Äê9ÔÂ18ÈÕ10:10:16
***************************************************************************************************/
static void RX_SDA_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = RX_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(RX_SDA_Group, &GPIO_InitStructure);
}

/***************************************************************************************************
*FunctionName:RX_IIC_Start 
*Description: IIC×ÜÏßÆðÊ¼ÐÅºÅ
*Input: 
*Output: 
*Author: xsx
*Date: 2016Äê9ÔÂ18ÈÕ10:18:20
***************************************************************************************************/
static void RX_IIC_Start(void)
{
	RX_SDA_OUT();
	
	RX_SCK_H();
	RX_SDA_H();
	delay_us(RX_DelayTime);
	
	RX_SDA_L();
	delay_us(RX_DelayTime);
	
	RX_SCK_L();
	delay_us(RX_DelayTime);
}

/***************************************************************************************************
*FunctionName:RX_IIC_Stop 
*Description: IIC×ÜÏß½áÊøÐÅºÅ
*Input: 
*Output: 
*Author: xsx
*Date: 2016Äê9ÔÂ18ÈÕ10:18:35
***************************************************************************************************/
static void RX_IIC_Stop(void)
{
	RX_SDA_OUT();
	
	RX_SCK_L();
	RX_SDA_L();
	delay_us(RX_DelayTime);
	
	RX_SCK_H();
	delay_us(RX_DelayTime);
	
	RX_SDA_H();
	delay_us(RX_DelayTime);
}

static unsigned char RX_IIC_Wait_Ack(void)
{
	unsigned char errortime = 0;
	
	RX_SDA_IN();
	
	RX_SDA_H();
	delay_us(RX_DelayTime);
	
	RX_SCK_H();
	delay_us(RX_DelayTime);
	
	while(RX_SDA_PIN())
	{
		errortime++;
		
		if(errortime > 250)
		{
			RX_IIC_Stop();
			return 0;
		}
	}
	
	RX_SCK_L();
	return 1;
}

//²úÉúACKÓ¦´ð
void RX_IIC_Ack(void)
{
	RX_SCK_L();
	
	RX_SDA_OUT();
	RX_SDA_L();
	delay_us(RX_DelayTime);
	
	RX_SCK_H();
	delay_us(RX_DelayTime);
	RX_SCK_L();
}
//²»²úÉúACKÓ¦´ð		    
void RX_IIC_NAck(void)
{
	RX_SCK_L();
	RX_SDA_OUT();
	RX_SDA_H();
	delay_us(RX_DelayTime);
	
	RX_SCK_H();
	delay_us(RX_DelayTime);
	RX_SCK_L();
}

static unsigned char RX_IIC_WriteByte(unsigned char data)
{
	unsigned char i=0;
	
	RX_SDA_OUT();
	
	RX_SCK_L();
	
	for(i=0; i<8; i++)
	{		
		if(data&0x80)
			RX_SDA_H();
		else
			RX_SDA_L();
		delay_us(RX_DelayTime);
		
		RX_SCK_H();
		delay_us(RX_DelayTime);
		
		RX_SCK_L();
		delay_us(RX_DelayTime);
		
		data <<= 1;
	}
	
	return RX_IIC_Wait_Ack();
}

static unsigned char RX_IIC_ReadByte(unsigned char ack)
{
	unsigned char i, data = 0;
	
	RX_SDA_IN();
	
	for(i=0; i<8; i++)
	{
		RX_SCK_L();
		delay_us(RX_DelayTime);
		
		RX_SCK_H();
		
		data <<= 1;
		
		if(RX_SDA_PIN())
			data++;
		delay_us(RX_DelayTime);
	}
	
	if(!ack)
		RX_IIC_NAck();
	else
		RX_IIC_Ack();
	
	return data;
}

static unsigned char RX8025_Write(unsigned char addr, unsigned char *pdata, unsigned char len)
{
	unsigned char i=0;
	
	RX_IIC_Start();
	
	if(RX_IIC_WriteByte(0x64) == 0)
	{
		RX_IIC_Stop();
		return 0;
	}
	
	if(RX_IIC_WriteByte(addr) == 0)
	{
		RX_IIC_Stop();
		return 0;
	}
	
	for(i=0; i<len; i++)
	{
		if(RX_IIC_WriteByte(*pdata) == 0)
		{
			RX_IIC_Stop();
			return 0;
		}
		pdata++;
	}
	
	RX_IIC_Stop();
	
	return 1;
}

static unsigned char RX8025_Read(unsigned char addr, unsigned char *pdata, unsigned char len)
{
	unsigned char i=0;
	
	RX_IIC_Start();
	
	if(RX_IIC_WriteByte(0x64) == 0)
	{
		RX_IIC_Stop();
		return 0;
	}
	
	if(RX_IIC_WriteByte(addr) == 0)
	{
		RX_IIC_Stop();
		return 0;
	}
	
	RX_IIC_Start();
	
	if(RX_IIC_WriteByte(0x65) == 0)
	{
		RX_IIC_Stop();
		return 0;
	}
	
	for(i=0; i<len-1; i++)
	{
		*pdata++ = RX_IIC_ReadByte(1);
	}
	
	*pdata++ = RX_IIC_ReadByte(0);
	
	RX_IIC_Stop();
	
	return 1;
}

static unsigned char BCD2HEX(unsigned char bcd_data)  
{   
    unsigned char temp;   
    temp=(bcd_data>>4)*10 + (bcd_data&0x0f);
    return temp;
}

/*******************************************************************************
* º¯ÊýÃû	: HEX2BCD
* ÃèÊö  	: HEX×ªÎªBCD  
* ²ÎÊý  	: -hex_data:´«ÈëÊ®Áù½øÖÆ¸ñÊ½µÄÊý¾Ý
* ·µ»ØÖµ	: BCDÂë
*******************************************************************************/
static unsigned char HEX2BCD(unsigned char hex_data)  
{   
    unsigned char temp; 
    temp=((hex_data/10)<<4) + (hex_data%10);
    return temp; 	
}  


/***************************************************************************************************
*FunctionName: RTC_SetTimeData
*Description: ÉèÖÃrtcÊ±¼ä
*Input: data -- ±ØÐëÎªMyTime_Def½á¹¹µÄÖ¸Õë
*Output: 
*Author: xsx
*Date: 2016Äê9ÔÂ18ÈÕ16:11:52
***************************************************************************************************/
MyState_TypeDef RTC_SetTimeData(MyTime_Def * data)
{
	unsigned char buf[7];
	
	buf[0] = 0x20;
	RX8025_Write(0x0e, buf, 1);
	
	/*ÉèÖÃÊ±¼ä*/
	buf[0] = HEX2BCD(data->sec);
	buf[1] = HEX2BCD(data->min);
	buf[2] = HEX2BCD(data->hour);
	buf[3] = HEX2BCD(0);
	
	/*ÉèÖÃÈÕÆÚ*/
	buf[4] = HEX2BCD(data->day);
	buf[5] = HEX2BCD(data->month);
	buf[6] = HEX2BCD(data->year);
	
	if(RX8025_Write(0, buf, 7) == 0)
		return My_Fail;
	else
		return My_Pass;
}

MyState_TypeDef RTC_SetTimeData2(char * buf)
{
	unsigned short temp = 0;
	MyTime_Def temptime;
	MyState_TypeDef status = My_Fail;
	
	char * tempbuf = NULL;
	
	tempbuf = MyMalloc(64);
	
	if(tempbuf && buf)
	{
		memset(tempbuf, 0, 64);
		memcpy(tempbuf, buf, 4);
		temp = strtol(tempbuf, NULL, 10);
		if((temp >= 2000) && (temp < 2100))
			temptime.year = temp-2000;
		else
			goto END;
		
		memset(tempbuf, 0, 64);
		memcpy(tempbuf, buf+4, 2);
		temp = strtol(tempbuf, NULL, 10);
		temptime.month = temp;
			
		memset(tempbuf, 0, 64);
		memcpy(tempbuf, buf+6, 2);
		temp = strtol(tempbuf, NULL, 10);
		temptime.day = temp;
			
		memset(tempbuf, 0, 64);
		memcpy(tempbuf, buf+8, 2);
		temp = strtol(tempbuf, NULL, 10);
		temptime.hour = temp;
			
		memset(tempbuf, 0, 64);
		memcpy(tempbuf, buf+10, 2);
		temp = strtol(tempbuf, NULL, 10);
		temptime.min = temp;
			
		memset(tempbuf, 0, 64);
		memcpy(tempbuf, buf+12, 2);
		temp = strtol(tempbuf, NULL, 10);
		temptime.sec = temp;
		
		status = RTC_SetTimeData(&temptime);
	}
	
	END:
		MyFree(tempbuf);
		return status;
}

void RTC_GetTimeData(MyTime_Def * time)
{
	unsigned char buf[7];
	
	RX8025_Read(0, buf, 7);
	
	time->year = BCD2HEX(buf[6]);
    time->month = BCD2HEX(buf[5]);
    time->day = BCD2HEX(buf[4]);
    time->hour = BCD2HEX(buf[2]);
    time->min = BCD2HEX(buf[1]);
    time->sec = BCD2HEX(buf[0]);	
}



/****************************************end of file************************************************/
