#ifndef __TEST_P_H__
#define __TEST_P_H__

#include	"MyTest_Data.h"
#include	"SystemSet_Data.h"
#include	"Test_Fun.h"
#include	"UI_Data.h"

typedef struct TestPageBuffer_tag {
	ItemData * currenttestdata;
	ResultState cardpretestresult;				//测试结果
	Basic_ICO myico[3];
	LineInfo line;
	unsigned short lcdinput[100];
	unsigned char testisover;
	char buf[100];
	unsigned char isPrintfData;							//是否正在打印
}TestPageBuffer;


MyState_TypeDef createTestActivity(Activity * thizActivity, Intent * pram);

#endif

