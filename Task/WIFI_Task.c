/***************************************************************************************************
*FileName：TcpClient_Task
*Description：wifi模块数据接口任务，socket A作为用户服务器接口，SOCKET B作为超级病例服务器接口
*Author：xsx
*Data：2016年4月21日17:13:42
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/

#include	"WIFI_Task.h"
#include	"WifiFunction.h"
#include 	"Usart4_Driver.h"
#include	"SelfCheck_Data.h"

#include	"QueueUnits.h"
#include	"Net_Data.h"
#include	"MyMem.h"

#include 	"FreeRTOS.h"
#include 	"task.h"

#include	<string.h>
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

static mynetbuf S_MyNetBuf;

#define WIFITask_PRIORITY			2						//客户端任务优先级
const char * WIFITaskName = "vWIFITask";				//客户端任务名


/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/

static void vWIFITask( void *pvParameters );	//客户端自动连接任务
static void SocketAHandle(void);
static void SocketBHandle(void);

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName：StartvTcpClientTask
*Description：建立tcp 客户端任务
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月21日17:15:52
***************************************************************************************************/
void StartvWIFITask(void)
{
	xTaskCreate( vWIFITask, WIFITaskName, configMINIMAL_STACK_SIZE*2, NULL, WIFITask_PRIORITY, NULL );
}

/***************************************************************************************************
*FunctionName：vTcpClientTask
*Description：tcp 客户端任务
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月21日17:15:39
***************************************************************************************************/
static void vWIFITask( void *pvParameters )
{	
	while(1)
	{
		if(SelfCheck_OK == GetGB_SelfCheckResult())
		{
			while(1)
			{
				/*获取wifi连接状态*/
/*				if(My_Pass == WifiIsConnectted())
				{
					SetGB_WifiState(Link_Up);
					
					if(NetNone == GetGB_NetCard())
						SetGB_NetCard(Wifi_Mode);
				}
				else
				{
					SetGB_WifiState(Link_Down);
					
					if(Wifi_Mode == GetGB_NetCard())
						SetGB_NetCard(NetNone);
				}
	*/			
	//			if(Wifi_Mode == GetGB_NetCard())
				{
					/*SocketA Handle*/
					SocketAHandle();
					
					/*SocketB Handle*/
					//SocketBHandle();
					vTaskDelay(500 / portTICK_RATE_MS);
				}
	//			else
//					vTaskDelay(5000 / portTICK_RATE_MS);
			}
		}
		
		vTaskDelay(5000 / portTICK_RATE_MS);
/*		if(Link_Up == GetGB_WifiState())														//正常工作时
		{
			SocketAHandle(&mybuf);
			SocketBHandle(&mybuf);
			
			if(pdPASS == WaitWifiFree(1000*portTICK_RATE_MS))
			{
				memset(&mybuf, 0, sizeof(mynetbuf));
				mybuf.data = MyMalloc(500);
				if(mybuf.data)
				{
					q = mybuf.data;
					
					while(pdPASS == ReceiveCharFromQueue(GetUsart4RXQueue(), GetUsart4RXMutex(), q+mybuf.datalen, 10*portTICK_RATE_MS))
						mybuf.datalen++;
					
					if(mybuf.datalen > 0)
					{
						//if(pdPASS != SendDataToQueue(GetGBUserClientRXQueue(), GetGBUserClientRXMutex(), &mybuf, 1, 10/portTICK_RATE_MS, NULL))
						//	MyFree(mybuf.data);
						
						//测试
						if(pdPASS != SendDataToQueue(GetGBNCDClientTXQueue(), GetGBNCDClientTXMutex(), &mybuf, 1, 50*portTICK_RATE_MS, NULL))
							MyFree(mybuf.data);
					}
					else
						MyFree(mybuf.data);
				}
				
				SetWifiFree();
			}
		}
		else
			vTaskDelay(500*portTICK_RATE_MS);
			*/
	}
}

static void SocketAHandle(void)
{
	/*有数据发送*/
//	if(pdPASS == ReceiveDataFromQueue(GetGBNCDClientTXQueue(), GetGBNCDClientTXMutex(), &S_MyNetBuf, 1, 50 / portTICK_RATE_MS))
	if(pdPASS == ReceiveDataFromQueue(GetGBUserClientRXQueue(), GetGBUserClientRXMutex(), &S_MyNetBuf, 1, 50 / portTICK_RATE_MS))
	{
		SendDataBySocketA(&S_MyNetBuf);
		
		if(pdPASS != SendDataToQueue(GetGBUserClientTXQueue(), GetGBUserClientTXMutex(), &S_MyNetBuf, 1, 50*portTICK_RATE_MS, NULL))
			MyFree(S_MyNetBuf.data);
		//MyFree(S_MyNetBuf.data);
	}
	
/*	if(My_Pass == RevDataBySocketA(&S_MyNetBuf))
	{
		if(pdPASS != SendDataToQueue(GetGBNCDClientTXQueue(), GetGBNCDClientTXMutex(), &S_MyNetBuf, 1, 50*portTICK_RATE_MS, NULL))
			MyFree(S_MyNetBuf.data);
	}*/
}


