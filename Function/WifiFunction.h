#ifndef __WIFI_F_H__
#define __WIFI_F_H__

#include	"Define.h"
#include	"Net_Data.h"

typedef enum
{ 
	None = 0,
	AT_Mode = 1,
	Normal_Mode = 2,
	PWM_Mode = 3,
}WIFI_WorkMode_DefType;

MyState_TypeDef WifiModuleInit(void);
MyState_TypeDef WIFICheck(void);

MyState_TypeDef SetWifiDefaultWorkMode(void);
WIFI_WorkMode_DefType GetWifiWorkMode(void);
MyState_TypeDef SetWifiWorkInAT(WIFI_WorkMode_DefType mode);
MyState_TypeDef ScanApList(WIFI_Def *wifis);
MyState_TypeDef ConnectWifi(WIFI_Def *wifis);
MyState_TypeDef RestartWifi(void);

MyState_TypeDef SetWifiSocketA(void);
MyState_TypeDef GetWifiSocketAState(void);
MyState_TypeDef CloseSocketA(void);
MyState_TypeDef SendDataBySocketA(mynetbuf *netbuf);
MyState_TypeDef RevDataBySocketA(mynetbuf *netbuf);

MyState_TypeDef SetWifiSocketB(void);
MyState_TypeDef GetWifiSocketBState(void);
MyState_TypeDef CloseSocketB(void);

MyState_TypeDef CheckWifiMID(void);
unsigned char GetWifiIndicator(void);
MyState_TypeDef WifiIsConnectted(void);

void InitMutex(void);
unsigned char WaitWifiFree(portTickType xBlockTime);
void SetWifiFree(void);
#endif

