/***************************************************************************************************
*FileName:
*Description:
*Author:
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"ServerFun.h"
#include	"QueueUnits.h"
#include	"Net_Data.h"

#include	"MyMem.h"

#include	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"
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

void SendDataToNCDServer(char *buf, unsigned short len)
{
	mynetbuf mybuf;
	
	mybuf.data = MyMalloc(len);
	if(mybuf.data)
	{
		memcpy(mybuf.data, buf, len);
		mybuf.datalen = len;
		
		if(pdPASS != SendDataToQueue(GetGBNCDClientTXQueue(), GetGBNCDClientTXMutex(), &mybuf, 1, 10 / portTICK_RATE_MS, NULL))
			MyFree(mybuf.data);
	}
}
MyState_TypeDef RecvDataFromNCDServer(char *buf)
{
	static unsigned char num = 0;
	mynetbuf mybuf;
	
	if(buf)
	{
		num = 0;
		while(1)
		{
			if(pdPASS == ReceiveDataFromQueue(GetGBNCDClientRXQueue(), GetGBNCDClientRXMutex(), &mybuf, 1, 1000/portTICK_RATE_MS))
			{
				memcpy(buf, mybuf.data, mybuf.datalen);
				MyFree(mybuf.data);
				
				return My_Pass;
			}
			
			num++;
			if(num > 10)
				break;
		}
	}
	
	return My_Fail;
}

void SendDataToUserServer(char *buf, unsigned short len)
{
	mynetbuf mybuf;
	
	mybuf.data = MyMalloc(len);
	if(mybuf.data)
	{
		memcpy(mybuf.data, buf, len);
		mybuf.datalen = len;
		
		if(pdPASS != SendDataToQueue(GetGBUserClientTXQueue(), GetGBUserClientTXMutex(), &mybuf, 1, 10 / portTICK_RATE_MS, NULL))
			MyFree(mybuf.data);
	}
}


/***************************************************************************************************
*FunctionName：
*Description：
*Input：None
*Output：None
*Author：xsx
*Data：
***************************************************************************************************/
MyState_TypeDef UpLoadData(char *URL, void * buf, unsigned short buflen)
{
	char *data = NULL;
	MyState_TypeDef statues = My_Fail;
	
	data = MyMalloc(buflen+1024);
	if(data)
	{
		memset(data, 0, buflen+1024);
		sprintf(data, "POST %s HTTP/1.1\nHost: 123.57.94.39\nConnection: keep-alive\nContent-Length: %d\nContent-Type:application/x-www-form-urlencoded;charset=GBK\nReferer: %s\n\n%s", URL, buflen, URL, (char *)buf);
		SendDataToNCDServer(data, strlen(data));
		SendDataToUserServer(data, strlen(data));
		/*如果创建成功*/
		memset(data, 0, buflen+1024);
		if((My_Pass == RecvDataFromNCDServer(data)) && (strstr(data, "CREATED")))
			statues = My_Pass;
	}
	MyFree(data);
	
	return statues;
}

