#ifndef __SD_F_H__
#define __SD_F_H__

#include	"Define.h"
#include	"MyTest_Data.h"

/*读写操作人*/
MyState_TypeDef SaveUserData(User_Type * user);
MyState_TypeDef ReadUserData(User_Type * user);

/*WIFI设置*/
MyState_TypeDef SaveWifiData(WIFI_Def * wifi);
MyState_TypeDef ReadWifiData(WIFI_Def * wifi);
MyState_TypeDef ClearWifiData(WIFI_Def * wifi);

/*老化数据*/
MyState_TypeDef SaveReTestData(ReTestData *retestdata, unsigned char type);

#endif

