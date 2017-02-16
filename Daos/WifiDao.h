/****************************************file start****************************************************/
#ifndef _WIFI_D_H
#define	_WIFI_D_H

#include	"Define.h"

MyState_TypeDef SaveWifiData(WIFI_Def * wifi);
MyState_TypeDef ReadWifiData(WIFI_Def * wifi);
MyState_TypeDef deleteWifi(WIFI_Def * wifi);
MyState_TypeDef ClearWifi(void);
#endif

/****************************************end of file************************************************/
