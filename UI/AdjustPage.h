#ifndef __ADJUST_P_H__
#define __ADJUST_P_H__

#include	"Timer_Data.h"
#include	"MyTest_Data.h"
#include	"Test_Fun.h"
#include	"Define.h"

typedef struct AdjustPageBuffer_tag {
	Timer timer;
	unsigned char step;
	ItemData testdata;
	ScanCodeResult scancode;
	ResultState cardpretestresult;
	AdjustData tempadjust;
	float targetresult;
	char buf[100];
	unsigned short lcdinput[100];
	unsigned char error;
}AdjustPageBuffer;

unsigned char DspAdjustPage(void *  parm);

#endif

