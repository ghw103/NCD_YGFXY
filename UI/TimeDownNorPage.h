#ifndef __TIMEDOWN_P_H__
#define __TIMEDOWN_P_H__

#include	"Timer_Data.h"
#include	"MyTest_Data.h"
#include	"UI_Data.h"

typedef struct TimeDownPage_tag {
	Timer *S_Timer;
	ItemData * currenttestdata;
	unsigned short count;
	unsigned short time;
}TimeDownPageData;


MyState_TypeDef createTimeDownActivity(Activity * thizActivity, Intent * pram);

#endif

