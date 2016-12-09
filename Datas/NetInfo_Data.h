/****************************************file start****************************************************/
#ifndef NETINFO_D_H
#define NETINFO_D_H

#include	"Define.h"

#pragma pack(1)
typedef struct
{
	IP_Def LineIP;										//有线网ip
	unsigned char LineMAC[6];							//有线网MAC
	unsigned char LineStatus;							//有线网链接状态，0 -- 无网络， 1 -- 有网络
	
	IP_Def WifiIP;										//wifi的ip
	char WifiSSID[30];									//所链接wifi的名字
	unsigned char WifiMAC[6];							//wifi模块的mac
	unsigned char WifiIndicator;						//所连接wifi的信号
}NetInfo_Type; 
#pragma pack()


void GetGB_NetInfo(NetInfo_Type * netinfo);
void SetGB_LineNetIP(unsigned int ip);
void SetGB_LineNetMac(unsigned char * mac);
void SetGB_LineNetStatus(unsigned char status);
void SetGB_WifiSSID(char * ssid);
void SetGB_WifiIP(IP_Def * ip);
void SetGB_WifiMAC(unsigned char * mac);
void SetGB_WifiIndicator(unsigned char indicator);
	
#endif

/****************************************end of file************************************************/
