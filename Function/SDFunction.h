#ifndef __SD_F_H__
#define __SD_F_H__

#include	"Define.h"
#include	"MyTest_Data.h"

MyState_TypeDef CheckSDFunction(void);

/*读写设备信息*/
MyState_TypeDef SaveDeviceInfo(DeviceInfo * deviceinfo);
MyState_TypeDef ReadDeviceInfo(DeviceInfo * deviceinfo);

/*读写操作人*/
MyState_TypeDef SaveUserData(User_Type * user);
void ReadUserData(User_Type * user);

/*测试数据*/
MyState_TypeDef SaveTestData(TestData *tempdata);
MyState_TypeDef ReadTestData(TestData *tempdata, unsigned short index, unsigned char readnum);

/*ip设置*/
MyState_TypeDef SaveNetData(NetData * netdata);
MyState_TypeDef ReadNetData(NetData * netdata);

/*WIFI设置*/
MyState_TypeDef SaveWifiData(WIFI_Def * wifi);
MyState_TypeDef ReadWifiData(WIFI_Def * wifi);
MyState_TypeDef ClearWifiData(WIFI_Def * wifi);
#endif

