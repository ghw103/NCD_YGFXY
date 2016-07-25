/***************************************************************************************************
*FileName:GetSampleIDFun
*Description:采集样品ID
*Author:xsx
*Data:2016年4月26日21:24:20
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"GetSampleIDFun.h"
#include	"Usart1_Driver.h"
#include	"MyMem.h"
#include	"QueueUnits.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
static xQueueHandle xSampleIDQueue = NULL;						//将读取到的数据的地址往外发送
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static void AnalysisData(void *pbuf , unsigned short len);
static MyState_TypeDef SendSampleIDData(void * data);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

void ScannerInit(void)
{
	if(xSampleIDQueue == NULL)
		xSampleIDQueue = xQueueCreate( 10, 4);								//指针的大小为4个字节，最多保存10个指针
}

/***************************************************************************************************
*FunctionName：GetOutScanerData
*Description：获取样品ID功能
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月26日21:24:53
***************************************************************************************************/
void GetOutScanerData(void)
{
	char *buf = NULL;
	unsigned short RXCount = 0;
	
	buf = MyMalloc(100);
	if(buf == NULL)
		return;
	
	memset(buf, 0, 100);

	while(pdPASS == ReceiveCharFromQueue(GetUsart1RXQueue(), GetUsart1RXMutex(), (buf+RXCount) , 100 * portTICK_RATE_MS))	
		RXCount++;

	
	if(RXCount > 0)
		AnalysisData(buf, RXCount);
	
	MyFree(buf);
}

/***************************************************************************************************
*FunctionName：AnalysisData
*Description：处理接收的数据
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月26日21:25:21
***************************************************************************************************/
static void AnalysisData(void *pbuf , unsigned short len)
{
	unsigned char *tempbuf = NULL;
	
	tempbuf = MyMalloc(len+10);
	if(tempbuf)
	{
		memset(tempbuf, 0, len+10);
		memcpy(tempbuf, pbuf, len);
		
		if(My_Fail == SendSampleIDData(&tempbuf))
			MyFree(tempbuf);
	}
}

/***************************************************************************************************
*FunctionName：GiveSampleIDStateMutex
*Description：发送通知
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月11日19:24:37
***************************************************************************************************/
static MyState_TypeDef SendSampleIDData(void * data)
{
	if(xSampleIDQueue == NULL)
		return My_Fail;
	
	if(pdPASS == xQueueSend( xSampleIDQueue, data, 10*portTICK_RATE_MS ))
		return My_Pass;
	else
		return My_Fail;
}

/***************************************************************************************************
*FunctionName：TakeSampleIDStateMutex
*Description：读取样品编号改变
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月11日19:24:37
***************************************************************************************************/
MyState_TypeDef TakeSampleIDData(void * data)
{
	if(xSampleIDQueue == NULL)
		return My_Fail;
	
	if(pdPASS == xQueueReceive( xSampleIDQueue, data, 10*portTICK_RATE_MS))
		return My_Pass;
	else
		return My_Fail;	
}
