#ifndef __SLEEP_P_H__
#define __SLEEP_P_H__

#include	"UI_Data.h"
#include	"Define.h"
#include	"Timer_Data.h"

typedef struct SleepPageBuffer_tag {
	Basic_ICO myBasicICO;
	unsigned short x;
	unsigned char x_direct;
	unsigned short y;
	unsigned char y_direct;
	unsigned int count;
	MyTime_Def time;
	char buf[50];
	unsigned short lcdinput[20];
}SleepPageBuffer;


MyState_TypeDef createSleepActivity(Activity * thizActivity, Intent * pram);

#endif

