#ifndef __TIMEDOWN_P_H__
#define __TIMEDOWN_P_H__

#include	"Timer_Data.h"
#include	"MyTest_Data.h"

typedef struct TimeDownPage_tag {
	Timer *S_Timer;
	ItemData * currenttestdata;
}TimeDownPageData;


unsigned char DspTimeDownNorPage(void *  parm);

#endif

