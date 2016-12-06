#ifndef _MYTEST_S_D_H__
#define _MYTEST_S_D_H__

#include	"Define.h"
#include	"Motor_Data.h"
#include	"Timer_Data.h"
#include	"Temperature_Data.h"



typedef struct ItemData_tag {
	TestData testdata;
	Timer timer;																		//常规倒计时计时器
	Timer timer2;																		//超时倒计时计时器
	Timer timer3;																		//超时倒计时计时器
	unsigned char errorcount;															//未操作次数
	MyPaiDuiStatues statues;															//测试阶段
	unsigned char jieduan;																//测试阶段
	unsigned char testlocation;
	
}ItemData;

unsigned char GetUsableLocation(void);
unsigned char CreateANewTest(unsigned char index);
MyState_TypeDef DeleteCurrentTest(void);
ItemData * GetTestItemByIndex(unsigned char index);
unsigned short GetMinWaitTime(void);

void SetCurrentTestItem(ItemData * s_itemdata);
ItemData * GetCurrentTestItem(void);
#endif
