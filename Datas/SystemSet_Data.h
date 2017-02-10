/****************************************file start****************************************************/
#ifndef SYSTEMSET_D_H
#define	SYSTEMSET_D_H

#include	"Define.h"

#define		MaxAdjDataNum	50											//最多保存50个品种的校准参数

#pragma pack(1)
typedef struct SystemSetData_Tag
{
	DeviceInfo deviceInfo;												//设备信息
	NetSet netSet;														//有线网设置
	bool isAutoPrint;													//是否自动打印
	bool isMute;														//是否静音
	unsigned short ledSleepTime;										//休眠时间 s
	unsigned char ledLightIntensity;									//屏幕亮度 0-100
	unsigned int testDataNum;											//测试数据已保存的数目
	unsigned int upLoadIndex;											//测试数据等待上传的索引
	unsigned short testLedLightIntensity;								//测试时采用的led亮度值，为校准后产生，默认为200
	AdjustData adjustData[MaxAdjDataNum];
	unsigned short crc;
}SystemSetData;
#pragma pack()

void setDefaultSystemSetData(SystemSetData * systemSetData);

void copyGBSystemSetData(SystemSetData * systemSetData);
SystemSetData * getGBSystemSetData(void);
void setSystemSetData(SystemSetData * systemSetData);

bool deviceInfoIsNew(void);
void getDeviceInfo(DeviceInfo * deviceinfo);
void getNetSet(NetSet * netSet);
bool isAutoPrint(void);
bool isMute(void);
unsigned short getLedSleepTime(void);
unsigned char getLedLightIntensity(void);

void plusTestDataTotalNum(unsigned char num);
unsigned int getTestDataTotalNum(void);

void setUpLoadIndex(unsigned int index);

void setTestLedLightIntensity(SystemSetData * systemSetData, unsigned short value);
unsigned short getTestLedLightIntensity(SystemSetData * systemSetData);

MyState_TypeDef addAdjPram(SystemSetData * systemSetData, AdjustData * adjData);
void getAdjPram(SystemSetData * systemSetData, AdjustData * adjData);

void setWifiIsUseable(bool isUseable);
bool isWifiUseable(void);

#endif

/****************************************end of file************************************************/
