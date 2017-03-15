/***************************************************************************************************
*FileName : TestLinks_Data
*Description: 测试数据链表
*Author:xsx
*Data: 2016年5月14日17:14:02
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"MyTest_Data.h"
#include	"SystemSet_Data.h"

#include	"MyMem.h"
#include	"string.h"
#include 	"FreeRTOS.h"
#include 	"task.h"
/***************************************************************************************************/
/**************************************变量*************************************************/
/***************************************************************************************************/
//测试数据缓冲区
static TestBuffer GB_TestBuffer = {
	.NormalTestDataBuffer = NULL,
	.PaiduiTestDataBuffer = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	.CurrentTestDataBuffer = NULL,
};
/***************************************************************************************************/
/**************************************内部函数*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName: IsPaiDuiTestting
*Description: 检查是否当前在进行批量测试
*Input: None
*Output: None
*Return: 	true -- 有卡在排队测试中
*			false -- 没有进行批量测试
*Author: xsx
*Date: 2016年12月2日15:10:07
***************************************************************************************************/
bool IsPaiDuiTestting(void)
{
	unsigned char i=0;

	for(i=0; i<PaiDuiWeiNum; i++)
	{
		if(GB_TestBuffer.PaiduiTestDataBuffer[i] != NULL)
			return true;
	}
	
	return false;
}

CreateTestErrorType CreateANewTest(TestType testtype)
{
	unsigned char i=0;
	
	if(GB_TestBuffer.CurrentTestDataBuffer != NULL)
		return Error_PaiduiTesting;
	
	//如果是常规测试
	if(testtype == NormalTestType)
	{
		//如果批量测试功能在使用中，则禁止常规测试
		if(true == IsPaiDuiTestting())
			return Error_StopNormalTest;
		
		//申请测试内存
		GB_TestBuffer.NormalTestDataBuffer = (ItemData *)MyMalloc(sizeof(ItemData));
		
		//内存申请失败
		if(GB_TestBuffer.NormalTestDataBuffer == NULL)
			return Error_Mem;
		else
		{
			GB_TestBuffer.CurrentTestDataBuffer = GB_TestBuffer.NormalTestDataBuffer;
			memset(GB_TestBuffer.CurrentTestDataBuffer, 0, sizeof(ItemData));
			GB_TestBuffer.NormalTestDataBuffer->testlocation = 0;
			
			//从系统设置数据中获取测试时led的亮度值
			GB_TestBuffer.NormalTestDataBuffer->ledLight = getTestLedLightIntensity(getGBSystemSetData());
			
			return Error_OK;
		}
	}
	else
	{
		if(60 > GetMinWaitTime())
			return Error_PaiDuiBusy;
		
		for(i=0; i<PaiDuiWeiNum; i++)
		{
			if(GB_TestBuffer.PaiduiTestDataBuffer[i] == NULL)
			{
				//申请测试内存
				GB_TestBuffer.PaiduiTestDataBuffer[i] = (ItemData *)MyMalloc(sizeof(ItemData));
				
				//内存申请失败
				if(GB_TestBuffer.PaiduiTestDataBuffer[i] == NULL)
					return Error_Mem;
				else
				{
					GB_TestBuffer.CurrentTestDataBuffer = GB_TestBuffer.PaiduiTestDataBuffer[i];
					memset(GB_TestBuffer.CurrentTestDataBuffer, 0, sizeof(ItemData));
					GB_TestBuffer.CurrentTestDataBuffer->testlocation = i + 1;
					
					//复制排队共用操作人到当前测试数据中，如果是第一次创建排队，后面选择操作人则会覆盖此次操作
					memcpy(&(GB_TestBuffer.CurrentTestDataBuffer->testdata.user), &(GB_TestBuffer.PaiduiUser), sizeof(User_Type));
					
					//从系统设置数据中获取测试时led的亮度值
					GB_TestBuffer.CurrentTestDataBuffer->ledLight = getTestLedLightIntensity(getGBSystemSetData());
					
					return Error_OK;
				}
			}
		}
		
		return Error_PaiduiFull;
	}
}




ItemData * GetTestItemByIndex(unsigned char index)
{
	return GB_TestBuffer.PaiduiTestDataBuffer[index];
}


unsigned short GetMinWaitTime(void)
{
	unsigned char index = 0;
	unsigned short min = 0xffff;
	unsigned short temp = 0;
	
	for(index = 0; index < PaiDuiWeiNum; index++)
	{
		if((GB_TestBuffer.PaiduiTestDataBuffer[index])&&
			(timerIsStartted(&(GB_TestBuffer.PaiduiTestDataBuffer[index]->timer)))&&
			(false == timerIsStartted(&(GB_TestBuffer.PaiduiTestDataBuffer[index]->timer2))))
		{
			temp = timer_surplus(&(GB_TestBuffer.PaiduiTestDataBuffer[index]->timer));
			if(temp < min)
				min = temp;
		}
	}
	
	return min;
}


/***************************************************************************************************
*FunctionName: SetCurrentTestItem, GetCurrentTestItem
*Description: 读写当前测试缓存
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月2日15:54:25
***************************************************************************************************/
void SetCurrentTestItem(ItemData * s_itemdata)
{
	GB_TestBuffer.CurrentTestDataBuffer = s_itemdata;
}

ItemData * GetCurrentTestItem(void)
{
	return GB_TestBuffer.CurrentTestDataBuffer;
}

/***************************************************************************************************
*FunctionName: DeleteCurrentTest
*Description: 删除当前测试
*Input: None
*Output: None
*Return: My_Pass -- 删除成功
*Author: xsx
*Date: 2016年12月2日15:55:07
***************************************************************************************************/
MyState_TypeDef DeleteCurrentTest(void)
{
	if(GB_TestBuffer.CurrentTestDataBuffer)
	{
		if(GB_TestBuffer.CurrentTestDataBuffer->testlocation == 0)
			GB_TestBuffer.NormalTestDataBuffer = NULL;
		else
			GB_TestBuffer.PaiduiTestDataBuffer[GB_TestBuffer.CurrentTestDataBuffer->testlocation-1] = NULL;

		MyFree(GB_TestBuffer.CurrentTestDataBuffer);
		
		GB_TestBuffer.CurrentTestDataBuffer = NULL;
	}
	return My_Pass;
}

void SetPaiduiUser(User_Type * user)
{
	if(user)
	{
		memcpy(&(GB_TestBuffer.PaiduiUser), user, sizeof(User_Type));
	}
}

void GetPaiduiUser(User_Type * user)
{
	if(user)
	{
		memcpy(user, &(GB_TestBuffer.PaiduiUser), sizeof(User_Type));
	}
}

