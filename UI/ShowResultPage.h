#ifndef __SHOWRESULT_P_H__
#define __SHOWRESULT_P_H__

#include	"MyTest_Data.h"
#include	"UI_Data.h"
#include	"SystemSet_Data.h"

typedef struct ShowPageBuffer_tag {
	char tempbuf[100];
	Basic_ICO myico[3];
	TestData testdata;
	LineInfo lineinfo;
	unsigned short lcdinput[100];
}ShowPageBuffer;

MyState_TypeDef createShowResultActivity(Activity * thizActivity, Intent * pram);

#endif

