#ifndef __SDEVICEINFO_P_H__
#define __SDEVICEINFO_P_H__

#include	"SystemSet_Data.h"
#include	"UI_Data.h"

typedef struct ShowDeviceInfoPageBuffer_Tag
{
	unsigned char presscount;
	SystemSetData systemSetData;
	unsigned short lcdinput[100];
}ShowDeviceInfoPageBuffer;

MyState_TypeDef createDeviceInfoActivity(Activity * thizActivity, Intent * pram);

#endif

