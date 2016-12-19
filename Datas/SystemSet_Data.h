/****************************************file start****************************************************/
#ifndef SYSTEMSET_D_H
#define	SYSTEMSET_D_H

#include	"Define.h"


#pragma pack(1)
typedef struct SystemSetData_Tag
{
	DeviceInfo deviceInfo;												//设备信息
	NetSet netSet;														//有线网设置
	bool isAutoPrint;													//是否自动打印
	bool isMute;														//是否静音
	unsigned short ledSleepTime;										//休眠时间 s
	unsigned char ledLightIntensity;									//屏幕亮度 0-100
	unsigned short crc;
}SystemSetData;
#pragma pack()

void setDefaultSetData(SystemSetData * systemSetData);

void getSystemSetData(SystemSetData * systemSetData);
void setSystemSetData(SystemSetData * systemSetData);

bool deviceInfoIsNew(void);
void getDeviceInfo(DeviceInfo * deviceinfo);
void getNetSet(NetSet * netSet);
bool isAutoPrint(void);
bool isMute(void);
unsigned short getLedSleepTime(void);
unsigned char getLedLightIntensity(void);
	
#endif

/****************************************end of file************************************************/
