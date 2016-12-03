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

#include	"MyMem.h"
#include	"string.h"
#include 	"FreeRTOS.h"
#include 	"task.h"
/***************************************************************************************************/
/**************************************变量*************************************************/
/***************************************************************************************************/
//常规测试缓存
static ItemData * GB_NormalTestDataBuffer = NULL;
//批量测试缓存
static ItemData * (GB_PaiduiTestDataBuffer[PaiDuiWeiNum]) = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
//当前操作的数据
static ItemData * GB_CurrentTestDataBuffer = NULL;
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
		if(GB_PaiduiTestDataBuffer[i] != NULL)
			return true;
	}
	
	return false;
}

CreateTestErrorType CreateANewTest(TestType testtype)
{
	unsigned char i=0;
	
	//如果是常规测试
	if(testtype == NormalTestType)
	{
		//如果批量测试功能在使用中，则禁止常规测试
		if(true == IsPaiDuiTestting())
			return Error_StopNormalTest;
		
		//申请测试内存
		GB_NormalTestDataBuffer = (ItemData *)MyMalloc(sizeof(ItemData));
		
		//内存申请失败
		if(GB_NormalTestDataBuffer == NULL)
			return Error_Mem;
		else
		{
			GB_CurrentTestDataBuffer = GB_NormalTestDataBuffer;
			memset(GB_NormalTestDataBuffer, 0, sizeof(ItemData));
			return Error_OK;
		}
	}
	else
	{
		for(i=0; i<PaiDuiWeiNum; i++)
		{
			if(GB_PaiduiTestDataBuffer[i] == NULL)
			{
				//申请测试内存
				GB_PaiduiTestDataBuffer[i] = (ItemData *)MyMalloc(sizeof(ItemData));
				
				//内存申请失败
				if(GB_PaiduiTestDataBuffer[i] == NULL)
					return Error_Mem;
				else
				{
					GB_CurrentTestDataBuffer = GB_PaiduiTestDataBuffer[i];
					memset(GB_CurrentTestDataBuffer, 0, sizeof(ItemData));
					return Error_OK;
				}
			}
		}
		
		return Error_PaiDuiBusy;
	}
}




ItemData * GetTestItemByIndex(unsigned char index)
{
	return GB_PaiduiTestDataBuffer[index];
}


unsigned short GetMinWaitTime(void)
{
	unsigned char index = 0;
	unsigned short min = 0xffff;
	unsigned short temp = 0;
	
	for(index = 0; index < PaiDuiWeiNum; index++)
	{
		if((GB_PaiduiTestDataBuffer[index])&&((GB_PaiduiTestDataBuffer[index]->statues == statues3) || ((GB_PaiduiTestDataBuffer[index]->statues == statues4))))
		{
			temp = timer_surplus(&(GB_PaiduiTestDataBuffer[index]->timer));
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
	GB_CurrentTestDataBuffer = s_itemdata;
}

ItemData * GetCurrentTestItem(void)
{
	return GB_CurrentTestDataBuffer;
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
	ItemData * temp = GetCurrentTestItem();
	unsigned char i=0;
	if(temp)
	{
		memset(temp, 0, sizeof(ItemData));
		MyFree(temp);

		for(i=0; i<PaiDuiWeiNum; i++)
		{
			if(GB_PaiduiTestDataBuffer[i] == temp)
			{
				GB_PaiduiTestDataBuffer[i] = NULL;
				SetCurrentTestItem(NULL);
			}
		}
	}
	return My_Pass;
}

