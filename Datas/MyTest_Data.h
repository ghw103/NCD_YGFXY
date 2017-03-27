#ifndef _MYTEST_S_D_H__
#define _MYTEST_S_D_H__

#include	"Define.h"

typedef enum
{ 
	Error_OK = 0,																	//创建成功
	Error_Mem = 1,																	//内存错误
	Error_PaiDuiBusy = 2,															//排队测试忙，禁止再次创建批量测试
	Error_StopNormalTest = 3,														//批量测试中，禁止常规测试
	Error_PaiduiFull = 4,															//排队位满
	Error_PaiduiTesting = 5,														//测试中
	Error_PaiduiDisconnect = 6														//排队模块失联
}CreateTestErrorType;



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
bool isInTimeOutStatus(ItemData * itemData);
void SetCurrentTestItem(ItemData * s_itemdata);
ItemData * GetCurrentTestItem(void);

void SetPaiduiUser(User_Type * user);
void GetPaiduiUser(User_Type * user);

#endif

