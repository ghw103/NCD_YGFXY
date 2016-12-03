#ifndef _MYTEST_S_D_H__
#define _MYTEST_S_D_H__

#include	"Define.h"
#include	"Motor_Data.h"
#include	"Timer_Data.h"

typedef enum
{ 
	Error_OK = 0,																	//创建成功
	Error_Mem = 1,																	//内存错误
	Error_PaiDuiBusy = 2,															//排队测试忙，禁止再次创建批量测试
	Error_StopNormalTest = 3,														//批量测试中，禁止常规测试
}CreateTestErrorType;

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

bool IsPaiDuiTestting(void);
CreateTestErrorType CreateANewTest(TestType testtype);
MyState_TypeDef DeleteCurrentTest(void);
ItemData * GetTestItemByIndex(unsigned char index);
unsigned short GetMinWaitTime(void);

void SetCurrentTestItem(ItemData * s_itemdata);
ItemData * GetCurrentTestItem(void);
#endif
