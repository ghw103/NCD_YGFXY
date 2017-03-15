#ifndef __SLEEP_P_H__
#define __SLEEP_P_H__

#include	"UI_Data.h"
#include	"Define.h"
#include	"Timer_Data.h"
#include	"SystemSet_Data.h"

typedef struct SleepPageBuffer_tag {
	unsigned short lcdinput[20];
	SystemSetData systemSetData;
}SleepPageBuffer;


MyState_TypeDef createSleepActivity(Activity * thizActivity, Intent * pram);

#endif

