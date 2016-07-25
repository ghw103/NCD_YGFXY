#ifndef __PRERCARD_P_H__
#define __PRERCARD_P_H__

#include	"Define.h"
#include	"MyTest_Data.h"
#include	"Test_Fun.h"

typedef struct PreReadPageBuffer_tag {
	ItemData * currenttestdata;
	ResultState cardpretestresult;
	ScanCodeResult scancode;
	Timer timer2;
	char buf[100];
}PreReadPageBuffer;

unsigned char DspPreReadCardPage(void *  parm);

#endif

