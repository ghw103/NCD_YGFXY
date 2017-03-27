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
#include	"System_Data.h"
#include 	"Usart4_Driver.h"
#include	"SystemSet_Data.h"
#include	"NetInfo_Data.h"
#include	"AppFileDao.h"
#include	"IAP_Fun.h"
#include	"RemoteSoft_Data.h"
#include	"WifiFunction.h"
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

void CommunicateWithServerByLineNet(MyServerData * myServerData)
{
	err_t err;
	struct pbuf *p = NULL;
	unsigned char i=0;
	
	SetGB_LineNetStatus(0);
	
	IP4_ADDR(&myServerData->server_ipaddr,GB_ServerIp_1, GB_ServerIp_2, GB_ServerIp_3, GB_ServerIp_4);

	//创建连接
	myServerData->clientconn = netconn_new(NETCONN_TCP);
	//创建失败
	if(myServerData->clientconn == NULL)
		return;

	//绑定本地ip
	err = netconn_bind(myServerData->clientconn, IP_ADDR_ANY, 0);
	//连接失败
	if(err != ERR_OK)
		goto END2;

	//尝试连接远程服务器
	err = netconn_connect(myServerData->clientconn, &myServerData->server_ipaddr, GB_ServerPort);
	//连接失败
	if(err != ERR_OK)
		goto END2;
		
	//设置接收数据超时时间100MS
	myServerData->clientconn->recv_timeout = 2000;
		
		//发送数据
	err = netconn_write(myServerData->clientconn, myServerData->sendBuf, myServerData->sendDataLen, NETCONN_COPY );
	//发送失败
	if(err != ERR_OK)
		goto END1;
		
	//接收数据
	while(ERR_OK == netconn_recv(myServerData->clientconn, &myServerData->recvbuf))
	{
		SetGB_LineNetStatus(1);
		//如果发生的是GET请求，则说明是下载固件，需要保存
		if(strstr(myServerData->sendBuf, "GET"))
		{
			p = myServerData->recvbuf->p;
			if(i == 0)
			{
				i++;
				WriteAppFile((unsigned char *)(p->payload)+229, p->len-229, true);
			}
			else
				WriteAppFile(p->payload, p->len, false);
			
			while(p->next)
			{
				p = p->next;
				WriteAppFile(p->payload, p->len, false);
			}
			
			vTaskDelay(10 / portTICK_RATE_MS);
		}
		else
		{
			myServerData->recvDataLen += netbuf_copy(myServerData->recvbuf, myServerData->recvBuf + myServerData->recvDataLen ,
				SERVERRECVBUFLEN - myServerData->recvDataLen);
		}
		
		netbuf_delete(myServerData->recvbuf);
	}
		
	END1:
		netconn_close(myServerData->clientconn);
		netconn_delete(myServerData->clientconn);
		return;
		
	END2:
		netconn_delete(myServerData->clientconn);
		return;
}

void CommunicateWithServerByWifi(MyServerData * myServerData)
{
	unsigned short i = 0;
	unsigned short readSize = 0;
	portTickType queueBlockTime;
	
	if(My_Pass == takeWifiMutex(1000 / portTICK_RATE_MS))
	{
		//清空队列数据
		while(pdPASS == ReceiveDataFromQueue(GetUsart4RXQueue(), NULL, myServerData->recvBuf, 1000, 
				&readSize, 1, 10 / portTICK_RATE_MS, 1 / portTICK_RATE_MS));
		
		//发送数据
		if(My_Pass == SendDataToQueue(GetUsart4TXQueue(), NULL, myServerData->sendBuf, myServerData->sendDataLen,
			1, 1000 / portTICK_RATE_MS, 10 / portTICK_RATE_MS, EnableUsart4TXInterrupt))
		{
			
			//接收数据,最好等待1s
			
			while(pdPASS == ReceiveDataFromQueue(GetUsart4RXQueue(), NULL, myServerData->recvBuf, 1000, 
				&readSize, 1, 1000 / portTICK_RATE_MS, 1000 / portTICK_RATE_MS))
			{
				//如果发生的是GET请求，则说明是下载固件，需要保存
				if(strstr(myServerData->sendBuf, "GET"))
				{
					if(i == 0)
					{
						WriteAppFile(myServerData->recvBuf + 229, readSize-229, true);
						i++;
					}
					else
						WriteAppFile(myServerData->recvBuf, readSize, false);
					
					myServerData->recvDataLen += readSize;
				}
			}
		}
		
		giveWifixMutex();
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
MyState_TypeDef UpLoadData(char *URL, void * sendBuf, unsigned short sendLen, void * recvBuf, unsigned short recvLen,
	char * sendType)
{
	MyState_TypeDef statues = My_Fail;
	MyServerData * myServerData = NULL;
	
	myServerData = MyMalloc(sizeof(MyServerData));

	if(myServerData)
	{
		memset(myServerData, 0, sizeof(MyServerData));
		if(strstr(sendType, "POST"))
			sprintf(myServerData->sendBuf, "POST %s HTTP/1.1\nHost: 116.62.108.201:8080\nConnection: keep-alive\nContent-Length: %d\nContent-Type:application/x-www-form-urlencoded;charset=GBK\nAccept-Language: zh-CN,zh;q=0.8\n\n%s", URL, sendLen, (char *)sendBuf);
		else
			sprintf(myServerData->sendBuf, "GET %s HTTP/1.1\nHost: 116.62.108.201:8080\nConnection: keep-alive\n\n", URL);
		
		myServerData->sendDataLen = strlen(myServerData->sendBuf);
			
		CommunicateWithServerByLineNet(myServerData);
		{
			if(strstr(sendType, "POST"))
			{
				myServerData->myp = strstr(myServerData->recvBuf, "success");
				if(myServerData->myp)
				{
					memset(recvBuf, 0, recvLen);
					memcpy(recvBuf, myServerData->myp, recvLen);

					statues = My_Pass;
					goto END1;
				}
			}
			else
			{
				if(My_Pass == checkNewFirmwareIsSuccessDownload())
				{
					statues = My_Pass;
					setIsSuccessDownloadFirmware(true);
					goto END1;
				}
			}
		}
		
		memset(myServerData->recvBuf, 0, SERVERRECVBUFLEN);
		CommunicateWithServerByWifi(myServerData);
		{
			if(strstr(sendType, "POST"))
			{
				myServerData->myp = strstr(myServerData->recvBuf, "success");
				if(myServerData->myp)
				{
					memset(recvBuf, 0, recvLen);
					memcpy(recvBuf, myServerData->myp, recvLen);
					statues = My_Pass;
				}
			}
			else
			{
				if(My_Pass == checkNewFirmwareIsSuccessDownload())
				{
					setIsSuccessDownloadFirmware(true);
					statues = My_Pass;
				}
			}
		}
		
		END1:
			;
	}
	
	MyFree(myServerData);
	
	return statues;
}

