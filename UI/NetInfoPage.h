#ifndef __NETINFO_P_H__
#define __NETINFO_P_H__

#include	"Define.h"
#include	"NetInfo_Data.h"

typedef struct NetInfoPageBuffer_tag {
	unsigned short lcdinput[20];
	Timer timer;
	NetInfo_Type netinfo;
	char tempbuffer1[100];
	Basic_ICO lineico;
	Basic_ICO wifiico;
}NetInfoPageBuffer;


unsigned char DspNetInfoPage(void *  parm);

#endif

