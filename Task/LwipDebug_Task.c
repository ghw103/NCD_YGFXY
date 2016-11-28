/***************************************************************************************************
*FileName：TcpClient_Task
*Description：LWIP的tcp客户端任务
*Author：xsx
*Data：2016年4月21日17:13:42
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/

#include	"LwipDebug_Task.h"

#include	"QueueUnits.h"
#include	"MyMem.h"
#include	"Define.h"

#include 	"FreeRTOS.h"
#include 	"task.h"

#include	"tcp.h"
#include 	"api.h" 

#include	<string.h>
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

#define LwipDebugTask_PRIORITY			2						//客户端任务优先级
const char * LwipDebugTaskName = "vLwipDebugTask";				//客户端任务名

static struct netconn *tcp_clientconn;
static struct ip_addr server_ipaddr;

static xQueueHandle xDebugDataQueue = NULL ;

/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/

static void vLwipDebugTask( void *pvParameters );	//客户端自动连接任务
static MyState_TypeDef DebugTXHandle(struct netconn *pxNetCon);
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
void StartvLwipDebugTask(void)
{
	xDebugDataQueue = xQueueCreate(10, sizeof(mynetbuf));
	
	xTaskCreate( vLwipDebugTask, LwipDebugTaskName, configMINIMAL_STACK_SIZE, NULL, LwipDebugTask_PRIORITY, NULL );
}

/***************************************************************************************************
*FunctionName：vTcpClientTask
*Description：tcp 客户端任务
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月21日17:15:39
***************************************************************************************************/
static void vLwipDebugTask( void *pvParameters )
{
	err_t err;
	
	IP4_ADDR(&server_ipaddr, 192,168,0,34);
			
	while(1)
	{
		tcp_clientconn = netconn_new(NETCONN_TCP);
		if(tcp_clientconn != NULL)
		{
			err = netconn_connect(tcp_clientconn,&server_ipaddr,9600);
			tcp_clientconn->recv_timeout = 10;											//接收超时时间10ms
						
			if(err != ERR_OK)
			{
				netconn_close( tcp_clientconn );
				netconn_delete(tcp_clientconn);
			}
			else if (err == ERR_OK)
			{
				while(1)									//正常连接后任务挂起
				{
					if(My_Fail == DebugTXHandle(tcp_clientconn))
						break;
				}
							
				netconn_close( tcp_clientconn );
				netconn_delete(tcp_clientconn);
			}
		}
		
		
		vTaskDelay(10 / portTICK_RATE_MS);
	}
				
	vTaskDelay(10 / portTICK_RATE_MS);
}


xQueueHandle GetLwipDebugQueue(void)
{
	return xDebugDataQueue;
}

/***************************************************************************************************
*FunctionName：ClientTXHandle
*Description：客户端发送数据处理
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月22日16:17:11
***************************************************************************************************/
static MyState_TypeDef DebugTXHandle(struct netconn *pxNetCon)
{
	mynetbuf sendbuf;
	err_t err;

	memset(&sendbuf, 0, sizeof(mynetbuf));
	
	if(pdPASS == ReceiveDataFromQueue(xDebugDataQueue, NULL, &sendbuf, 1, sizeof(mynetbuf), 10*portTICK_RATE_MS))
	{
		err = netconn_write( pxNetCon, sendbuf.data, sendbuf.datalen, NETCONN_COPY );
		
		if(err != ERR_OK)
			return My_Fail;
	}
	
	return My_Pass;
}

/***************************************************************************************************
*FunctionName：ClientRXHandle
*Description：客户端接收数据处理
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月22日16:17:28
***************************************************************************************************/
static MyState_TypeDef DebugRXHandle(struct netconn *pxNetCon)
{
	err_t err;
	struct netbuf *recvbuf;
	struct pbuf *q;
	unsigned char *myp = NULL;
	static mynetbuf myrecvbuf;

	err = netconn_recv(pxNetCon,&recvbuf);
	if(err == ERR_OK)
	{
		q=recvbuf->p;
		myrecvbuf.data = MyMalloc(q->tot_len);
		myrecvbuf.datalen = q->tot_len;
		
		if(myrecvbuf.data)
		{
			myp = myrecvbuf.data;
			
			for(; q!=NULL; q=q->next)
			{
				memcpy(myp, q->payload, q->len);
				myp += q->len;
			}
			
			/*正常*/
			if(pdPASS != SendDataToQueue(xDebugDataQueue, NULL, &myrecvbuf, 1, sizeof(mynetbuf), 10 / portTICK_RATE_MS, NULL))
			//if(pdPASS != SendDataToQueue(GetGBUserClientTXQueue(), GetGBUserClientTXMutex(), &myrecvbuf, 1, 10*portTICK_RATE_MS, NULL))
				MyFree(myrecvbuf.data);
		}
		
		netbuf_delete(recvbuf);
	}
}
