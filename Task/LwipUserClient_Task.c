/***************************************************************************************************
*FileName：TcpClient_Task
*Description：LWIP的tcp客户端任务
*Author：xsx
*Data：2016年4月21日17:13:42
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/

#include	"LwipUserClient_Task.h"

#include	"Net_Data.h"
#include	"QueueUnits.h"
#include	"Net_Data.h"
#include	"SelfCheck_Data.h"
#include	"MyMem.h"

#include 	"FreeRTOS.h"
#include 	"task.h"

#include	"tcp.h"
#include 	"api.h" 

#include	<string.h>
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

#define LwipUserClientTask_PRIORITY			2						//客户端任务优先级
const char * LwipUserClientTaskName = "vLwipUserClientTask";				//客户端任务名

static struct netconn *tcp_clientconn;
static struct ip_addr server_ipaddr;

/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/

static void vLwipUserClientTask( void *pvParameters );	//客户端自动连接任务

static void ClientTXHandle(struct netconn *pxNetCon);
static void ClientRXHandle(struct netconn *pxNetCon);
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
void StartvLwipUserClientTask(void)
{
	xTaskCreate( vLwipUserClientTask, LwipUserClientTaskName, configMINIMAL_STACK_SIZE, NULL, LwipUserClientTask_PRIORITY, NULL );
}

/***************************************************************************************************
*FunctionName：vTcpClientTask
*Description：tcp 客户端任务
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月21日17:15:39
***************************************************************************************************/
static void vLwipUserClientTask( void *pvParameters )
{
	err_t err;
	while(1)
	{
		if(SelfCheck_OK == GetGB_SelfCheckResult())
		{
			IP4_ADDR(&server_ipaddr,GetGB_NetData()->serverip.ip_1,GetGB_NetData()->serverip.ip_2,GetGB_NetData()->serverip.ip_3,GetGB_NetData()->serverip.ip_4);
			
			while(1)
			{
				if(Line_Mode == GetGB_NetCard())														//当前网卡是有线网时
				{
					tcp_clientconn = netconn_new(NETCONN_TCP);
					netconn_bind(tcp_clientconn , IP_ADDR_ANY , 9002);
					err = netconn_connect(tcp_clientconn,&server_ipaddr,9602);
					tcp_clientconn->recv_timeout = 10;											//接收超时时间10ms
					
					if(err != ERR_OK)
					{
						netconn_close( tcp_clientconn );
						netconn_delete(tcp_clientconn);
					}
					else if (err == ERR_OK)
					{
						SetGB_UserServerLinkState(Link_Up);
						while(ESTABLISHED == tcp_clientconn->pcb.tcp->state)									//正常连接后任务挂起
						{
							ClientTXHandle(tcp_clientconn);
							ClientRXHandle(tcp_clientconn);
						}
						
						SetGB_UserServerLinkState(Link_Down);
						netconn_close( tcp_clientconn );
						netconn_delete(tcp_clientconn);
					}
				}
				
				vTaskDelay(10 / portTICK_RATE_MS);
			}
		}
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

/***************************************************************************************************
*FunctionName：ClientTXHandle
*Description：客户端发送数据处理
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月22日16:17:11
***************************************************************************************************/
static void ClientTXHandle(struct netconn *pxNetCon)
{
	mynetbuf sendbuf;

	memset(&sendbuf, 0, sizeof(mynetbuf));
	
	if(pdPASS == ReceiveDataFromQueue(GetGBUserClientTXQueue(), GetGBUserClientTXMutex(), &sendbuf, 1, 10*portTICK_RATE_MS))
	{
		netconn_write( pxNetCon, sendbuf.data, sendbuf.datalen, NETCONN_COPY );
		
		MyFree(sendbuf.data);
	}
}

/***************************************************************************************************
*FunctionName：ClientRXHandle
*Description：客户端接收数据处理
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月22日16:17:28
***************************************************************************************************/
static void ClientRXHandle(struct netconn *pxNetCon)
{
	err_t err;
	struct netbuf *recvbuf;
	struct pbuf *q;
	
	static mynetbuf myrecvbuf;

	err = netconn_recv(pxNetCon,&recvbuf);
	if(err == ERR_OK)
	{
		for(q=recvbuf->p; q!=NULL; q=q->next)
		{
			/*申请内存*/
			myrecvbuf.data = MyMalloc(q->len);
			if(myrecvbuf.data)
			{
				memset(myrecvbuf.data, 0, q->len);
				
				/*保存数据*/
				memcpy(myrecvbuf.data, q->payload, q->len);
				/*保存数据长度*/
				myrecvbuf.datalen = q->len;
				
				/*正常*/
				if(pdPASS != SendDataToQueue(GetGBUserClientRXQueue(), GetGBUserClientRXMutex(), &myrecvbuf, 1, 10*portTICK_RATE_MS, NULL))
				//if(pdPASS != SendDataToQueue(GetGBUserClientTXQueue(), GetGBUserClientTXMutex(), &myrecvbuf, 1, 10*portTICK_RATE_MS, NULL))
					MyFree(myrecvbuf.data);
			}
		}
		
		netbuf_delete(recvbuf);
	}
}
