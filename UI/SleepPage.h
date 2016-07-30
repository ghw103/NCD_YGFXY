#ifndef __SLEEP_P_H__
#define __SLEEP_P_H__

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
}SleepPageBuffer;


unsigned char DspSleepPage(void *  parm);

#endif

