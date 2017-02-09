#ifndef __SETDEVICEID_P_H__
#define __SETDEVICEID_P_H__

#include	"SystemSet_Data.h"
#include	"UI_Data.h"

typedef struct SetDeviceIDPage_tag {
	SystemSetData systemSetData;
	unsigned char tempbuf[150];								//临时指针
	char deviceId[MaxDeviceIDLen];							//新输入的设备id
	unsigned char ismodify;
	unsigned short lcdinput[100];
}SetDeviceIDPage;

MyState_TypeDef createSetDeviceIDActivity(Activity * thizActivity, Intent * pram);

#endif

