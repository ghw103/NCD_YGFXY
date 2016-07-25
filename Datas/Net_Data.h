#ifndef _NET_S_D_H__
#define _NET_S_D_H__

#include	"FreeRTOS.h"
#include 	"queue.h"
#include	"semphr.h"
#include	"Define.h"


void SetGB_NetCard(NetCard_Type netcard);
NetCard_Type GetGB_NetCard(void);

void SetGB_LinkState(LinkState_TypeDef linkstate);
LinkState_TypeDef GetGB_LinkState(void);

void SetGB_WifiState(LinkState_TypeDef linkstate);
LinkState_TypeDef GetGB_WifiState(void);

void SetGB_NetData(NetData netdata);
NetData * GetGB_NetData(void);

void SetGB_NCDServerLinkState(LinkState_TypeDef linkstate);
LinkState_TypeDef GetGB_NCDServerLinkState(void);

void SetGB_UserServerLinkState(LinkState_TypeDef linkstate);
LinkState_TypeDef GetGB_UserServerLinkState(void);

void CreateNetQueue(void);

/*获取纽康度服务器数据接口*/
xQueueHandle GetGBNCDClientTXQueue(void);
xSemaphoreHandle GetGBNCDClientTXMutex(void);
xQueueHandle GetGBNCDClientRXQueue(void);
xSemaphoreHandle GetGBNCDClientRXMutex(void);

/*获取用户服务器数据接口*/
xQueueHandle GetGBUserClientTXQueue(void);

xSemaphoreHandle GetGBUserClientTXMutex(void);
xQueueHandle GetGBUserClientRXQueue(void);
xSemaphoreHandle GetGBUserClientRXMutex(void);
#endif

