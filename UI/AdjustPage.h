#ifndef __ADJUST_P_H__
#define __ADJUST_P_H__

#include	"Timer_Data.h"
#include	"MyTest_Data.h"
#include	"SystemSet_Data.h"
#include	"Test_Fun.h"
#include	"Define.h"
#include	"UI_Data.h"

typedef struct AdjustPageBuffer_tag {

	ItemData itemData;
	ScanCodeResult scancode;
	ResultState cardTestResult;
	unsigned char testErrorCount;
	AdjustData adjustData;								//新数据
	SystemSetData systemSetData;						//系统参数副本，用于保存失败回滚
	float targetresult;
	bool isAdjjing;										//是否正在校准中
	char buf[100];
	unsigned short lcdinput[20];
	double adjResult;									//校准结果
}AdjustPageBuffer;

MyState_TypeDef createAdjActivity(Activity * thizActivity, Intent * pram);

#endif

