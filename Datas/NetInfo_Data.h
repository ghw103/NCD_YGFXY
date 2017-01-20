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
}NetInfo_Type; 
#pragma pack()


void GetGB_NetInfo(NetInfo_Type * netinfo);
void SetGB_LineNetIP(unsigned int ip);
void SetGB_LineNetMac(unsigned char * mac);
void SetGB_LineNetStatus(unsigned char status);
	
#endif

/****************************************end of file************************************************/
