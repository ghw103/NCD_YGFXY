#ifndef __ADJUSTLED_P_H__
#define __ADJUSTLED_P_H__

#include	"UI_Data.h"
#include	"MyTest_Data.h"
#include	"SystemSet_Data.h"

typedef struct AdjustLedPageBuffer_tag {
	unsigned short lcdinput[20];
	char buf[100];
	bool isTestting;									//是否正在测试中
	ResultState cardpretestresult;
	ItemData itemData;
	unsigned short targetValue;							//目标值
	unsigned short targetRange;							//误差值
	unsigned short maxPoint[2];							//0--最大值，1--最大值位置
	unsigned short i;
	unsigned char testCnt;								//查找次数
	SystemSetData systemSetData;						//系统参数副本，用于保存失败回滚
}AdjustLedPageBuffer;


MyState_TypeDef createAdjustLedActivity(Activity * thizActivity, Intent * pram);

#endif

