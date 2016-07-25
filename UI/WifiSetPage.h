#ifndef __WIFISET_P_H__
#define __WIFISET_P_H__

#include	"Define.h"


typedef struct WifiPageBuffer_tag {
	unsigned char pageindex;							//Ò³ÃæË÷Òý
	unsigned char selectindex;
	WIFI_Def wifilist[MaxWifiListNum];
	WIFI_Def * wifip;
	unsigned short wifinum;
	Basic_ICO myico[PageWifiNum];
}WifiPageBuffer;

unsigned char DspWifiSetPage(void *  parm);

#endif

