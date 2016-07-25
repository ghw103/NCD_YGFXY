#ifndef __WAITCARD_P_H__
#define __WAITCARD_P_H__

#include	"Timer_Data.h"
#include	"MyTest_Data.h"

typedef struct WaitPage_tag {
	Timer timer;				//等待插卡时间
	Timer timer2;				//间隔提示插卡
	ItemData * currenttestdata;
}WaitPageData;

unsigned char DspWaittingCardPage(void *  parm);

#endif

