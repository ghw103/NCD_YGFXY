/***************************************************************************************************
*FileName：CodeScanner_Driver
*Description：二维码扫描器驱动
*Author：xsx
*Data：2016年4月24日18:09:18
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/

#include	"CodeScanner_Driver.h"
#include	"Usart2_Driver.h"
#include	"QueueUnits.h"
#include	"Delay.h"

#include	"MyMem.h"
#include	"CRC_CCITT.h"

#include	<string.h>
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName：CodeScanner_GPIOInit
*Description：二维码扫描器io初始化
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月24日18:12:44
***************************************************************************************************/
void CodeScanner_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(Trig_Rcc, ENABLE);

  	GPIO_InitStructure.GPIO_Pin = Trig_Pin; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(Trig_GpioGroup, &GPIO_InitStructure);
	
	/*关闭二维码扫描器*/
  	CloseCodeScanner();
}

/***************************************************************************************************
*FunctionName：OpenCodeScanner
*Description：打开二维码扫描器，开始扫描
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月24日18:14:54
***************************************************************************************************/
void OpenCodeScanner(void)
{
	GPIO_WriteBit(Trig_GpioGroup, Trig_Pin, Bit_RESET);
	delay_ms(1);
}
void CloseCodeScanner(void)
{
	GPIO_WriteBit(Trig_GpioGroup, Trig_Pin, Bit_SET);
	delay_ms(1);
}

/***************************************************************************************************
*FunctionName：ErWeiMaSelfTest
*Description：测试二维码是否正常
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月2日16:59:09
***************************************************************************************************/
MyState_TypeDef ErWeiMaSelfTest(void)
{
	unsigned char *temp1, *temp2;
	unsigned char *p;
	unsigned short *tempdata = NULL;
	unsigned char i=0, j=0;
	MyState_TypeDef result = My_Fail;
	
	temp1 = MyMalloc(50);
	temp2 = MyMalloc(50);
	tempdata = MyMalloc(sizeof(unsigned short)*20);
	
	if((temp1 == NULL)||(temp2 == NULL)||(tempdata == NULL))
		goto END;
	
	memset(temp1, 0, 50);
	memset(temp2, 0, 50);
	memset(tempdata, 0, sizeof(unsigned short)*20);
	
	/*读取波特率*/
	p = temp1;
/*	*p++ = 0x7e;
	*p++ = 0x00;
	*p++ = 0x07;
	*p++ = 0x01;
	*p++ = 0x00;
	*p++ = 0x2a;
	*p++ = 0x02;
	*p++ = 0xd8;
	*p++ = 0x0f;*/
	
	*p++ = 0x7e;
	*p++ = 0x00;
	*p++ = 0x07;
	*p++ = 0x01;
	*p++ = 0x00;
	*p++ = 0x0a;
	*p++ = 0x01;
	*p++ = 0xee;
	*p++ = 0xa8;

	for(i=0; i<3; i++)
	{
		if(pdPASS == SendStrToQueue(GetUsart2TXQueue(), GetUsart2TXMutex(),temp1, 9, 500/portTICK_RATE_MS, EnableUsart2TXInterrupt))
		{
			p = temp2;
			while(pdPASS == ReceiveCharFromQueue(GetUsart2RXQueue(), GetUsart2RXMutex(), p+j, 50/portTICK_RATE_MS))
					j++;
			
			if(j > 0)
			{
				tempdata[1] = temp2[5];
				tempdata[1] = (tempdata[1]<<8) + temp2[4];
				j = 0;
				if(tempdata[1] == 0x0139)
				{
					result = My_Pass;
					goto END;
				}
			}
		}	
	}
	
	END:
		MyFree(temp1);
		MyFree(temp2);
		MyFree(tempdata);
		return result;
}



