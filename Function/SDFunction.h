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
MyState_TypeDef ReadTestData(TestData *tempdata, unsigned int index, unsigned char readnum, unsigned short *br);
MyState_TypeDef SaveDateInfo(TestData *tempdata, unsigned int index);
MyState_TypeDef ReadDateInfo(TestDateInfo_Def *tempdata, MyTime_Def *testtime);
MyState_TypeDef WriteUpLoadIndex(unsigned int index);
MyState_TypeDef ReadUpLoadIndex(unsigned int *index);

/*ip设置*/
MyState_TypeDef SaveNetData(NetData * netdata);
MyState_TypeDef ReadNetData(NetData * netdata);

/*WIFI设置*/
MyState_TypeDef SaveWifiData(WIFI_Def * wifi);
MyState_TypeDef ReadWifiData(WIFI_Def * wifi);
MyState_TypeDef ClearWifiData(WIFI_Def * wifi);

/*校准数据*/
MyState_TypeDef SaveAdjustData(AdjustData *adjustdata);
MyState_TypeDef ReadAdjustData(AdjustData *adjustdata);
#endif

