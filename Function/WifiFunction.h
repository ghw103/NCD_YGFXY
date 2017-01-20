#ifndef __WIFI_F_H__
#define __WIFI_F_H__

#include	"Define.h"


typedef enum
{ 
	None = 0,
	AT_Mode = 1,
	Normal_Mode = 2,
	PWM_Mode = 3,
}WIFI_WorkMode_DefType;

MyState_TypeDef WIFIInit(void);

WIFI_WorkMode_DefType GetWifiWorkMode(void);
MyState_TypeDef SetWifiWorkInAT(WIFI_WorkMode_DefType mode);
MyState_TypeDef ScanApList(WIFI_Def *wifis);
MyState_TypeDef ConnectWifi(WIFI_Def *wifis);
MyState_TypeDef RestartWifi(void);
MyState_TypeDef GetWifiStaMac(char *mac);
MyState_TypeDef GetWifiStaIP(IP_Def * ip);
MyState_TypeDef CheckWifiMID(void);
unsigned char GetWifiIndicator(void);
MyState_TypeDef WifiIsConnectted(char * ssid);

#endif

