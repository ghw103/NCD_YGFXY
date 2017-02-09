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
	Error_PaiduiFull = 4,															//排队位满
	Error_PaiduiTesting = 5															//测试中
}CreateTestErrorType;

typedef struct ItemData_tag {
	TestData testdata;
	Timer timer;																	//常规倒计时计时器
	Timer timer2;																	//超时倒计时计时器
	Timer timer3;																	//超时倒计时计时器
	unsigned char errorcount;														//未操作次数
	MyPaiDuiStatues statues;														//测试阶段
	unsigned char jieduan;															//测试阶段
	unsigned char testlocation;
	unsigned char varIcoIndex;														//排队界面显示卡的图标界面的索引
	unsigned short ledLight;														//测试时led的亮度，为系统设置中保存的校准led值
}ItemData;

typedef struct TestBuffer_tag{
	ItemData * NormalTestDataBuffer;												//常规测试缓冲区
	ItemData * (PaiduiTestDataBuffer[PaiDuiWeiNum]);								//8个排队位测试缓冲区
	User_Type PaiduiUser;															//排队模式共用的操作人
	ItemData * CurrentTestDataBuffer;												//当前测试
}TestBuffer;

bool IsPaiDuiTestting(void);
CreateTestErrorType CreateANewTest(TestType testtype);
MyState_TypeDef DeleteCurrentTest(void);
ItemData * GetTestItemByIndex(unsigned char index);
unsigned short GetMinWaitTime(void);

void SetCurrentTestItem(ItemData * s_itemdata);
ItemData * GetCurrentTestItem(void);

void SetPaiduiUser(User_Type * user);
void GetPaiduiUser(User_Type * user);

#endif

