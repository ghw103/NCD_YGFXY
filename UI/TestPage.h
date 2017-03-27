#ifndef __TEST_P_H__
#define __TEST_P_H__

#include	"MyTest_Data.h"
#include	"SystemSet_Data.h"
#include	"Test_Fun.h"
#include	"UI_Data.h"

typedef struct TestPageBuffer_tag {
	ItemData * currenttestdata;
	Basic_ICO myico[3];
	LineInfo line;
	unsigned short lcdinput[100];
	unsigned char testisover;
	char buf[100];
	TestData testDataForPrintf;							//复制一份测试数据，给打印用
	unsigned char isPrintfData;							//是否正在打印
}TestPageBuffer;


MyState_TypeDef createTestActivity(Activity * thizActivity, Intent * pram);

#endif

