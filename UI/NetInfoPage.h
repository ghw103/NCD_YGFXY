#ifndef __NETINFO_P_H__
#define __NETINFO_P_H__

#include	"Define.h"
#include	"NetInfo_Data.h"
#include	"UI_Data.h"
#include	"SystemSet_Data.h"

typedef struct NetInfoPageBuffer_tag {
	unsigned short lcdinput[20];
	Timer timer;
	NetInfo_Type netinfo;
	char tempbuffer1[100];
	Basic_ICO lineico;
	Basic_ICO wifiico;
	SystemSetData systemSetData;
	Timer timer2;
	
	IP_Def WifiIP;										//wifi的ip
	char WifiSSID[30];									//所链接wifi的名字
	char WifiMAC[13];									//wifi模块的mac
	unsigned char WifiIndicator;						//所连接wifi的信号
	
}NetInfoPageBuffer;


MyState_TypeDef createNetInfoActivity(Activity * thizActivity, Intent * pram);

#endif

