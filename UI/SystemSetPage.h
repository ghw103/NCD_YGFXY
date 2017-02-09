#ifndef __SYSSET_P_H__
#define __SYSSET_P_H__

#include	"UI_Data.h"
#include	"SystemSet_Data.h"

typedef struct SysSetPageBuffer_tag {
	SystemSetData systemSetData;
	unsigned char pressCnt;
	Timer timer;
	unsigned short lcdinput[100];
}SysSetPageBuffer;

MyState_TypeDef createSystemSetActivity(Activity * thizActivity, Intent * pram);

#endif

