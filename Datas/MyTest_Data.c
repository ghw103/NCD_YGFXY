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
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
static ItemData * (itemdata[PaiDuiWeiNum]) = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
static ItemData * CurrentItemData = NULL;
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/*在排队位中获取可用位置*/
unsigned char GetUsableLocation(void)
{
	unsigned char i=0;

	for(i=1; i<PaiDuiWeiNum; i++)
	{
		if(itemdata[i] == NULL)
			return i;
	}
	
	return 0xff;
}

unsigned char CreateANewTest(unsigned char index)
{
	unsigned char templ = GetUsableLocation();
	
	/*有空闲位置*/
	if(0xff != templ)
	{
		/*如果常规测试，且无卡在排队*/
		if(index == 0)
		{
			if(templ == 1)
				templ = 0;
			else
				return 1;
		}
		
		if(itemdata[templ] == NULL)
		{
			//有排队的卡即将测试
			if((GetMinWaitTime() > 60) || (NULL != CurrentItemData))
			{
				itemdata[templ] = (ItemData *)MyMalloc(sizeof(ItemData));
				if(itemdata[templ])
				{
					memset(itemdata[templ], 0, sizeof(ItemData));
						
					itemdata[templ]->testlocation = templ;
						
					SetCurrentTestItem(itemdata[templ]);
						
					return 0;
				}
				else
					return 2;
			}
			else
				return 3;
		}
	}

	return 1;
}




ItemData * GetTestItemByIndex(unsigned char index)
{
	return itemdata[index];
}


unsigned short GetMinWaitTime(void)
{
	unsigned char index = 0;
	unsigned short min = 0xffff;
	unsigned short temp = 0;
	
	for(index = 1; index < PaiDuiWeiNum; index++)
	{
		if((itemdata[index])&&((itemdata[index]->statues == statues3) || ((itemdata[index]->statues == statues4))))
		{
			temp = timer_surplus(&(itemdata[index]->timer));
			if(temp < min)
				min = temp;
		}
	}
	
	return min;
}



void SetCurrentTestItem(ItemData * s_itemdata)
{
	CurrentItemData = s_itemdata;
}

ItemData * GetCurrentTestItem(void)
{
	return CurrentItemData;
}

/*删除当前测试*/
MyState_TypeDef DeleteCurrentTest(void)
{
	ItemData * temp = GetCurrentTestItem();
	unsigned char i=0;
	if(temp)
	{
		MyFree(temp);
		memset(temp, 0, sizeof(ItemData));
		
		for(i=0; i<PaiDuiWeiNum; i++)
		{
			if(itemdata[i] == temp)
			{
				itemdata[i] = NULL;
				SetCurrentTestItem(itemdata[i]);
			}
		}
	}
	return My_Pass;
}

