/***************************************************************************************************
*FileName:Net_Data
*Description:网络配置数据
*Author:xsx
*Data:2016年5月8日16:41:34
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"Net_Data.h"
#include	"MyMem.h"
#include	"CRC16.h"
#include 	"task.h"

#include	<string.h>
#include	"stdio.h"
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
static NetCard_Type GB_NetCard = NetNone;										//当前网卡

static LinkState_TypeDef GB_LinkState = Link_Down;								//有线网状态
static LinkState_TypeDef GB_WifiState = Link_Down;								//wifi工作状态

static LinkState_TypeDef GB_NCDServerLinkState = Link_Down;						//纽康度服务器连接状态
static LinkState_TypeDef GB_UserServerLinkState = Link_Down;					//用户服务器连接状态

static NetData GB_NetData;														//设备网络数据

/*超级病例*/
static xQueueHandle xRxQueue1 = NULL;											//接收队列
static xSemaphoreHandle xRxQueueMutex1 = NULL;									//接收互斥量
static xQueueHandle xTxQueue1 = NULL;											//发送队列
static xSemaphoreHandle xTxQueueMutex1 = NULL;									//发送互斥量

/*用户自己服务器*/
static xQueueHandle xRxQueue2 = NULL;											//接收队列
static xSemaphoreHandle xRxQueueMutex2 = NULL;									//接收互斥量
static xQueueHandle xTxQueue2 = NULL;											//发送队列
static xSemaphoreHandle xTxQueueMutex2 = NULL;									//发送互斥量
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
*FunctionName：SetGB_NetCard,GetGB_NetCard
*Description：标记当前采用的网卡种类
*Input：None
*Output：None
*Author：xsx
*Data：2016年7月21日19:44:59
***************************************************************************************************/
void SetGB_NetCard(NetCard_Type netcard)
{
	GB_NetCard = netcard;
}

NetCard_Type GetGB_NetCard(void)
{
	return GB_NetCard;
}

/***************************************************************************************************
*FunctionName：SetGB_LinkState
*Description：更新网线连接状态
*Input：None
*Output：None
*Author：xsx
*Data：2016年3月9日15:16:33
***************************************************************************************************/
void SetGB_LinkState(LinkState_TypeDef linkstate)
{
	GB_LinkState = linkstate;
}

LinkState_TypeDef GetGB_LinkState(void)
{
	return GB_LinkState;
}

/***************************************************************************************************
*FunctionName：SetGB_WifiState,GetGB_WifiState
*Description：wifi连接状态
*Input：None
*Output：None
*Author：xsx
*Data：2016年7月21日19:08:25
***************************************************************************************************/
void SetGB_WifiState(LinkState_TypeDef linkstate)
{
	GB_WifiState = linkstate;
}

LinkState_TypeDef GetGB_WifiState(void)
{
	return GB_WifiState;
}

/***************************************************************************************************
*FunctionName：SetGB_NCDServerLinkState， GetGB_NCDServerLinkState
*Description：纽康度服务器的连接状态
*Input：None
*Output：None
*Author：xsx
*Data：2016年7月22日19:37:40
***************************************************************************************************/
void SetGB_NCDServerLinkState(LinkState_TypeDef linkstate)
{
	GB_NCDServerLinkState = linkstate;
}

LinkState_TypeDef GetGB_NCDServerLinkState(void)
{
	return GB_NCDServerLinkState;
}

/***************************************************************************************************
*FunctionName：SetGB_UserServerLinkState， GetGB_UserServerLinkState
*Description：用户服务器的连接状态
*Input：None
*Output：None
*Author：xsx
*Data：2016年7月22日19:37:20
***************************************************************************************************/
void SetGB_UserServerLinkState(LinkState_TypeDef linkstate)
{
	GB_UserServerLinkState = linkstate;
}

LinkState_TypeDef GetGB_UserServerLinkState(void)
{
	return GB_UserServerLinkState;
}

/***************************************************************************************************
*FunctionName：SetGB_NetData,GetGB_NetData
*Description：服务器数据
*Input：None
*Output：None
*Author：xsx
*Data：
***************************************************************************************************/
void SetGB_NetData(NetData netdata)
{
	GB_NetData = netdata;
}

NetData * GetGB_NetData(void)
{
	return &GB_NetData;
}

NetIP_Type GetNteIPMode(void)
{
	if(GB_NetData.crc == CalModbusCRC16Fun1(&GB_NetData, sizeof(NetData)-2))
		return GB_NetData.ipmode;
	else
		return DHCP_Mode;
}

void GetMyIP(IP_Def * myip)
{
	if(GB_NetData.crc == CalModbusCRC16Fun1(&GB_NetData, sizeof(NetData)-2))
		memcpy(myip, &(GB_NetData.myip), sizeof(IP_Def));
	else
	{
		myip->ip_1 = 192;
		myip->ip_2 = 168;
		myip->ip_3 = 1;
		myip->ip_4 = 127;
	}
}
void GetServerIP(IP_Def * serverip)
{
	if(GB_NetData.crc == CalModbusCRC16Fun1(&GB_NetData, sizeof(NetData)-2))
		memcpy(serverip, &(GB_NetData.serverip), sizeof(IP_Def));
}


void CreateNetQueue(void)
{
	/*超级病例服务器*/
	if(xTxQueue1 == NULL)
		xTxQueue1 = xQueueCreate( 10, ( unsigned portBASE_TYPE ) sizeof( mynetbuf ) );
	if(xTxQueueMutex1 == NULL)
		vSemaphoreCreateBinary(xTxQueueMutex1);
	
	if(xRxQueue1 == NULL)
		xRxQueue1 = xQueueCreate( 10, ( unsigned portBASE_TYPE ) sizeof( mynetbuf ) );
	if(xRxQueueMutex1 == NULL)
		vSemaphoreCreateBinary(xRxQueueMutex1);
	
	/*用户服务器*/
	if(xTxQueue2 == NULL)
		xTxQueue2 = xQueueCreate( 10, ( unsigned portBASE_TYPE ) sizeof( mynetbuf ) );
	if(xTxQueueMutex2 == NULL)
		vSemaphoreCreateBinary(xTxQueueMutex2);
	
	if(xRxQueue2 == NULL)
		xRxQueue2 = xQueueCreate( 10, ( unsigned portBASE_TYPE ) sizeof( mynetbuf ) );
	if(xRxQueueMutex2 == NULL)
		vSemaphoreCreateBinary(xRxQueueMutex2);
}

/*获取纽康度服务器数据接口*/
xQueueHandle GetGBNCDClientTXQueue(void)
{
	return xTxQueue1;
}

xSemaphoreHandle GetGBNCDClientTXMutex(void)
{
	return xTxQueueMutex1;
}

xQueueHandle GetGBNCDClientRXQueue(void)
{
	return xRxQueue1;
}

xSemaphoreHandle GetGBNCDClientRXMutex(void)
{
	return xRxQueueMutex1;
}

/*获取用户服务器数据接口*/
xQueueHandle GetGBUserClientTXQueue(void)
{
	return xTxQueue2;
}

xSemaphoreHandle GetGBUserClientTXMutex(void)
{
	return xTxQueueMutex2;
}

xQueueHandle GetGBUserClientRXQueue(void)
{
	return xRxQueue2;
}

xSemaphoreHandle GetGBUserClientRXMutex(void)
{
	return xRxQueueMutex2;
}

