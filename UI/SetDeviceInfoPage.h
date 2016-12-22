#ifndef __SDI_P_H__
#define __SDI_P_H__

#include	"SystemSet_Data.h"
#include	"UI_Data.h"

typedef struct SetDeviceInfoPageBuffer_tag {
	SystemSetData systemSetData;
	unsigned char ismodify;
	unsigned short lcdinput[100];
}SetDeviceInfoPageBuffer;


MyState_TypeDef createSetDeviceInfoActivity(Activity * thizActivity, Intent * pram);

#endif

